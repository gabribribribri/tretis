/// ***
/// Petit point culture :
/// On habite avec un coeur plein un monde vide - Chateaubriand
/// Mieux une tête bien faite qu'un tête bien pleine - Montaigne
/// ***

#include <SFML/Graphics.hpp>
#include <functional>
#include "game.hpp"


int main() {
    Game game;
    game.run(std::bind(&Game::main_gameloop, &game)); // this is some level 26 c++ wizzard shit right there
}
