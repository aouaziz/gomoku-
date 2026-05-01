#ifndef GOMOKU_HPP
#define GOMOKU_HPP

#include <vector>
#include <iostream>
#include <stdexcept> 
#include <SFML/Graphics.hpp>
#include <string>


const int BOARD_SIZE = 19;
const int CELL_SIZE    = 40;
const int MARGIN       = 40;
const int BOARD_PIXELS = (BOARD_SIZE - 1) * CELL_SIZE;
const int WINDOW_WIDTH = BOARD_PIXELS + 2 * MARGIN;
const int WINDOW_HEIGHT= WINDOW_WIDTH + 100;
const int STONE_RADIUS = CELL_SIZE / 2 - 3;

enum Cell { EMPTY, BLACK, WHITE };

struct Point {
    int row , col; // row and column
};

struct MoveResult{
    Point placed;
    Cell color;
    std::vector<Point> captured; // stones captured by this move
};

class Gomoku{
    private:
    Cell board[BOARD_SIZE][BOARD_SIZE];
    Cell turn;
    int capturedBlack;
    int capturedWhite;
    std::vector<MoveResult> history;
    
    public:
        Gomoku();
        ~Gomoku();

        // ── core actions ──
        Cell getCell(int row, int col) const;
        void setCell(int row, int col, Cell cell);
        void setCaptures(Cell cell);
        bool inBounds(int row, int col) const;
        Cell opponent(Cell color) const;
        void printBoard() const ;


        // ── game logic (implement step by step) ──
        MoveResult applyMove(int row, int col, Cell color);
        void undoMove(const MoveResult &move);
        std::vector<Point> checkCaptures(int row, int col, Cell c) const;
        // --- check Five 
        bool    hasFiveAt(int r, int c, Cell color) const;
        bool    hasFive(Cell color) const;  // scans whole board (fallback)
        int     countDirection(int r, int c, int dr, int dc, Cell color) const;

        bool hasTenCaptures(Cell color) const;
        bool isOpenFourAt(int r, int c, int dr, int dc, Cell color);
        bool isFreeThreeAt(int r, int c, int dr, int dc, Cell color) ;
        bool isDoubleThree(int r, int c, Cell color)  ;
        
        bool isFiveBreakable(Cell winnerColor);
        bool checkIfCorrectFive(int r , int c , Cell opponentColor,Cell winnerColor);
};


#endif