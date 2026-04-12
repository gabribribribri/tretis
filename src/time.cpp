#include "time.hpp"

Chronometre::Chronometre(sf::Time time) : time(time) {}

bool Chronometre::has_time_passed() {
    if (clock.getElapsedTime() > time) {
        clock.restart();
        return true;
    }
    return false;
}

void Chronometre::restart() { clock.restart(); }
