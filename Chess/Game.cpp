#include <cassert>
#include <cmath>
#include "Game.h"
#include <map>


namespace Chess
{
    /////////////////////////////////////
    // DO NOT MODIFY THIS FUNCTION!!!! //
    /////////////////////////////////////
    Game::Game() : is_white_turn(true) {
        // Add the pawns
        for (int i = 0; i < 8; i++) {
            board.add_piece(Position('A' + i, '1' + 1), 'P');
            board.add_piece(Position('A' + i, '1' + 6), 'p');
        }


        // Add the rooks
        board.add_piece(Position( 'A'+0 , '1'+0 ) , 'R' );
        board.add_piece(Position( 'A'+7 , '1'+0 ) , 'R' );
        board.add_piece(Position( 'A'+0 , '1'+7 ) , 'r' );
        board.add_piece(Position( 'A'+7 , '1'+7 ) , 'r' );


        // Add the knights
        board.add_piece(Position( 'A'+1 , '1'+0 ) , 'N' );
        board.add_piece(Position( 'A'+6 , '1'+0 ) , 'N' );
        board.add_piece(Position( 'A'+1 , '1'+7 ) , 'n' );
        board.add_piece(Position( 'A'+6 , '1'+7 ) , 'n' );


        // Add the bishops
        board.add_piece(Position( 'A'+2 , '1'+0 ) , 'B' );
        board.add_piece(Position( 'A'+5 , '1'+0 ) , 'B' );
        board.add_piece(Position( 'A'+2 , '1'+7 ) , 'b' );
        board.add_piece(Position( 'A'+5 , '1'+7 ) , 'b' );


        // Add the kings and queens
        board.add_piece(Position( 'A'+3 , '1'+0 ) , 'Q' );
        board.add_piece(Position( 'A'+4 , '1'+0 ) , 'K' );
        board.add_piece(Position( 'A'+3 , '1'+7 ) , 'q' );
        board.add_piece(Position( 'A'+4 , '1'+7 ) , 'k' );
    }


    void Game::make_move(const Position& start, const Position& end) {


        // Find the piece to move using the overloaded "()" operator
        Piece* piece_to_move = (Piece*) board(start);
        Piece* piece_at_end = (Piece*) board(end);




        // Check if there is a piece at the start position
        if (piece_to_move == nullptr) {
            throw Exception("no piece at start position");
            return;
        }

        // Exception is handeled in the helper method verify_source_and_destination
        if (verify_source_and_destination(start, end)) {
            // handels the case for moving a pawn
            if ((toupper(piece_to_move->to_ascii())) == 'P')  // checks if piece is a pawn
            {
		

                if (piece_at_end != nullptr)    // There is an opponent piece to be catptured
                {  
                    // checks if the move is possible
                    if (!(piece_to_move->legal_capture_shape(start, end)))          // Illegal capture shape
                    {
                        throw Exception("illegal capture shape");
                        return;
                    }
                    // checks if move leads exposes check
                    {  
                        // creates a deep copied game board to check if the move leads to check mate
                        Game temp_game = *this;
                        temp_game.board.remove_piece(end);

						Piece* piece_to_move = (Piece*) temp_game.board(start);
					


                        // handels the case for pawn promotion
                        if ((piece_to_move->to_ascii() == 'P') && (end.second == '8')) { // White pawn promotion
                            temp_game.board.add_piece(end, 'Q');    
                        } else if ((piece_to_move->to_ascii() == 'p') && (end.second == '1')) { // Black pawn promotion
                            temp_game.board.add_piece(end, 'q');
                        } else {
                            temp_game.board.add_piece(end, (piece_to_move->to_ascii()));
                        }
                        temp_game.board.remove_piece(start);

                        if(temp_game.in_check(is_white_turn))       // if true: the proposed move would result in the current player being in check
                        {  
                            temp_game.clear_board();                // Clear dynamically allocated memory in temporary board
                            throw Exception("move exposes check");
                            return;
                        }
                    }


                    board.remove_piece(end);
                    // handels the case for pawn promotion
                    if ((piece_to_move->to_ascii() == 'P') && (end.second == '8'))  // White pawn promotion
                        board.add_piece(end, 'Q');  
                    else if ((piece_to_move->to_ascii() == 'p') && (end.second == '1'))  // Black pawn promotion
                        board.add_piece(end, 'q');
                    else 
                        board.add_piece(end, piece_to_move->to_ascii());
                    
                    board.remove_piece(start);


                    is_white_turn = !(is_white_turn);   // changes the game turn


                    Pawn* moved_pawn = (Pawn*)board(end);
                    moved_pawn->change_first();
                    return;
                   
                }
                else
                {  
                    // checks if the move specified is a legal move for a pawn
                    if (!(piece_to_move->legal_move_shape(start, end))) { // Illegal move shape
                        throw Exception("illegal move shape");
                        return;
                    }
                    // checks if there is a path blocking the pawn movement
                    if (!(check_path(start, end)))
                    {
                        throw Exception("path is not clear");
                        return;
                    }
                    // checks if move leads to check mate
                    {  
                        // creates a deep copied game board to check if the move leads to check mate
                        Game temp_game = *this;
						Piece* piece_to_move = (Piece*) temp_game.board(start);
                       
                        // handels the case for pawn promotion
                        if ((piece_to_move->to_ascii() == 'P') && (end.second == '8')) { // White pawn promotion
                            temp_game.board.add_piece(end, 'Q');    
                        } else if ((piece_to_move->to_ascii() == 'p') && (end.second == '1')) { // Black pawn promotion
                            temp_game.board.add_piece(end, 'q');
                        } else {
                            temp_game.board.add_piece(end, piece_to_move->to_ascii());
                        }


                        temp_game.board.remove_piece(start);


                        if(temp_game.in_check(is_white_turn))       // if true: the proposed move would result in the current player being in check
                        {  
                            temp_game.clear_board();                // Clear dynamically allocated memory in temporary board
                            throw Exception("move exposes check");
                            return;
                        }


                        temp_game.clear_board();  // Even if the if statement does not pass, have to clear the dynamic memory
                    }
				
                    // Moves the actual pieces in this board
                    // handels the case for pawn promotion
                    if ((piece_to_move->to_ascii() == 'P') && (end.second == '8')) { // White pawn promotion
                        board.add_piece(end, 'Q');  
                    } else if ((piece_to_move->to_ascii() == 'p') && (end.second == '1')) { // Black pawn promotion
                        board.add_piece(end, 'q');
                    } else {
                        board.add_piece(end, (piece_to_move->to_ascii()));
                    }


                    board.remove_piece(start);


                    is_white_turn = !(is_white_turn);   // changes the game turn`


                    Pawn* moved_pawn = (Pawn*)board(end);
                    moved_pawn->change_first();
                    return;
                }
            }
            else    // The piece to be moved is not a pawn
            {
                if(! piece_to_move->legal_move_shape(start, end)){
                    throw Exception("illegal move shape");
                    return;
                }


                if (! check_path(start, end)) {         // path is blocked
                    throw Exception("path is not clear");
                    return;
                }
                // checks if move to check mate
                {
                    Game temp_game = *this;
                   
                   
                    if (piece_at_end != nullptr) {
                        temp_game.board.remove_piece(end);
                    }
                   
                    temp_game.board.add_piece(end, piece_to_move->to_ascii());
                    temp_game.board.remove_piece(start);
                       
                    if(temp_game.in_check(is_white_turn)){  //the proposed move would result in the current player being in check
                        temp_game.clear_board();  // Clear dynamically allocated memory in temporary board
                        throw Exception("move exposes check");
                    }
                }


                if (piece_at_end != nullptr)    // End square is occupied. Capturing opponent piece
                    board.remove_piece(end);
               
                else    // End square is empty
                {
                    board.add_piece(end, piece_to_move->to_ascii());
                    board.remove_piece(start);
                   
                }
                is_white_turn = !(is_white_turn);
            }


        }
    }



    bool Game::in_check(const bool& white) const {
        Position king_position;


        // Find the position of the king
        std::vector<Position> all_positions = board.get_all_positions();
        for (std::vector<Position>::const_iterator it = all_positions.begin(); it != all_positions.end(); it++) {
            Position pos = *it;
            Piece* piece = (Piece*)board(pos);  // Use overloaded opperator to find the piece at the given position


            if (piece != nullptr) {
                char piece_symbol = piece->to_ascii();


                // Find the king of the specified color
                if ((white && piece_symbol == 'K') || (!white && piece_symbol == 'k')) {
                    king_position = pos;
                    break;
                }
            }
        }


        // Iterate over all pieces to check if any opponent piece can legally move to the king's position
        for (std::vector<Position>::const_iterator it = all_positions.cbegin(); it != all_positions.cend(); ++it) {
            Position pos = *it;
            Piece* piece = (Piece*)board(pos);


            if (piece != nullptr && piece->is_white() != white) {
                // used to check for pawn check
                char symbol = tolower(piece->to_ascii());


                if (symbol == 'p') {
                    // For pawns, check using legal_capture_shape()
                    if (piece->legal_capture_shape(pos, king_position) && check_path(pos, king_position)) {
                        return true;
                    }
                } else {
                    if (piece->legal_move_shape(pos, king_position) && check_path(pos, king_position)) {
                        return true;  // The king is in check
                    }
                }
            }
        }
        return false;  // No opponent piece can move to the king's position
    }


    // Copy Constructor for Game
    Game::Game(const Game& other) : board(other.board), is_white_turn(other.is_white_turn) {
        // The board copy constructor takes care of deep copying all pieces
    }




    // Helper function to check if there are any valid moves (essentially a move that will not put you in check)
    bool Game::has_legal_moves(const bool& white) const {
        // First, get all positions
        std::vector<Position> all_positions = board.get_all_positions();


        // Loop through all positions and ensure we are only checking the pieces that belong to the specific player (the bool variable)
        for (std::vector<Position>::const_iterator it = all_positions.cbegin(); it != all_positions.cend(); ++it) {
            Position start = *it;  // Get the initial starting position
            const Piece* piece = board(start);


            if (piece != nullptr && piece->is_white() == white) {
                // Iterate over every square on the board to check for potential end positions
                for (char col = 'A'; col <= 'H'; ++col) {
                    for (char row = '1'; row <= '8'; ++row) {
                        Position end = std::make_pair(col, row);  // Make end position


                        if (start == end) {
                            continue; // Skip if start and end positions are the same
                        }


                        const Piece* piece_at_end = board(end);


                        // If there's a piece at the destination, ensure it is of the opposite color
                        if (piece_at_end != nullptr && piece_at_end->is_white() == white) {
                            continue;  // Cannot capture your own piece, so skip
                        }


                        bool can_move = false;
                        char symbol = tolower(piece->to_ascii());


                        if (piece_at_end != nullptr) {
                            // If it's a pawn and we are capturing, use legal_capture_shape()
                            if (symbol == 'p') {
                                can_move = piece->legal_capture_shape(start, end) && check_path(start, end);
                            } else {
                                // Non-pawn pieces can use legal_move_shape() for capturing as their capture move is the same as normal move
                                can_move = piece->legal_move_shape(start, end) && check_path(start, end);
                            }
                        } else {
                            // Normal non-capturing move
                            can_move = piece->legal_move_shape(start, end) && check_path(start, end);
                        }


                        if (can_move) {
                            // Simulate the move
                            Game temp_game = *this;
                            Piece* piece_to_move = (Piece*)temp_game.board(start);
                           
                            // If capturing an opponent's piece, remove it
                            if (piece_at_end != nullptr) {
                                temp_game.board.remove_piece(end);
                            }
                           
                            // Perform the move
                            temp_game.board.add_piece(end, piece_to_move->to_ascii());
                            temp_game.board.remove_piece(start);


                            // Check if you're still in check after making this move
                            if (!temp_game.in_check(white)) {
                                temp_game.clear_board();  // Clear the dynamically allocated memory from the board
                                return true; // Found a move that gets you out of check or is a legal move
                            }
                            // If we don't have a legal move, still ensure we clear the current temp_board from memory before continuing
                            temp_game.clear_board();
                        }
                    }
                }
            }
        }


        // If no legal moves found
        return false;
    }

    // Function to check if the player is in checkmate
    bool Game::in_mate(const bool& white) const {
        return (in_check(white) && !has_legal_moves(white));
    }

    // Function to check if the player is in stalemate
    bool Game::in_stalemate(const bool& white) const {
        return (!in_check(white) && !has_legal_moves(white));
    }

    // Return the total material point value of the designated player
    int Game::point_value(const bool& white) {
        return board.Board::point_value(white);
    }

    std::istream& operator>> (std::istream& is, Game& game) {
        // First, will need to clear the current board to allow us to populate occ
        game.clear_board();

        // Loop through the file based on its saving characterisitics
        for (char row = '8'; row >= '1'; row--) {  // The top row of characters will be 8 and then deincrement
            for (char col = 'A'; col <= 'H'; col++){  // The cols will go from A to H and incremement
                char piece_designator;
                is >> piece_designator;  // Read character
               
                if (piece_designator != '-') {  // If the char read is not the designated blank space character, we will add the piece
                    Position position = std::make_pair(col, row);  // Position is defined as the col first, and then row


                    game.board.add_piece(position, (const char&)piece_designator);  // Have to cast the piece_designator as a constant char or else it will be read as invalid
                }
            }
        }

        // After the main board is read, check whose turn it is from the final character
        char turn;
        is >> turn;

        if (turn == 'w'){
            game.is_white_turn = true;
        } else if (turn == 'b'){
            game.is_white_turn = false;
        } else{
            throw Exception("Invalid turn character in input file. Expected 'w' or 'b'.");
        }

        if (! game.is_valid_game()) {
            throw Exception("Invalid game. Number of kings required must be 2 in a chess board");
        }
        return is;
    }

    /////////////////////////////////////
    // DO NOT MODIFY THIS FUNCTION!!!! //
    /////////////////////////////////////
    std::ostream& operator<< (std::ostream& os, const Game& game) {
        // Write the board out and then either the character 'w' or the character 'b',
        // depending on whose turn it is
        return os << game.board << (game.turn_white() ? 'w' : 'b');
    }



    // checks if the source and destination are similar colored piece.
    // checks if the move specified lies in the board. Is a valid possible move
    bool Game::verify_source_and_destination(const Position& start, const Position& end) const  {
        // The next two if statements check if a move happens on a board
        {
            if ((start.first < 'A') || (start.first > 'H') || (start.second < '1') || (start.second > '8')) {
                throw Exception("start position is not on board");
                return false;
            }
            if ((end.first < 'A') || (end.first > 'H') || (end.second < '1') || (end.second > '8')) {
                throw Exception("end position is not on board");
                return false;
            }
        }
       
        const Piece* current = board(start);
        const Piece* destination = board(end);


        // checks if there is a piece to move at start
        if (current == nullptr) {
            throw Exception("no piece at start position");
            return false;
        }
       
        // If the destination is null pointer then it is a legal destination
        if (destination == nullptr) {
            return true;


        } else if (current->is_white() == destination->is_white()) {
            throw Exception("cannot capture own piece");
            return false;
        }
       
        // checks if the start and the end are not equal. If (start == end) then no move is happening
        if (start == end) {
            throw Exception("illegal move shape");
            return false;
        }

        // if the code reaches here it passes all the if statements
        else return true;
    }

      // Checks whether a given 'start' and 'end' position are a valid horizontal move. Used for Rook and Queen
      // Returns a boolean to signify whether:
      // - The entire move takes place on one row.
      // - All spaces directly between 'start' and 'end' are empty
      bool Game::verify_horizontal_path(const Position& start, const Position& end) const {
        if (start.second != end.second) {
            return false;
        }


        // this loop acertains that we always check from the smallest to largest. Making the loop always traverse in a known direction
        char loop_start = (start.first < end.first) ? start.first : end.first;  // same as math.min(startcol, endcol)
        char loop_end = (end.first > start.first) ? end.first : start.first;    // same as math.max(startcol, endcol)
   
        for (char c = loop_start + 1; c < loop_end; c++) {  // :) using c++ in c++ real funny
            Position current(c, start.second);
            // makes sure that all the positions from start to end are nullptr    
            if (board(current) != nullptr)
                return false;
        }
        return true;
      }
   
    // Checks whether a given 'start' and 'end' position are a valid vertical move. Used for Rook and Queen
    // Returns a boolean to signify whether:
    // - The entire move takes place on one column.
    // - All spaces directly between 'start' and 'end' are empty
    bool Game::verify_vertical_path(const Position& start, const Position& end) const {
        if (start.first != end.first) {
            return false;
        }


        // This loop makes sure that we always check from the smallest position to the largest
        char loop_start = (start.second < end.second) ? start.second : end.second;
        char loop_end = (start.second < end.second) ? end.second : start.second;
   
        for (char c = loop_start + 1; c < loop_end; c++) {
            Position current(start.first, c);
            // makes sure that all the positions from start to end are nullptr  
            if (board(current) != nullptr)
                return false;
        }
        return true;
    }


    // Checks whether a given 'start' and 'end' position are a valid diagonal move. Used for Bishop and Queen
    // Returns a boolean to signify whether:
    // - The entire move takes place on one diagonal.
    // - All spaces in the diagonal between 'start' and 'end' are empty
    bool Game::verify_diagonal_path(const Position& start, const Position& end) const {
        if (std::abs(start.first - end.first) != std::abs(start.second - end.second)) {
            return false;
        }


        // Determine the direction of traversal
        int col_step = (end.first > start.first) ? 1 : -1; // Step for columns (right or left)
        int row_step = (end.second > start.second) ? 1 : -1;    // Step for rows (up or down)
   
        // Start traversing from the next position after `start`
        for (int r = start.second + row_step, c = start.first + col_step;
            r != end.second && c != end.first;
            r += row_step, c += col_step) {
            Position current(c, r);
   
            // If any position along the path is not empty, the path is invalid
            if (board(current) != nullptr) {


                return false;
            }
        }
   
        // If we reach here, the diagonal path is valid`
        return true;
    }
    // What this method basically does is it checks if there is a piece in the path that would inhibit the move of a piece
    bool Game::check_path(const Position& start, const Position& end) const {
        const Piece* current = board(start);    // The piece we are going to move
       
       
        assert(current != nullptr);   // Must be checked in the move method
        char designator = current->to_ascii();    // the piece type we are moving and their white flag. Upper case means white and lower case means black
   
        // Here we would be checking the path depending on the piece.
        switch (tolower(designator)) {
       
            case 'n': { // knight
                // All the cases for knight are handled in verify_source_and_destination
                return true;
                break;
            }
       
            case 'r':  { // ROOK
                return verify_horizontal_path(start, end) || verify_vertical_path(start, end);
                break;
            }
       
            case 'b':  {    // BISHOP
                return verify_diagonal_path(start, end);
                break;
            }
       
            case 'p':   { // PAWN
                Pawn* current_pawn = (Pawn*) current;
                if (current_pawn->get_first_move()) {
                    return verify_vertical_path(start, end);
                }
                return true;
                break;
            }
           
            case 'q':   { // QUEEN
                return verify_diagonal_path(start, end) || verify_horizontal_path(start, end) || verify_vertical_path(start, end);
                break;
            }


            case 'k':   { // KING
                return true;
                break;
            }

			case 'm': {	// Mystery
				
				if ((std::abs(start.first - end.first) != std::abs(start.second - end.second)) && (start.second != end.second) && (start.first != end.first)) {
					return true;
				}
				
				else return verify_diagonal_path(start, end) || verify_vertical_path(start, end) || verify_horizontal_path(start, end);
			
				break;
			}
       
            default:
                std::cerr << "Error: Invalid piece located. Shouldn't happen" << std::endl;
                return false;
                break;
           
        }
    }
}
   



