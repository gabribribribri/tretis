#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

/// TIME CONSTANTS ///
const int FRAME_PER_SECONDS = 240;
const sf::Time TIME_PER_FRAME = sf::milliseconds(1000/FRAME_PER_SECONDS);
const sf::Time BASE_BLOCK_FALL_BY_ONE = sf::seconds(1.0);
const sf::Time LATERAL_AUTO_REPEAT_DELAY = sf::milliseconds(200);
const sf::Time AUTO_REPEAT_INTERVAL = sf::milliseconds(50);


/// CHRONOMETRE ///
class Chronometre {
public:
    sf::Clock clock;
    sf::Time time;
public:
    Chronometre(sf::Time time) : time(time) {}

    /// if time has passed and
    /// and if it has passed restart clock
    bool has_time_passed() {
        if (clock.getElapsedTime() > time) {
            clock.restart();
            return true;
        }
        return false;
    }
        
    void wait_until_time_has_passed() {
        sf::sleep(time - clock.getElapsedTime());
        clock.restart();
    }

    void restart() {
        clock.restart();
    }
};
