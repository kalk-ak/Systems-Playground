# ♟️ C++ Polymorphic Chess Engine ♟️

![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![Build](https://img.shields.io/badge/build-Make-brightgreen.svg)
![Concept](https://img.shields.io/badge/concept-OOP%20&%20Polymorphism-purple.svg)

This project is a complete, command-line chess game written in modern C++. It is built from the ground up with a clean, object oriented design, focusing on inheritance and dynamic dispatch to handle complex game logic in a scalable and elegant way.

---

## Core Features

- **Full Chess Logic**: Accurately implements all piece movements (King, Queen, Rook, Bishop, Knight, Pawn).
- **Game State Detection**: Automatically detects **Check**, **Checkmate**, and **Stalemate** conditions.
- **Pawn Promotion**: Pawns that reach the opposite end of the board are automatically promoted to a Queen.
- **Interactive CLI**: A simple and robust command-line interface for playing the game.
- **Save & Load**: Games can be saved to a text file at any point and loaded later to resume play.
- **Polymorphic Design**: Built to handle _any_ piece that inherits from the abstract `Piece` class, including the provided "Mystery Piece."

---

## 🏛️ Object-Oriented Design

The core of this project is its objec oriented architecture. All logic is built around an abstract base class, `Piece`.

1.  **Abstract `Piece` Class**: This class defines the "contract" for all pieces. It declares pure virtual functions like `legal_move_shape()` and `legal_capture_shape()`, forcing each specific piece to implement its own unique logic.

2.  **Concrete Piece Classes**: `King`, `Queen`, `Rook`, `Bishop`, `Knight`, and `Pawn` all inherit from `Piece`. Each one provides its own unique implementation of the virtual functions.

3.  **Dynamic Dispatch**: The `Board` class doesn't know (or care) what _kind_ of piece is at a given square. It simply holds a grid of `Piece*` (piece pointers). When checking a move, it just calls `the_piece->legal_move_shape(...)`. The C++ runtime automatically figures out which version to run (e.g., the `King`'s version or the `Rook`'s version).

### Example Game Play
![Example Game Play](example_game/ex.png)


---

## Technologies Used

- **Language:** C++
- **Build System:** `make`
- **Debugging:** `gdb` and `valgrind` (for memory-safe, leak-free code)

---

## How to Play

### 1. Compile

Navigate to the project directory and use the provided `Makefile`:

```bash
make
```

### 2. Run

Launch the game from your terminal:

```bash
./chess
```

### 3. Game Commands

Once the game starts, the board will be displayed. You can enter the following commands:

- `?` - Display the list of actions.
- `Q` - Quit the game.
- `L <filename>` - Load a game from the specified file (e.g., `L checkmate.txt`).
- `S <filename>` - Save the current game to the specified file.
- `M <move>` - Make a move.

Moves are specified in a four-character format: `<start-col><start-row><end-col><end-row>`. For example:

- `M E2E4` - Moves the piece at E2 to E4.
- `M G1F3` - Moves the piece at G1 to F3.
