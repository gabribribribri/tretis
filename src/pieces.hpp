#include <array>
#include <cstdint>

// is constexpr inline ?
namespace Pieces {

struct Coo {
    int8_t x, y;
};

using Piece = std::array<std::array<Coo, 4>, 4>;

constexpr Piece I { {
    { { { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 } } },
    { { { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 } } },
    { { { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 } } },
    { { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } } },
} };

constexpr std::array<Piece, 1> ALL { { I } };

}  // namespace Pieces
