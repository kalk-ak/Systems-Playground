#include "Queen.h"
#include <iostream>

namespace Chess
{
  
  bool Queen::legal_move_shape(const Position& start, const Position& end) const {
	// Diagonal move shape for queen
	if (std::abs(start.first - end.first) == std::abs(start.second - end.second)) {
		return true;
	}
	// Veritical and horizontal move for queen
	if ((start.first == end.first && start.second != end.second) || (start.first != end.first && start.second == end.second)) {
		return true;
	}

	// if none of the case shapes pass print error and return false
	return false;
  }
}
