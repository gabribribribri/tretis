#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <thread>
#include "grid.hpp"

constexpr float CELL_SIZE = 33.0f;

constexpr uint32_t FRAME_PER_SECOND = 120;
constexpr std::chrono::microseconds USEC_PER_FRAME =
    std::chrono::microseconds(1'000'000 / FRAME_PER_SECOND);

class Game {
public:
    // Base init has everything you need
    Game() = default;

    Game(const uint32_t grid_width, const uint32_t grid_height)
        : m_grid(grid_width, grid_height) {};

    // Oh I'm not so sure about that
    void run(std::function<void(Game&)> callback) {
        m_continue_gameloop = true;
        while (m_continue_gameloop) {
            const std::chrono::time_point start_frame =
                std::chrono::high_resolution_clock::now();
            callback(*this);
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
                        case sf::Keyboard::Key::S:
                            std::cout << "You pressed the S key\n";
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
                        sf::Vector2f(event.size.width/2.0, event.size.height/2.0),
                        sf::Vector2f(event.size.width, event.size.height)));
                    break;
                case sf::Event::Closed:
                    exit(0);
                default:
                    break;
            }
        }
    }

    void display_cells() {
        for (uint32_t yc = 0; yc < m_grid.height(); yc++) {
            for (uint32_t xc = 0; xc < m_grid.width(); xc++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setFillColor(m_grid.at(xc, yc));
                cell.setPosition(sf::Vector2f(
                    left_offset() + xc * CELL_SIZE,
                    up_offset() + (m_grid.height() - yc) * CELL_SIZE));
                m_win.draw(cell);
            }
        }
    }

    void main_gameloop() {
        handle_events();

        m_win.clear(sf::Color(69, 69, 69));
        display_cells();
        m_win.display();
    }

    float left_offset() const {
        return (m_win.getSize().x / 2.0f) -
               ((CELL_SIZE * m_grid.width()) / 2.0f);
    }
    float up_offset() const {
        return (m_win.getSize().y / 2.0f) -
               ((CELL_SIZE * m_grid.height()) / 2.0f);
    }

private:
    Grid m_grid { 10, 20 };
    bool m_continue_gameloop = true;
    sf::RenderWindow m_win { sf::VideoMode(800, 600), std::string("Tretis") };
};
