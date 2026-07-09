#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <sys/types.h>

class StockfishConnection {
	public: 
		explicit StockfishConnection(std::function<void(const std::string&)> line_callback); //whenever a background thread reads a line from stockfish, this function will automatically be called
		
		~StockfishConnection(); //guarantees process termination
		

		StockfishConnection(const StockfishConnection&) = delete;
		StockfishConnection& operator =(const StockfishConnection&) = delete; //prevents copying of this class to reduce overhead
		

		bool connect(std::string& path_to_stockfish) ;//connects child process stockfish via pipes, forks, and execvp
		void send_message(std::string& message);
		void disconnect();
		bool is_connected() const {return is_running.load(); }

		
	private:
		
		void read_loop();
		int write_fd = -1;
		int read_fd = -1;
		pid_t engine_pid{-1};

		std::thread reader_thread;
		std::atomic<bool> is_running{false};

		std::function<void(const std::string&)> on_line_received; //lambda callback function that sends messages to the interpreter
		};

