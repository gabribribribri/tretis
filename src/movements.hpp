#pragma once

#include <SFML/System/Time.hpp>

#include "grid.hpp"
#include "time.hpp"
#include "tretis.hpp"

class Movements {
private:
    Coo lateral_direction;  // uninitialized ?
    bool vertical_pressed = false;
    bool lateral_pressed = false;
    bool lateral_auto_repeat_enabled = false;

public:
    Chronometre lateral_auto_repeat_delay { LATERAL_AUTO_REPEAT_DELAY };
    Chronometre lateral_auto_repeat_interval { AUTO_REPEAT_INTERVAL };
    Chronometre vertical_auto_repeat_interval { AUTO_REPEAT_INTERVAL };
    Chronometre crbl_fall_by_one_countdown =
        Score::Get().get_drop_speed_from_level();

private:
    void ping_lateral() {
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

    void ping_vertical() {
        if (!vertical_pressed) {
            return;
        }

        if (vertical_auto_repeat_interval.has_time_passed()) {
            Grid::Get().move_crbl_down_or_place();
        }
    }

    void stop_moving_if_crbl_placed() {
        if (Grid::Get().move_crbl_down_or_place()) {
            stop_lateral();
            stop_vertical();
        }
    }

public:
    void go_lateral(Coo direction) {
        if (lateral_pressed and lateral_direction == direction) {
            return;
        }
        lateral_direction = direction;
        lateral_pressed = true;
        lateral_auto_repeat_enabled = false;
        lateral_auto_repeat_delay.restart();
        Grid::Get().move_crbl(lateral_direction);
    }

    void go_vertical() {
        if (vertical_pressed) {
            return;
        }
        vertical_pressed = true;
        vertical_auto_repeat_interval.restart();
        stop_moving_if_crbl_placed();
    }

    void stop_lateral() {
        lateral_pressed = false;
        lateral_auto_repeat_enabled = false;
    }

    void stop_vertical() { vertical_pressed = false; }

    void ping() {
        ping_lateral();
        ping_vertical();

        if (!vertical_pressed and
            crbl_fall_by_one_countdown.has_time_passed()) {
            stop_moving_if_crbl_placed();
        }
    }

private:
    Movements() = default;
    ~Movements() = default;

public:
    static Movements& Get() {
        static Movements instance;
        return instance;
    }

    Movements(Movements const&) = delete;
    Movements(Movements&&) = delete;
    Movements operator=(Movements) = delete;
};
