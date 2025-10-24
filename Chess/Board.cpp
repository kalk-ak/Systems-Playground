#include <iostream>
#include <utility>
#include <map>
#ifndef _WIN32
#include "Terminal.h"
#endif // !_WIN32
#include "Board.h"
#include "CreatePiece.h"
#include "Exceptions.h"

namespace Chess
{
    /////////////////////////////////////
    // DO NOT MODIFY THIS FUNCTION!!!! //
    /////////////////////////////////////
    Board::Board() {}

    const Piece* Board::operator()(const Position& position) const {
        // Use find to get the iterator to the specified position
        std::map<Position, Piece*>::const_iterator it = occ.find(position);

        // If the iterator points to occ.end(), the position was not found
        if (it == occ.end()) {
            return nullptr; // Position is empty; no piece found here
        }

        // Otherwise, return the piece pointer at the found position
        return it->second; // Position found; return the piece pointer
    }

    // Will add a piece to OCC at the given position and use the create_piece function
    void Board::add_piece(const Position& position, const char& piece_designator) {
        // Check if the position is valid (i.e., on the board)
        if (position.first < 'A' || position.first > 'H' || position.second < '1' || position.second > '8') {
            throw Exception("invalid position");
        }

        // Check if the position is already occupied using operator()
        if (operator()(position) != nullptr) {
            throw Exception("position is occupied");
        }

        // Create the appropriate piece based on the designator using create_piece
        Piece* piece = create_piece(piece_designator);
        if (piece == nullptr) {
            throw Exception("invalid designator");
        }

        // Add the piece to the board
        occ[position] = piece;
    }

    // Removes a piece at the given position from OCC and deletes its dynamically allocated memory
    void Board::remove_piece(const Position& position) {
        // Find the piece at the given position using the map
        std::map<Position, Piece*>::iterator it = occ.find(position);

        // If the position contains a piece, delete it and remove it from the map
        if (it != occ.end()) {
            delete it->second; // Free the memory allocated for the piece
            occ.erase(it);     // Remove the position from the map
        }
    }

    // Clear board helper function that will depopulate occ so we can load a file to it
    void Board::clear_board() {
        // Iterate through where all positions are stored,
        for (std::map<Position, Piece*>::iterator it = occ.begin(); it != occ.end(); it++) {
            delete it->second;  // Free dynamically allocated memory
        }

        occ.clear();  // Clear the map
    }

    // A method that will return all the positions as a vector so we use the overloaded operator to find a specific piece at a given position
    std::vector<Position> Board::get_all_positions() const {
        std::vector<Position> positions;
        for (std::map<Position, Piece*>::const_iterator it = occ.cbegin(); it != occ.cend(); it++) {
            positions.push_back(it->first);
        }
        return positions;
    }

    int Board::point_value(const bool& white) {
        int total_value = 0;
        for (std::map<Position, Piece*>::iterator it = occ.begin(); it != occ.end(); it++) {
            if ((it->second->is_white()) == white) { // Checks for piece of the right type
                total_value += (it->second->point_value());
            }
        }
        return total_value;
    }

  // Copy Constructor
  Board::Board(const Board& other) {
    // Iterator over the map and copy it
    
    for (std::map<Position, Piece*>::const_iterator it = other.occ.cbegin(); it != other.occ.cend(); it++) {
        Position pos = it->first;
        const Piece* original_piece = it->second;

        if (original_piece != nullptr) {
            // Create a new piece based on the type of the original piece.
            occ[pos] = create_piece(original_piece->to_ascii());
        }
    }
}


  // Assignment operator for deep copying
  Board& Board::operator=(const Board& other) {
    if (this == &other) {
        return *this; // Handle self-assignment
    }

    // Clear the current board to avoid memory leaks.
    clear_board();

    // Deep copy each piece using an explicit iterator type.
    std::map<Position, Piece*>::const_iterator it = other.occ.begin();
    for (; it != other.occ.end(); ++it) {
        Position pos = it->first;
        const Piece* original_piece = it->second;

        if (original_piece != nullptr) {
            occ[pos] = create_piece(original_piece->to_ascii());
        }
    }

    return *this;
}


    void Board::display() const {
    // Prints the column names for clarity
    std::cout << "   ";
    for (char c = 'A'; c <= 'H'; c++) {
        std::cout << c << " ";
    }
    std::cout << "\n  ";
    for (int i = 0; i < 8; i++) {
        std::cout << "--";
    }
    std::cout << "-\n";

    // Prints each row with alternating background colors for the board squares
    for (char r = '8'; r >= '1'; r--) {
        std::cout << r << " |";
        for (char c = 'A'; c <= 'H'; c++) {
            const Piece* piece = (*this)(Position(c, r));
    

            if (piece) {
                // Display the piece in a contrasting color
                std::cout << piece->to_unicode();
            } else {
                std::cout << " ";
            }

            Terminal::set_default(); // Reset colors after each square
            std::cout << " ";
        }
        std::cout << "|" << r << "\n";
    }

    // Prints the column names again for clarity
    std::cout << "  ";
    for (int i = 0; i < 8; i++) {
        std::cout << "--";
    }
    std::cout << "-\n   ";
    for (char c = 'A'; c <= 'H'; c++) {
        std::cout << c << " ";
    }
    std::cout << std::endl;
}


    bool Board::has_valid_kings() const {
        int white_king_count = 0;
        int black_king_count = 0;
        for (std::map<std::pair<char, char>, Piece*>::const_iterator it = occ.begin();
             it != occ.end();
             it++) {
            if (it->second) {
                switch (it->second->to_ascii()) {
                    case 'K':
                        white_king_count++;
                        break;
                    case 'k':
                        black_king_count++;
                        break;
                }
            }
        }
        return (white_king_count == 1) && (black_king_count == 1);
    }

    /////////////////////////////////////
    // DO NOT MODIFY THIS FUNCTION!!!! //
    /////////////////////////////////////
    std::ostream& operator<<(std::ostream& os, const Board& board) {
        for (char r = '8'; r >= '1'; r--) {
            for (char c = 'A'; c <= 'H'; c++) {
                const Piece* piece = board(Position(c, r));
                if (piece) {
                    os << piece->to_ascii();
                } else {
                    os << '-';
                }
            }
            os << std::endl;
        }
        return os;
    }
}


