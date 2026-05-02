#include "gomoku.hpp"

Gomoku::Gomoku() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    turn = BLACK;
    capturedBlack = 0;
    capturedWhite = 0;
    std::cout << "Gomoku game created" << std::endl;
}

Gomoku::~Gomoku() {
}

int Gomoku::getCaptures(Cell color) const {
    return (color == BLACK) ? capturedBlack : capturedWhite;
}
Cell Gomoku::getCell(int row, int col) const {
        return board[row][col];
}
void Gomoku::setCaptures(Cell cell)
{
    if(cell == BLACK)
        capturedBlack++;
    else if (cell == WHITE)
        capturedWhite++;
    
}
void Gomoku::setCell(int row, int col, Cell cell) {
    if (inBounds(row, col) && getCell(row,col) == EMPTY) {
        board[row][col] = cell;
        setCaptures(cell);
    }
    else
        std::cout << "Wrong Move \n";
}

bool Gomoku::inBounds(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

Cell Gomoku::opponent(Cell color) const {
    return (color == BLACK) ? WHITE : BLACK;
}

MoveResult Gomoku::applyMove(int row, int col, Cell color)
{
    MoveResult mv;

    if (!inBounds(row, col) || board[row][col] != EMPTY)
        throw std::invalid_argument("Invalid move");
    
    else if (isDoubleThree(row, col, color) && checkCaptures(row, col, color).empty()) {
        throw std::invalid_argument("Forbidden Move: Double-Three!");
    }

    mv.placed = {row, col};
    mv.color = color;

    board[row][col] = color;

    // detect captures
    mv.captured = checkCaptures(row, col, color);

    int stonesCaptured = 0;

    for (auto &p : mv.captured)
    {
        board[p.row][p.col] = EMPTY;
        stonesCaptured++;
    }

    if (color == BLACK)
        capturedBlack += stonesCaptured;
    else
        capturedWhite += stonesCaptured;

    turn = opponent(color);

    history.push_back(mv);

    return mv;
}

void Gomoku::undoMove(const MoveResult &mr)
{
    board[mr.placed.row][mr.placed.col] = EMPTY;

    int stonesCaptured = 0;
    Cell oppColor = opponent(mr.color);

    for (const auto &p : mr.captured)
    {
        board[p.row][p.col] = oppColor;
        stonesCaptured++;
    }

    if (mr.color == BLACK)
        capturedBlack -= stonesCaptured;
    else
        capturedWhite -= stonesCaptured;

    turn = mr.color;

    // Remove from history
    if (!history.empty())
        history.pop_back();
}


void Gomoku::printBoard() const {
    std::cout << "   ";
    for (int c = 0; c < BOARD_SIZE; c++)
        std::cout << (c % 10)<< " ";
    std::cout << "\n";

    for (int r = 0; r < BOARD_SIZE; r++) {
        std::cout << (r < 10 ? " " : "") << r << " ";
        for (int c = 0; c < BOARD_SIZE; c++) {
            char ch = '.';
            if (board[r][c] == BLACK) ch = 'X';
            if (board[r][c] == WHITE) ch = 'O';
            std::cout << ch << " ";
        }
        std::cout << "\n";
    }
    std::cout << "Captures  B:" << capturedBlack << "  W:" << capturedWhite << "\n";
}


std::vector<Point> Gomoku::checkCaptures(int r, int c, Cell color) const {
    std::vector<Point> captured;
    Cell opp = opponent(color);

    // 8 directions (but we check each individually)
    const int dr[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
    const int dc[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

    for (int d = 0; d < 8; d++) {
        int r1 = r + dr[d],     c1 = c + dc[d];      // first neighbor
        int r2 = r + 2*dr[d],   c2 = c + 2*dc[d];    // second neighbor
        int r3 = r + 3*dr[d],   c3 = c + 3*dc[d];    // third cell (must be your color)

        if (inBounds(r1,c1) && inBounds(r2,c2) && inBounds(r3,c3)) {
            if (board[r1][c1] == opp &&
                board[r2][c2] == opp &&
                board[r3][c3] == color)
            {
                captured.push_back({r1, c1});
                captured.push_back({r2, c2});
            }
        }
    }
    return captured;
}
int Gomoku::countDirection(int r, int c, int dr, int dc, Cell color ) const{

    int count =0;
    int nr = r +dr ;
    int nc = c + dc;
    while(inBounds(nr,nc) && board[nr][nc] == color)   
    {
        count++;
        nr += dr;
        nc += dc;
    }
    return count ;
}
bool Gomoku::hasFiveAt(int r, int c, Cell color) const{
    int dr[] = {0,1,1,1};
    int dc[] = {1,0,1,-1};
    for(int i =0 ; i< 4 ; i++)
    {
        int total = 1 + countDirection(r,c,dr[i],dc[i],color) + countDirection(r,c,-dr[i],-dc[i],color);
        if(total >= 5)
            return true; 
    }
    return false;
}

bool Gomoku::hasFive(Cell color)const {
   
   
   
    for(int r = 0 ; r < BOARD_SIZE ; r++ )
    {
        for(int c = 0 ; c < BOARD_SIZE ; c++)
        {
            if(board[r][c]==color && hasFiveAt(r,c,color))
                return true;
        }
    }
    return false;
}

bool Gomoku::hasTenCaptures(Cell color) const{
    return (color  == BLACK && capturedBlack >=10) || (color == WHITE && capturedWhite >= 10);
}
