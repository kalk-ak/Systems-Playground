#include "Pawn.h"
#include <cmath>
#include <iostream>
#include <assert.h>

namespace Chess
{
	bool Pawn::legal_move_shape(const Position& start, const Position& end) const {

		// checks if move happens in the same Column
		if (start.first != end.first) {
			// std::cerr << "Invalid move: The move for the pawn doesn't happen in the same column" << std::endl;
			return false;
		}
		// Check if the move is backward based on pawn's color
		int direction = is_white() ? 1 : -1; 
		if ((end.second - start.second) * direction <= 0) {
			// std::cerr << "Invalid move: Pawn can't move backward." << std::endl;
			return false;
		}

	
		// Special handling for the first move
		if (this->first_move) {
			// Allow moving 1 or 2 squares forward, but not diagonally
			if ((end.second - start.second) * direction <= 2) {
				return true;
			}
			// std::cerr << "Invalid move: Pawn can't move more than 2 squares forward on the first move." << std::endl;
			return false;
		}
	
		// For subsequent moves, allow only 1 square forward in the same column
		if ((end.second - start.second) * direction == 1) {
			return true;
		}
	
		// Invalid move for a pawn
		// std::cerr << "Invalid move: Pawn move not allowed." << std::endl;
		return false;
	}

	bool Pawn::legal_capture_shape(const Position& start, const Position& end) const {
		// Calculate the direction the pawn moves based on its color
		int direction = this->is_white() ? 1 : -1;
	
		// Check if the move is diagonal
		if (abs(end.first - start.first) == 1 && (end.second - start.second) * direction == 1) {
			return true; // Valid capture move
		}
	
		// Invalid capture move
		// std::cerr << "Invalid capture: Pawn can only capture diagonally forward by one square." << std::endl;
		return false;
	}
}
	
