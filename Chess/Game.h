#ifndef GAME_H
#define GAME_H

#include <iostream>
#include "Piece.h"
#include "Board.h"
#include "Exceptions.h"

namespace Chess
{

	class Game {

	public:
		// This default constructor initializes a board with the standard
		// piece positions, and sets the state to white's turn
		Game();

		// Returns true if it is white's turn
		/////////////////////////////////////
		// DO NOT MODIFY THIS FUNCTION!!!! //
		/////////////////////////////////////
		bool turn_white() const { return is_white_turn; }
    
    /////////////////////////////////////
		// DO NOT MODIFY THIS FUNCTION!!!! //
		/////////////////////////////////////
    // Displays the game by printing it to stdout
		void display() const { board.display(); }
    
    /////////////////////////////////////
		// DO NOT MODIFY THIS FUNCTION!!!! //
		/////////////////////////////////////
    // Checks if the game is valid
		bool is_valid_game() const { return board.has_valid_kings(); }

		// Attempts to make a move. If successful, the move is made and
		// the turn is switched white <-> black. Otherwise, an exception is thrown
		void make_move(const Position& start, const Position& end);

		// Returns true if the designated player is in check
		bool in_check(const bool& white) const;

		// Returns true if the designated player is in mate
		bool in_mate(const bool& white) const;

		// Returns true if the designated player is in mate
		bool in_stalemate(const bool& white) const;

		// Checks if the path between start and end is clear (except for the start and end positions)
		bool check_path(const Position& start, const Position& end) const;

		// Helper function to check if there are any valid moves (essentially a move that will not put you in check)
		bool has_legal_moves(const bool& white) const;


		// Return the total material point value of the designated player
	        int point_value(const bool& white);

		// To allow Game the access to clear_board and remove the dynamiccally allocated member
		void clear_board() {
			board.clear_board();  // Allows access the clear_board function in the Board class
		}

		// Declare the overloaded >> function as a friend so we can access the private board
		friend std::istream& operator>> (std::istream& is, Game& game);

		// Copy Constructor for Game
    	Game(const Game& other);

	private:
		// The board
		Board board;

		// Is it white's turn?
		bool is_white_turn;

        // Writes the board out to a stream
        friend std::ostream& operator<< (std::ostream& os, const Game& game);

        // Reads the board in from a stream
        friend std::istream& operator>> (std::istream& is, Game& game);

		// Helper functions used in clear path to check if the path from start to end is clear to move
		bool verify_horizontal_path(const Position& start, const Position& end) const;
		bool verify_vertical_path(const Position& start, const Position& end) const;
		bool verify_diagonal_path(const Position& start, const Position& end) const;
		bool verify_source_and_destination(const Position& start, const Position& end) const;
	};
}
#endif // GAME_H
