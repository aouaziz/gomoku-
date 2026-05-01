#include "gomoku.hpp"


int main() {
    Gomoku game;
    Cell currentTurn = BLACK;
    bool gameOver    = false;
    std::string statusMsg = "Black's turn";

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Gomoku");
    window.setFramerateLimit(60);

    sf::Font font;
    // Load any TTF you have, or grab one from /usr/share/fonts on Linux
    bool fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    sf::Color boardColor(220, 179, 92);

    // ── Star-point dots (hoshi) positions for a 19x19 board ─────────────────
    const int hoshi[] = {3, 9, 15};

    while (window.isOpen()) {

        // ── Event Loop ───────────────────────────────────────────────────────
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && !gameOver) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    int col = (event.mouseButton.x - MARGIN + CELL_SIZE / 2) / CELL_SIZE;
                    int row = (event.mouseButton.y - MARGIN + CELL_SIZE / 2) / CELL_SIZE;

                    try {
                        game.applyMove(row, col, currentTurn);

                        if (game.hasTenCaptures(currentTurn)) {
                            statusMsg = (currentTurn == BLACK ? "Black" : "White");
                            statusMsg += " wins by 10 captures!";
                            gameOver = true;
                        } else if (game.hasFiveAt(row, col, currentTurn)) {
                            if (!game.isFiveBreakable(currentTurn)) {
                                statusMsg = (currentTurn == BLACK ? "Black" : "White");
                                statusMsg += " wins with 5-in-a-row!";
                                gameOver = true;
                            } else {
                                statusMsg = "5-in-a-row formed but breakable – game continues.";
                            }
                        }

                        if (!gameOver) {
                            currentTurn = game.opponent(currentTurn);
                            statusMsg   = (currentTurn == BLACK ? "Black" : "White");
                            statusMsg  += "'s turn";
                        }

                    } catch (const std::invalid_argument& e) {
                        statusMsg = std::string("Invalid move: ") + e.what();
                    }
                }
            }
        }

        // ── Render ───────────────────────────────────────────────────────────
        window.clear(boardColor);

        // 1) Grid lines
        for (int i = 0; i < BOARD_SIZE; i++) {
            sf::Vertex vLine[] = {
                sf::Vertex(sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN),                    sf::Color::Black),
                sf::Vertex(sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN + BOARD_PIXELS),     sf::Color::Black)
            };
            sf::Vertex hLine[] = {
                sf::Vertex(sf::Vector2f(MARGIN,                 MARGIN + i * CELL_SIZE),    sf::Color::Black),
                sf::Vertex(sf::Vector2f(MARGIN + BOARD_PIXELS,  MARGIN + i * CELL_SIZE),    sf::Color::Black)
            };
            window.draw(vLine, 2, sf::Lines);
            window.draw(hLine, 2, sf::Lines);
        }

        // 2) Star points (hoshi)
        for (int r : hoshi) {
            for (int c : hoshi) {
                sf::CircleShape dot(4);
                dot.setFillColor(sf::Color::Black);
                dot.setOrigin(4, 4);
                dot.setPosition(MARGIN + c * CELL_SIZE, MARGIN + r * CELL_SIZE);
                window.draw(dot);
            }
        }

        // 3) Stones
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                Cell cell = game.getCell(r, c); // assumes getCell(row,col) exists
                if (cell == EMPTY) continue;

                sf::CircleShape stone(STONE_RADIUS);
                stone.setOrigin(STONE_RADIUS, STONE_RADIUS);
                stone.setPosition(MARGIN + c * CELL_SIZE, MARGIN + r * CELL_SIZE);

                if (cell == BLACK) {
                    stone.setFillColor(sf::Color::Black);
                    stone.setOutlineColor(sf::Color(50, 50, 50));
                } else {
                    stone.setFillColor(sf::Color::White);
                    stone.setOutlineColor(sf::Color(180, 180, 180));
                }
                stone.setOutlineThickness(1.5f);
                window.draw(stone);
            }
        }

        // 4) Bottom info bar (captures + status)
        if (fontLoaded) {
            int blackCaps = game.getCaptures(BLACK); // assumes getCaptures() exists
            int whiteCaps = game.getCaptures(WHITE);

            auto makeText = [&](const std::string& str, float x, float y, unsigned size, sf::Color color) {
                sf::Text t;
                t.setFont(font);
                t.setString(str);
                t.setCharacterSize(size);
                t.setFillColor(color);
                t.setPosition(x, y);
                window.draw(t);
            };

            float barY = WINDOW_WIDTH + 8;
            makeText("Black captures: " + std::to_string(blackCaps), MARGIN,           barY,      16, sf::Color::Black);
            makeText("White captures: " + std::to_string(whiteCaps), MARGIN + 280,     barY,      16, sf::Color::Black);
            makeText(statusMsg,                                        MARGIN,           barY + 26, 18,
                     gameOver ? sf::Color(180, 0, 0) : sf::Color::Black);
        }

        window.display();
    }

    return 0;
}
