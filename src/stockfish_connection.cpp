#include "stockfish_connection.hpp"
#include <iostream> //stderr
#include <unistd.h> //fork, pipe, dup2, read, write, close
#include <sys/types.h> //pid
#include <cstring> //string manipulation for later
#include <sys/wait.h>
		   

StockfishConnection::StockfishConnection(std::function<void(const std::string&)> line_callback):
	on_line_received(line_callback){}

StockfishConnection::~StockfishConnection()
{	
	disconnect();
}

bool StockfishConnection::connect(std::string& path_to_stockfish){
	int parent_to_child[2];
	int child_to_parent[2];

	if(pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) //invalid piping
	{
		std::cerr << "pipe incorrectly created" << std::endl;
		return 1;
	}
	engine_pid = fork();
	if (engine_pid == -1)//invalid forking
	{
		std::cerr << "protocols running invalidly fr" << std::endl;
		return 1;
	}
	if (engine_pid == 0) //Stockfish process
	{
		dup2(child_to_parent[1], STDOUT_FILENO); //stockfish process write through stdout
		dup2(parent_to_child[0], STDIN_FILENO); //stockfish processes commands from stdin 
		
		close(parent_to_child[0]);
		close(parent_to_child[1]);
		close(child_to_parent[0]);
		close(child_to_parent[1]);

		std::vector<char> path_buffer(path_to_stockfish.begin(), path_to_stockfish.end());
		path_buffer.push_back('\0'); // Ensure null-termination

		char* args[] = {path_buffer.data(), nullptr};
		execvp(args[0], args);
	
	}
	else
	{
		
		read_fd = child_to_parent[0];
		write_fd = parent_to_child[1];

		close(child_to_parent[1]);
		close(parent_to_child[0]);

		is_running.store(true);

		reader_thread = std::thread(&StockfishConnection::read_loop, this);
		return true;
	
	}
	return false;
}
void StockfishConnection::send_message(std::string& message){
	if (!is_running.load() || write_fd == -1 )return;

	std::string formatted = message + "\n";
	write(write_fd, formatted.c_str(), message.length());
}
void StockfishConnection::read_loop() {
    char buffer[4096];
    std::string current_line;

    while (is_running.load()) {
        ssize_t bytes_read = read(read_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            // Engine disconnected or read error occurred, didnt feel like not using break;
            break;
        }

        buffer[bytes_read] = '\0';
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') {
                if (!current_line.empty() && current_line.back() == '\r') {
                    current_line.pop_back();
                }
                if (on_line_received) {
                    on_line_received(current_line);
                }
                current_line.clear();
            } else {
                current_line += buffer[i];
            }
        }
    }
    is_running.store(false);
}

void StockfishConnection::disconnect()
{
	is_running.store(false); //tells atomic that thread has closed
	if (write_fd != -1) //disconnects file descriptors
	{
		close(write_fd);
		write_fd = -1;
	}
	if (read_fd != -1)
	{
		close(read_fd);
		read_fd = -1;
	}
	if(reader_thread.joinable()) //closes stockfish thread
	{
		reader_thread.join();
	}

	if(engine_pid == 0) //if in child process
	{
		printf("Child process executing.\n");
		exit(1);
	}
	else
	{
		wait(NULL); //waits for child process to terminate
		printf("Child process has executed.\n");
	}
	
}


	








