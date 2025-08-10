#include "time.hpp"

Chronometre::Chronometre(sf::Time time) : time(time) {}

bool Chronometre::has_time_passed() {
    if (clock.getElapsedTime() > time) {
        clock.restart();
        return true;
    }
    return false;
}

void Chronometre::wait_until_time_has_passed() {
    sf::sleep(time - clock.getElapsedTime());
    clock.restart();
}

void Chronometre::restart() { clock.restart(); }
