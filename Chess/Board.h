#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <map>
#include <vector>
#include "Piece.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "Mystery.h"


namespace Chess
{
  class Board {

		// Throughout, we will be accessing board positions using Position defined in Piece.h.
		// The assumption is that the first value is the column with values in
		// {'A','B','C','D','E','F','G','H'} (all caps)
		// and the second is the row, with values in {'1','2','3','4','5','6','7','8'}

	public:
		// Default constructor
		Board();

		// Returns a const pointer to the piece at a prescribed location if it exists,
		// or nullptr if there is nothing there.
		const Piece* operator() (const Position& position) const;

		// Attempts to add a new piece with the specified designator, at the given position.
		// Throw exception for the following cases:
		// -- the designator is invalid, throw exception with error message "invalid designator"
		// -- the specified position is not on the board, throw exception with error message "invalid position"
		// -- if the specified position is occupied, throw exception with error message "position is occupied"
		void add_piece(const Position& position, const char& piece_designator);

		// Removes the piece from the specified position
    	void remove_piece(const Position& position);

		// Clear board helper function that will depopulate occ so we can load a file to it
		void clear_board();

		// Checks if the path between start and end is clear (except for the start and end positions)
   	 	bool check_path(const Position& start, const Position& end) const;

		// Displays the board by printing it to stdout
		void display() const;

		// Returns true if the board has the right number of kings on it
		bool has_valid_kings() const;

		// Method for the point display 
        int point_value(const bool& white); 

		// A method that will return all the positions as a vector so we use the overloaded operator to find a specific piece at a given position
		std::vector<Position> get_all_positions() const;

		// Copy Constructor for deep copy
    	Board(const Board& other);

		// Assignment Operator for deep copy
		Board& operator=(const Board& other);

	private:
		// The sparse map storing the pieces, keyed off locations
		std::map<Position, Piece*> occ;

        // Write the board state to an output stream
        friend std::ostream& operator<< (std::ostream& os, const Board& board);

		
	};
}
#endif // BOARD_H
