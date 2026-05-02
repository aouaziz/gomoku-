#include "gomoku.hpp"
#include <iostream>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// Mini test framework
// ─────────────────────────────────────────────────────────────────────────────
static int passed = 0;
static int failed = 0;

#define TEST(name, expr) do { \
    if (expr) { \
        std::cout << "[PASS] " << name << "\n"; \
        ++passed; \
    } else { \
        std::cout << "  [FAIL] " << name << "  (line " << __LINE__ << ")\n"; \
        ++failed; \
    } \
} while(0)

#define TEST_THROWS(name, game, r, c, color) do { \
    bool threw = false; \
    try { (game).applyMove(r, c, color); } \
    catch (const std::invalid_argument&) { threw = true; } \
    TEST(name, threw); \
} while(0)

#define TEST_NO_THROW(name, game, r, c, color) do { \
    bool threw = false; \
    try { MoveResult _mv = (game).applyMove(r, c, color); (void)_mv; } \
    catch (const std::invalid_argument& e) { \
        threw = true; \
        std::cout << "     -> exception: " << e.what() << "\n"; } \
    TEST(name, !threw); \
} while(0)

static void section(const std::string& title) {
    std::cout << "\n══════════════════════════════════════════\n";
    std::cout << "  " << title << "\n";
    std::cout << "══════════════════════════════════════════\n";
}

// ─────────────────────────────────────────────────────────────────────────────
void test_basic_placement() {
    section("1. Basic Placement");
    Gomoku g;
    TEST_NO_THROW("Place on empty cell (9,9)",          g, 9,  9, BLACK);
    TEST_THROWS  ("Place on occupied cell same color",  g, 9,  9, BLACK);
    TEST_THROWS  ("Place on occupied cell other color", g, 9,  9, WHITE);
    TEST_THROWS  ("Row -1 out of bounds",               g, -1,  0, BLACK);
    TEST_THROWS  ("Col -1 out of bounds",               g,  0, -1, BLACK);
    TEST_THROWS  ("Row 19 out of bounds",               g, 19,  0, BLACK);
    TEST_THROWS  ("Col 19 out of bounds",               g,  0, 19, BLACK);
}

// ─────────────────────────────────────────────────────────────────────────────
void test_captures() {
    section("2. Captures");
    {
        Gomoku g;
        g.applyMove(5, 5, BLACK);
        g.applyMove(5, 6, WHITE);
        g.applyMove(5, 7, WHITE);
        MoveResult mv = g.applyMove(5, 8, BLACK);
        TEST("Horizontal: 2 stones in MoveResult",   mv.captured.size() == 2);
        TEST("Horizontal: getCaptures(BLACK) == 2",  g.getCaptures(BLACK) == 2);
        TEST("Captured cell (5,6) is EMPTY",         g.getCell(5,6) == EMPTY);
        TEST("Captured cell (5,7) is EMPTY",         g.getCell(5,7) == EMPTY);
    }
    {
        Gomoku g;
        g.applyMove(2, 2, BLACK);
        g.applyMove(3, 2, WHITE);
        g.applyMove(4, 2, WHITE);
        MoveResult mv = g.applyMove(5, 2, BLACK);
        TEST("Vertical capture: 2 captured", mv.captured.size() == 2);
    }
    {
        Gomoku g;
        g.applyMove(1, 1, BLACK);
        g.applyMove(2, 2, WHITE);
        g.applyMove(3, 3, WHITE);
        MoveResult mv = g.applyMove(4, 4, BLACK);
        TEST("Diagonal (down-right) capture: 2 captured", mv.captured.size() == 2);
    }
    {
        Gomoku g;
        g.applyMove(1, 5, BLACK);
        g.applyMove(2, 4, WHITE);
        g.applyMove(3, 3, WHITE);
        MoveResult mv = g.applyMove(4, 2, BLACK);
        TEST("Anti-diagonal (down-left) capture: 2 captured", mv.captured.size() == 2);
    }
    {
        Gomoku g;
        g.applyMove(5, 5, BLACK);
        g.applyMove(5, 6, WHITE);
        MoveResult mv = g.applyMove(5, 7, BLACK);
        TEST("Single stone not captured",  mv.captured.size() == 0);
        TEST("getCaptures still 0",        g.getCaptures(BLACK) == 0);
    }
    {
        Gomoku g;
        g.applyMove(5, 5, BLACK);
        g.applyMove(5, 6, WHITE);
        g.applyMove(5, 7, WHITE);
        g.applyMove(5, 8, WHITE);
        MoveResult mv = g.applyMove(5, 9, BLACK);
        TEST("Triple not captured", mv.captured.size() == 0);
    }
    {
        Gomoku g;
        g.applyMove(5, 5, WHITE);
        g.applyMove(5, 7, WHITE);
        // FIX: Moving into a flanked position is a legal move.
        TEST_NO_THROW("Moving into flanked position is LEGAL", g, 5, 6, BLACK);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_ten_captures_win() {
    section("3. Win by 10 Captures");
    Gomoku g;
    for (int row = 1; row <= 4; ++row) {
        g.applyMove(row, 1, BLACK);
        g.applyMove(row, 2, WHITE);
        g.applyMove(row, 3, WHITE);
        g.applyMove(row, 4, BLACK);
    }
    TEST("8 captures: hasTenCaptures == false", !g.hasTenCaptures(BLACK));
    g.applyMove(5, 1, BLACK);
    g.applyMove(5, 2, WHITE);
    g.applyMove(5, 3, WHITE);
    g.applyMove(5, 4, BLACK);
    TEST("10 captures: hasTenCaptures == true",  g.hasTenCaptures(BLACK));
    TEST("White never reached 10",              !g.hasTenCaptures(WHITE));
}

// ─────────────────────────────────────────────────────────────────────────────
void test_five_in_a_row() {
    section("4. Five-in-a-Row");
    {
        Gomoku g;
        for (int c = 0; c < 4; ++c) g.applyMove(9, c, BLACK);
        g.applyMove(9, 4, BLACK);
        TEST("Horizontal 5-in-a-row", g.hasFiveAt(9, 4, BLACK));
    }
    {
        Gomoku g;
        for (int r = 0; r < 4; ++r) g.applyMove(r, 9, BLACK);
        g.applyMove(4, 9, BLACK);
        TEST("Vertical 5-in-a-row", g.hasFiveAt(4, 9, BLACK));
    }
    {
        Gomoku g;
        for (int i = 0; i < 4; ++i) g.applyMove(i, i, BLACK);
        g.applyMove(4, 4, BLACK);
        TEST("Diagonal (down-right) 5-in-a-row", g.hasFiveAt(4, 4, BLACK));
    }
    {
        Gomoku g;
        for (int i = 0; i < 4; ++i) g.applyMove(i, 8 - i, BLACK);
        g.applyMove(4, 4, BLACK);
        TEST("Diagonal (down-left) 5-in-a-row", g.hasFiveAt(4, 4, BLACK));
    }
    {
        Gomoku g;
        for (int c = 0; c < 5; ++c) g.applyMove(9, c, BLACK);
        g.applyMove(9, 5, BLACK);
        TEST("Six-in-a-row also wins (5 or more)", g.hasFiveAt(9, 5, BLACK));
    }
    {
        Gomoku g;
        for (int c = 0; c < 3; ++c) g.applyMove(9, c, BLACK);
        g.applyMove(9, 3, BLACK);
        TEST("Four-in-a-row is NOT a win", !g.hasFiveAt(9, 3, BLACK));
    }
    {
        Gomoku g;
        for (int c = 0; c < 5; ++c) g.applyMove(9, c, BLACK);
        TEST("hasFive (board scan) agrees", g.hasFive(BLACK));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_count_direction() {
    section("5. countDirection Helper");
    Gomoku g;
    g.applyMove(9, 9,  BLACK);
    g.applyMove(9, 10, BLACK);
    g.applyMove(9, 11, BLACK);
    // FIX: countDirection measures stones AFTER the origin point
    TEST("Right from (9,9)  == 2", g.countDirection(9,  9, 0,  1, BLACK) == 2);
    TEST("Left  from (9,11) == 2", g.countDirection(9, 11, 0, -1, BLACK) == 2);
    TEST("Down  from (9,9)  == 0", g.countDirection(9,  9, 1,  0, BLACK) == 0);
}

// ─────────────────────────────────────────────────────────────────────────────
void test_breakable_five() {
    section("6. Breakable Five");
    {
        Gomoku g;
        for (int c = 3; c <= 7; ++c) g.applyMove(9, c, BLACK);
        
        // FIX: Setup a scenario where White can ACTUALLY capture a stone
        g.applyMove(10, 4, BLACK);
        g.applyMove(8,  4, WHITE);
        
        TEST("Five is detected",  g.hasFiveAt(9, 7, BLACK));
        // White can play at (11,4) to capture B(9,4) and B(10,4), breaking the 5-in-a-row
        TEST("Five is breakable", g.isFiveBreakable(BLACK));
    }
    {
        Gomoku g;
        for (int c = 0; c <= 4; ++c) g.applyMove(9, c, BLACK);
        TEST("Isolated five detected",       g.hasFiveAt(9, 4, BLACK));
        TEST("Isolated five not breakable", !g.isFiveBreakable(BLACK));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_double_three() {
    section("7. Double-Three Rule");
    {
        Gomoku g;
        g.applyMove(9, 7, BLACK);
        g.applyMove(9, 8, BLACK);
        g.applyMove(7,  9, BLACK);
        g.applyMove(8,  9, BLACK);
        // Playing at 9,9 now correctly creates exactly two Open-Threes
        TEST_THROWS("Double-three at centre is forbidden", g, 9, 9, BLACK);
    }
    {
        Gomoku g;
        g.applyMove(9, 7, BLACK);
        g.applyMove(9, 8, BLACK);
        g.applyMove(7,  9, BLACK);
        g.applyMove(8,  9, BLACK);
        TEST("isDoubleThree(9,9,BLACK) == true", g.isDoubleThree(9, 9, BLACK));
    }
    {
        Gomoku g;
        g.applyMove(9, 7, BLACK);
        g.applyMove(9, 8, BLACK);
        TEST_NO_THROW("Single free-three is allowed", g, 9, 9, BLACK);
    }
    {
        // Double-three exception: move also captures a pair → legal
        Gomoku g;
        g.applyMove(9, 7, BLACK);
        g.applyMove(9, 8, BLACK);
        g.applyMove(7,  9, BLACK);
        g.applyMove(8,  9, BLACK);
        
        // FIX: Put capture setup on the diagonal so it doesn't block the vertical Free-Three!
        g.applyMove(12, 12, BLACK);
        g.applyMove(11, 11, WHITE);
        g.applyMove(10, 10, WHITE);
        
        TEST_NO_THROW("Double-three + capture is LEGAL (exception)", g, 9, 9, BLACK);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_undo() {
    section("8. Undo Move[undoMove(const MoveResult&)]");
    {
        Gomoku g;
        MoveResult mv = g.applyMove(9, 9, BLACK);
        TEST("Cell occupied after move", g.getCell(9,9) == BLACK);
        g.undoMove(mv);
        TEST("Cell empty after undo",    g.getCell(9,9) == EMPTY);
    }
    {
        Gomoku g;
        g.applyMove(5, 5, BLACK);
        g.applyMove(5, 6, WHITE);
        g.applyMove(5, 7, WHITE);
        MoveResult mv = g.applyMove(5, 8, BLACK);
        TEST("Captures before undo: 2",          g.getCaptures(BLACK) == 2);
        g.undoMove(mv);
        TEST("Captures after undo: 0",           g.getCaptures(BLACK) == 0);
        TEST("W(5,6) restored",                  g.getCell(5,6) == WHITE);
        TEST("W(5,7) restored",                  g.getCell(5,7) == WHITE);
        TEST("B(5,8) anchor removed",            g.getCell(5,8) == EMPTY);
    }
    {
        Gomoku g;
        MoveResult mv = g.applyMove(3, 3, WHITE);
        g.undoMove(mv);
        TEST("Non-capture undo: cell empty",        g.getCell(3,3) == EMPTY);
        TEST("Non-capture undo: captures still 0",  g.getCaptures(WHITE) == 0);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_white_wins() {
    section("9. White Can Also Win");
    {
        Gomoku g;
        for (int c = 0; c < 5; ++c) g.applyMove(5, c, WHITE);
        TEST("White five-in-a-row detected", g.hasFiveAt(5, 4, WHITE));
    }
    {
        Gomoku g;
        for (int row = 1; row <= 5; ++row) {
            g.applyMove(row, 1, WHITE);
            g.applyMove(row, 2, BLACK);
            g.applyMove(row, 3, BLACK);
            g.applyMove(row, 4, WHITE);
        }
        TEST("White wins by 10 captures", g.hasTenCaptures(WHITE));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void test_helpers() {
    section("10. Helper Methods");
    Gomoku g;
    TEST("inBounds(0,0)   == true",  g.inBounds(0,  0));
    TEST("inBounds(18,18) == true",  g.inBounds(18, 18));
    TEST("inBounds(-1,0)  == false", !g.inBounds(-1, 0));
    TEST("inBounds(19,0)  == false", !g.inBounds(19, 0));
    TEST("inBounds(0,19)  == false", !g.inBounds(0, 19));
    TEST("opponent(BLACK) == WHITE", g.opponent(BLACK) == WHITE);
    TEST("opponent(WHITE) == BLACK", g.opponent(WHITE) == BLACK);
    g.applyMove(0, 0, BLACK);
    TEST("getCell after applyMove == BLACK", g.getCell(0,0) == BLACK);
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║         GOMOKU RULE TEST SUITE           ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n";

    test_basic_placement();
    test_captures();
    test_ten_captures_win();
    test_five_in_a_row();
    test_count_direction();
    test_breakable_five();
    test_double_three();
    test_undo();
    test_white_wins();
    test_helpers();

    std::cout << "\n══════════════════════════════════════════\n";
    std::cout << "  RESULTS: " << passed << " passed, "
              << failed << " failed\n";
    std::cout << "══════════════════════════════════════════\n\n";
    return (failed == 0) ? 0 : 1;
}