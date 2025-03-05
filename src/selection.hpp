#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <random>

#include "blocks.hpp"

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
        return value[(start + index + 1) % Size];
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
    CircularBuffer<Tretomino, NEXT_QUEUE_SIZE> next_queue {};
    ShuffleBag shufflebag {};
    std::optional<Tretomino> hold_tretomino {};

public:
    Tretomino next_tretomino() {
        return next_queue.add(shufflebag.take_one());
    }    

    std::optional<Tretomino> replace_hold_tretomino(Tretomino new_one) {
        std::optional<Tretomino> old_one = hold_tretomino;
        hold_tretomino = new_one;
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
        for (size_t i = 0; i < next_queue.size(); i++) {
            next_queue.add(shufflebag.take_one());
        }
    }
    ~Selection() = default;
};
