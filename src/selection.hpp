#pragma once

#include <cassert>
#include <optional>
#include <random>

#include "blocks.hpp"
#include "tretomino_render_shape.hpp"

const size_t NEXT_QUEUE_SIZE = 5;

const std::array<Tretomino, TRETOMINO_COUNT> ALL_ENUM_TRETOMINOS {
    Tretomino::T, Tretomino::O, Tretomino::I, Tretomino::L,
    Tretomino::J, Tretomino::S, Tretomino::Z,
};

template <typename Elem, size_t Size>
struct CircularBuffer {
    std::array<Elem, Size> value {};
    size_t start = 0;

    // returns the first element to be added
    Elem add(Elem elem) {
        Elem first_element = value[start];
        value[start] = elem;
        start += 1;
        start %= Size;
        return first_element;
    }

    [[nodiscard]] Elem at(size_t index) {
        assert(index < Size);
        return value[(start + index) % Size];
    }

    [[nodiscard]] size_t size() const { return Size; }

    CircularBuffer() = default;
};

struct ShuffleBag {
    std::random_device device {};
    std::mt19937 rng;  // Really I... I don't know what to do with this thing.
    std::array<Tretomino, TRETOMINO_COUNT> bag {};
    size_t items_left {};

    ShuffleBag();

    Tretomino take_one();

    void repopulate();
};

class Selection {
public:
    ShuffleBag shufflebag {};

    CircularBuffer<Tretomino, NEXT_QUEUE_SIZE> next_queue {};
    std::array<TretominoRenderShape, NEXT_QUEUE_SIZE> next_queue_shapes {};

    std::optional<Tretomino> hold_tretomino {};
    bool hold_locked = false;
    TretominoRenderShape hold_shape {};


public:
    Tretomino next_tretomino();    

    void refresh_next_queue_shapes();

    std::optional<Tretomino> replace_hold_tretomino(Tretomino new_one);
public:
    Selection(Selection const&) = delete;
    Selection(Selection&&) = delete;
    Selection operator=(Selection) = delete;

    static Selection& Get();

private:
    Selection();
    ~Selection() = default;
};
