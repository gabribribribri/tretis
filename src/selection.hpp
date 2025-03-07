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

    ShuffleBag() : rng(device()) { repopulate(); }

    Tretomino take_one() {
        if (items_left == 0) {
            repopulate();
        }
        return bag[items_left--];
    }

    void repopulate() {
        items_left = bag.size() - 1;
        bag = ALL_ENUM_TRETOMINOS;
        std::shuffle(bag.begin(), bag.end(), rng);
    }
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
    Tretomino next_tretomino() {
        Tretomino next_tretomino = next_queue.add(shufflebag.take_one());
        refresh_next_queue_shapes();
        return next_tretomino;
    }    

    void refresh_next_queue_shapes() {
        for (size_t i = 0; i < next_queue.size(); i++) {
            next_queue_shapes[i].set_tretomino(next_queue.at(i));
        }
    }

    std::optional<Tretomino> replace_hold_tretomino(Tretomino new_one) {
        assert(!hold_locked);
        std::optional<Tretomino> old_one = hold_tretomino;
        hold_tretomino = new_one;
        // forcément has_value(). T'façon ça crash si jamais donc trkl
        hold_shape.set_tretomino(*hold_tretomino);
        return old_one;
    }
public:
    Selection(Selection const&) = delete;
    Selection(Selection&&) = delete;
    Selection operator=(Selection) = delete;

    static Selection& Get() {
        static Selection instance;
        return instance;
    }

private:
    Selection() {
        // Fill the next_queue
        for (size_t i = 0; i < next_queue.size(); i++) {
            next_queue.add(shufflebag.take_one());
        }

        // Hold Tretomino Shape
        hold_shape.set_origin(-HOLD_PIECE_DELIMITER_POS);

        // Next Queue Shapes
        for (size_t i = 0; i < NEXT_QUEUE_SIZE; i++) {
            next_queue_shapes[i].set_origin({
                -NEXT_QUEUE_POS.x,
                -NEXT_QUEUE_POS.y - (NEXT_QUEUE_HEIGHT / NEXT_QUEUE_SIZE) * i,
            });
        }

        refresh_next_queue_shapes();
    }
    ~Selection() = default;
};
