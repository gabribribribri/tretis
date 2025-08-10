#include "selection.hpp"

ShuffleBag::ShuffleBag() : rng(device()) { repopulate(); }

Tretomino ShuffleBag::take_one() {
    if (items_left == 0) {
        repopulate();
    }
    return bag[items_left--];
}

void ShuffleBag::repopulate() {
    items_left = bag.size() - 1;
    bag = ALL_ENUM_TRETOMINOS;
    std::shuffle(bag.begin(), bag.end(), rng);
}

Tretomino Selection::next_tretomino() {
    Tretomino next_tretomino = next_queue.add(shufflebag.take_one());
    refresh_next_queue_shapes();
    return next_tretomino;
}    

void Selection::refresh_next_queue_shapes() {
    for (size_t i = 0; i < next_queue.size(); i++) {
        next_queue_shapes[i].set_tretomino(next_queue.at(i));
    }
}

std::optional<Tretomino> Selection::replace_hold_tretomino(Tretomino new_one) {
    assert(!hold_locked);
    std::optional<Tretomino> old_one = hold_tretomino;
    hold_tretomino = new_one;
    // forcément has_value(). T'façon ça crash si jamais donc trkl
    hold_shape.set_tretomino(*hold_tretomino);
    return old_one;
}

Selection& Selection::Get() {
    static Selection instance;
    return instance;
}

Selection::Selection() {
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
