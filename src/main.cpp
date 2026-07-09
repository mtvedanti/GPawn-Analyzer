#include <iostream>
#include <string>
#include "chess/chess.hpp"
#include <fstream>
#include "stockfish_connection.hpp"
// Current game object, recycled every new game
class GameProfilerVisitor : public chess::pgn::Visitor {
private:
	std::string uci = "uci";
	std::string isready = "isready";
	int ply_count = 0; //number of half moves played
public:

	//automatically is called when parser detects any metadata vals
	void header(std::string_view key, std::string_view value) override {
	        if (key == "White" || key == "Black") {
        	    std::cout << key << ": " << value << "\n";
        	}
    	}	
        //automatically is called when parser detects new game
	void startMoves() override {
        	ply_count = 0;
    	}
	//automatically is called when parser detects a move input
	void move(std::string_view move_san, std::string_view comment) override {
		ply_count++; 
		std::cout <<"Ply: " << ply_count << "- " << move_san << "\n";
	}

	void startPgn() override{}
	void endPgn() override {
	
		std::cout << "Sucessfully finished parsing game and metadata. \n\n";
	}
        struct game_data {
		int ply = 0; //counts the number of half moves. 
		double avg_cpl = 0; //counts the average of the centipawn loss. 1/n *(sum)( cpl(n))
		bool is_mate = false;
		int mate_in = 0; //can only be non zero if is_mate ==1
	};

};

 
		
		
int main() {
    std::string UCI = "uci";
    std::string ISREADY= "isready";
    std::string FILEPATH_TO_STOCKFISH = "./stockfish";

    std::ios::sync_with_stdio(false);
    std::cout << "test failed successfully"; 
    std::ifstream pgn_file("/Users/mihirvedanti/GPawn-Analyzer/extern/testpgn.pgn");

    if (!pgn_file.is_open()){
	    std::cerr <<"failed to open the pgn files, you suck, ur damn pathing skills u bum";
    }
    GameProfilerVisitor myVisitor;
    //note to self: once i get stuff up and running, ill have to instantiate a way to pipeline blocks of games into different visitor classes
    chess::pgn::StreamParser Parser(static_cast<std::istream&>(pgn_file)); 
    Parser.readGames(myVisitor); //takes in pgn file and orders it into nice plys for the uci

    StockfishConnection sf([](const std::string& line)
    {
	    std::cout << "[Stockfish Out]: " << line << "\n";
    });
    if (!sf.connect(FILEPATH_TO_STOCKFISH))
    {
	    std::cerr << "Stockfish $FILEPATH not found \n";
	    return -1;
    }
    sf.send_message(UCI);
    sf.send_message(ISREADY);



}
	    
		    

    
    return 0;
    

}

