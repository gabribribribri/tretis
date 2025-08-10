#pragma once

#include <SFML/System/Time.hpp>

#include "time.hpp"
#include "score.hpp"
#include "blocks.hpp"

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
    void ping_lateral();

    void ping_vertical();

    void move_and_stop_if_crbl_placed();

public:
    void go_lateral(Coo direction);

    void go_vertical();

    void stop_lateral();

    void stop_vertical();

    void ping();

private:
    Movements() = default;
    ~Movements() = default;

public:
    static Movements& Get();

    Movements(Movements const&) = delete;
    Movements(Movements&&) = delete;
    Movements operator=(Movements) = delete;
};
