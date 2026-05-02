#include "gomoku.hpp"

int main() {
    Gomoku game;
    Cell currentTurn = BLACK;
    bool gameOver    = false;
    std::string statusMsg = "Black's turn";

    // SFML 3.0: VideoMode uses Vector2u
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Gomoku");
    window.setFramerateLimit(60);

    sf::Font font;
    // SFML 3.0: Renamed loadFromFile to openFromFile
    if (!font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "Font not found!" << std::endl;
    }

    sf::Color boardColor(220, 179, 92);
    const int hoshi[] = {3, 9, 15};

    while (window.isOpen()) {
        // SFML 3.0: New Event Polling System
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* mousePtr = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePtr->button == sf::Mouse::Button::Left && !gameOver) {
                    int col = (mousePtr->position.x - MARGIN + CELL_SIZE / 2) / CELL_SIZE;
                    int row = (mousePtr->position.y - MARGIN + CELL_SIZE / 2) / CELL_SIZE;

                    try {
                        game.applyMove(row, col, currentTurn);
                        Cell opp = game.opponent(currentTurn);
                                        
                        // 1. Check if current player wins by 10 captures
                        if (game.hasTenCaptures(currentTurn)) {
                            statusMsg = (currentTurn == BLACK ? "Black" : "White");
                            statusMsg += " wins by 10 captures!";
                            gameOver = true;
                        } 
                        // 2. Check if the OPPONENT had a 5-in-a-row that survived this turn
                        // (If the current player didn't break the opponent's 5, the opponent wins now)
                        else if (game.hasFive(opp)) {
                            statusMsg = (opp == BLACK ? "Black" : "White");
                            statusMsg += " wins! (5-in-a-row was not broken)";
                            gameOver = true;
                        }
                        // 3. Check if the current player just formed a 5-in-a-row
                        else if (game.hasFiveAt(row, col, currentTurn)) {
                            // If it's NOT breakable, they win immediately
                            if (!game.isFiveBreakable(currentTurn)) {
                                statusMsg = (currentTurn == BLACK ? "Black" : "White");
                                statusMsg += " wins with 5-in-a-row!";
                                gameOver = true;
                            } else {
                                statusMsg = "5-in-a-row formed! White must break it or lose.";
                            }
                        }
                    
                        if (!gameOver) {
                            currentTurn = opp;
                            if (statusMsg.find("formed") == std::string::npos) { // Don't overwrite the warning msg
                                statusMsg = (currentTurn == BLACK ? "Black's turn" : "White's turn");
                            }
                        }
                    } catch (const std::invalid_argument& e) {
                        statusMsg = std::string("Invalid move: ") + e.what();
                    }
                }
            }
        }

        window.clear(boardColor);

        // 1) Grid lines - SFML 3.0: sf::PrimitiveType::Lines
        for (int i = 0; i < BOARD_SIZE; i++) {
            sf::Vertex vLine[] = {
                { sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN), sf::Color::Black },
                { sf::Vector2f(MARGIN + i * CELL_SIZE, MARGIN + BOARD_PIXELS), sf::Color::Black }
            };
            sf::Vertex hLine[] = {
                { sf::Vector2f(MARGIN, MARGIN + i * CELL_SIZE), sf::Color::Black },
                { sf::Vector2f(MARGIN + BOARD_PIXELS, MARGIN + i * CELL_SIZE), sf::Color::Black }
            };
            window.draw(vLine, 2, sf::PrimitiveType::Lines);
            window.draw(hLine, 2, sf::PrimitiveType::Lines);
        }

        // 2) Star points
        for (int r : hoshi) {
            for (int c : hoshi) {
                sf::CircleShape dot(4);
                dot.setFillColor(sf::Color::Black);
                dot.setOrigin({4.f, 4.f}); // SFML 3.0: requires Vector2f
                dot.setPosition({(float)MARGIN + c * CELL_SIZE, (float)MARGIN + r * CELL_SIZE});
                window.draw(dot);
            }
        }

        // 3) Stones
        for (int r = 0; r < BOARD_SIZE; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                Cell cell = game.getCell(r, c);
                if (cell == EMPTY) continue;

                sf::CircleShape stone((float)STONE_RADIUS);
                stone.setOrigin({(float)STONE_RADIUS, (float)STONE_RADIUS});
                stone.setPosition({(float)MARGIN + c * CELL_SIZE, (float)MARGIN + r * CELL_SIZE});

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

        // 4) UI Text - SFML 3.0: Text constructor requires the font
        int blackCaps = game.getCaptures(BLACK);
        int whiteCaps = game.getCaptures(WHITE);

        auto drawText = [&](const std::string& str, float x, float y, unsigned size, sf::Color color) {
            sf::Text t(font, str, size); // SFML 3.0 constructor
            t.setFillColor(color);
            t.setPosition({x, y});
            window.draw(t);
        };

        float barY = (float)WINDOW_WIDTH + 8;
        drawText("Black captures: " + std::to_string(blackCaps), MARGIN, barY, 16, sf::Color::Black);
        drawText("White captures: " + std::to_string(whiteCaps), MARGIN + 280, barY, 16, sf::Color::Black);
        drawText(statusMsg, MARGIN, barY + 26, 18, gameOver ? sf::Color(180, 0, 0) : sf::Color::Black);

        window.display();
    }

    return 0;
}