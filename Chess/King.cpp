#include "King.h"
#include <cmath>
#include <iostream>

namespace Chess
{
	bool King::legal_move_shape(const Position& start, const Position& end) const {
		// Moves exactly one square in any directions
		if (abs(start.first - end.first) <= 1 && abs(start.second - end.second) <= 1) {
			return true;
		}

		// if false prints error
		// std::cerr << "Invalid move for king. King can only move in adjacent squares" << std::endl;
		return false;

	}
}
