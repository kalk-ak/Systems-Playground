#include "Knight.h"
#include <cmath>

namespace Chess
{
  bool Knight::legal_move_shape(const Position& start, const Position& end) const {

    int xChange = abs(start.first - end.first);
    int yChange = abs(start.second - end.second);
    
    return (((xChange == 1) && (yChange == 2)) || ((xChange == 2) && (yChange == 1)));
  }

  
}
