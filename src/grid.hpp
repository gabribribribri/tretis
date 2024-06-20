#pragma once

#include <SFML/Graphics/Color.hpp>
#include <cassert>
#include <cstdint>
#include <vector>

class Grid {
public:
    Grid() = delete;
    Grid(const uint32_t width, const uint32_t height)
        : m_grid(width * height, sf::Color::Black),
          m_width(width),
          m_height(height) {}

    const sf::Color at(const uint32_t x, const uint32_t y) const {
        assert(x < m_width);
        assert(y < m_height);
        return m_grid[y * m_width + x];
    }

    sf::Color& at(const uint32_t x, const uint32_t y) {
        assert(x < m_width);
        assert(y < m_height);
        return m_grid[y * m_width + x];
    }

    uint32_t width()  const { return m_width;  } 
    uint32_t height() const { return m_height; }

private:
    std::vector<sf::Color> m_grid;
    uint32_t m_width;
    uint32_t m_height;
};
