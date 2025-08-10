#include "movements.hpp"

void Movements::ping_lateral() {
    if (!lateral_pressed) {
        return;
    }

    if (lateral_auto_repeat_enabled and
        lateral_auto_repeat_interval.has_time_passed()) {
        Grid::Get().move_crbl(lateral_direction);
    } else if (lateral_auto_repeat_delay.has_time_passed()) {
        Grid::Get().move_crbl(lateral_direction);
        lateral_auto_repeat_enabled = true;
        lateral_auto_repeat_interval.restart();
    }
}

void Movements::ping_vertical() {
    if (!vertical_pressed) {
        return;
    }

    if (vertical_auto_repeat_interval.has_time_passed()) {
        move_and_stop_if_crbl_placed();
        Score::Get().add_soft_drop();
    }
}

void Movements::move_and_stop_if_crbl_placed() {
    if (Grid::Get().move_crbl_down_or_place()) {
        stop_lateral();
        stop_vertical();
    }
}

void Movements::go_lateral(Coo direction) {
    if (lateral_pressed and lateral_direction == direction) {
        return;
    }
    lateral_direction = direction;
    lateral_pressed = true;
    lateral_auto_repeat_enabled = false;
    lateral_auto_repeat_delay.restart();
    Grid::Get().move_crbl(lateral_direction);
}


void Movements::go_vertical() {
    if (vertical_pressed) {
        return;
    }
    vertical_pressed = true;
    vertical_auto_repeat_interval.restart();
    move_and_stop_if_crbl_placed();
}

void Movements::stop_lateral() {
    lateral_pressed = false;
    lateral_auto_repeat_enabled = false;
}

void Movements::stop_vertical() { vertical_pressed = false; }

void Movements::ping() {
    ping_lateral();
    ping_vertical();

    if (!vertical_pressed and
        crbl_fall_by_one_countdown.has_time_passed()) {
        move_and_stop_if_crbl_placed();
    }
}

Movements& Movements::Get() {
    static Movements instance;
    return instance;
}
