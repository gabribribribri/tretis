#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <iostream>
#include <random>
#include <thread>

#include "grid.hpp"
#include "pieces.hpp"

constexpr float CELL_SIZE = 33.0f;

constexpr uint32_t FRAME_PER_SECOND = 120;
constexpr std::chrono::microseconds USEC_PER_FRAME =
    std::chrono::microseconds(1'000'000 / FRAME_PER_SECOND);

class Game {
public:
    // Base init has everything you need
    Game() : m_gen(m_rnd()) { select_new_piece(); };

    Game(const uint32_t grid_width, const uint32_t grid_height)
        : m_blocks_grid(grid_width, grid_height),
          m_print_grid(grid_width, grid_height),
          m_gen(m_rnd()) {
        select_new_piece();
    };

    // Oh I'm not so sure about that
    void run(std::function<bool(Game&)> callback) {
        for (;;) {
            const std::chrono::time_point start_frame =
                std::chrono::high_resolution_clock::now();
            if (!callback(*this)) {
                return;
            }
            const std::chrono::time_point end_frame =
                std::chrono::high_resolution_clock::now();
            const std::chrono::duration frame_duration =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    end_frame - start_frame);
            std::this_thread::sleep_for(USEC_PER_FRAME - frame_duration);
        }
    }

    void handle_events() {
        sf::Event event;
        while (m_win.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Key::W:
                            switch_rotation();
                            break;
                        case sf::Keyboard::Key::Escape:
                            exit(0);
                            break;
                        default:
                            break;
                    }
                    break;
                case sf::Event::Resized:
                    m_win.setView(sf::View(
                        sf::Vector2f(event.size.width / 2.0,
                                     event.size.height / 2.0),
                        sf::Vector2f(event.size.width, event.size.height)));
                    break;
                case sf::Event::Closed:
                    exit(0);
                default:
                    break;
            }
        }
    }

    void switch_rotation() { m_fp_rotation = (m_fp_rotation + 1) % 4; }

    void select_new_piece() {
        std::uniform_int_distribution<> distrib(0, Pieces::ALL.size() - 1);
        m_fp_coo = { 3, 1 };
        m_falling_piece = Pieces::ALL.at(distrib(m_gen));
        m_fp_color = Pieces::COLORS.at(distrib(m_gen));
    }

    void populate_print_grid() {
        m_print_grid.refresh_with(m_blocks_grid);
        for (Pieces::Coo falling_cell_offset : m_falling_piece[m_fp_rotation]) {
            // construct cell
            Pieces::Coo falling_cell { m_fp_coo.x + falling_cell_offset.x,
                                       m_fp_coo.y + falling_cell_offset.y };
            // bornes check
            assert(falling_cell.x >= 0);
            assert(falling_cell.y >= 0);
            assert(falling_cell.x < m_print_grid.width());
            assert(falling_cell.y < m_print_grid.height());
            // place on the grid
            m_print_grid.at(falling_cell.x, falling_cell.y) = m_fp_color;
        }
    }

    void display_cells() const {
        for (uint32_t yc = 0; yc < m_blocks_grid.height(); yc++) {
            for (uint32_t xc = 0; xc < m_blocks_grid.width(); xc++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setFillColor(m_print_grid.at(xc, yc));
                cell.setPosition(sf::Vector2f(left_offset() + xc * CELL_SIZE,
                                              up_offset() + yc * CELL_SIZE));
                m_win.draw(cell);
            }
        }
    }

    bool main_gameloop() {
        // game logic
        handle_events();
        populate_print_grid();

        // game rendering
        m_win.clear(sf::Color(69, 69, 69));
        display_cells();
        m_win.display();
        return true;
    }

    float left_offset() const {
        return (m_win.getSize().x / 2.0f) -
               ((CELL_SIZE * m_blocks_grid.width()) / 2.0f);
    }
    float up_offset() const {
        return (m_win.getSize().y / 2.0f) -
               ((CELL_SIZE * m_blocks_grid.height()) / 2.0f);
    }

private:
    mutable sf::RenderWindow m_win { sf::VideoMode(800, 600),
                                     std::string("Tretis") };

    Grid m_blocks_grid { 10, 20 };
    Grid m_print_grid { 10, 20 };
    // bool m_continue_gameloop = true;

    Pieces::Piece m_falling_piece = Pieces::I;
    sf::Color m_fp_color = sf::Color::Cyan;
    uint8_t m_fp_rotation;
    Pieces::Coo m_fp_coo;

    std::random_device m_rnd;
    std::mt19937 m_gen;
    std::chrono::time_point<std::chrono::system_clock> m_last_downing_piece;
    std::chrono::microseconds n_down_elapse =
        std::chrono::microseconds(1'200'000);
};
