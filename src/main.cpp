#include <iostream>
#include <string>
#include "chess/chess.hpp"
#include <fstream>
// Current game object, recycled every new game
class GameProfilerVisitor : public chess::pgn::Visitor {
private:
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
};

 
		
		
int main() {
    std::ios::sync_with_stdio(false);
    std::cout << "test failed successfully"; 
    std::ifstream pgn_file("/Users/mihirvedanti/GPawn-Analyzer/extern/testpgn.pgn");
    if (!pgn_file.is_open()){
	    std::cerr <<"failed to open the pgn files, you suck, ur damn pathing skills u bum";
    }
    GameProfilerVisitor myVisitor;
    //note to self: once i get stuff up and running, ill have to instantiate a way to pipeline blocks of games into different visitor classes
    chess::pgn::StreamParser Parser(static_cast<std::istream&>(pgn_file)); 
    Parser.readGames(myVisitor);
    return 0;
    

}

