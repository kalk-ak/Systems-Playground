#include "Bishop.h"
#include <iostream>

namespace Chess
{
	bool Bishop::legal_move_shape(const Position& start, const Position& end) const {
		// Check if the move is diagonal
		if (std::abs(start.first - end.first) != std::abs(start.second - end.second)) {
			// std::cerr << "Move specified for bishop is not diagonal" << std::endl;
			return false;
		}
		return true;
	}
}
