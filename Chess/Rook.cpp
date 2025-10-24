#include "Rook.h"

namespace Chess
{
  bool Rook::legal_move_shape(const Position& start, const Position& end) const {
        // Extract column and row from the start and end positions
        char start_col = start.first;   // Column of the start position (e.g., 'A' to 'H')
        char start_row = start.second;  // Row of the start position (e.g., '1' to '8')
        char end_col = end.first;       // Column of the end position
        char end_row = end.second;      // Row of the end position

        // Check if the rook is moving in a straight line:
        // - Either the column remains the same and the row changes (vertical move)
        // - Or the row remains the same and the column changes (horizontal move)
        return (start_col == end_col && start_row != end_row) ||  // Vertical move
               (start_row == end_row && start_col != end_col);    // Horizontal move
    }
}