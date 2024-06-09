import my_module;

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    int i = my_module::bonsoir();
    std::cout << i << "\n";
}


// int main()
// {
//     // Create a window with a size of 800x600 pixels and titled "Basic SFML Window"
//     sf::RenderWindow window(sf::VideoMode(900, 600), "Basic SFML Window");
    

//     // Main loop
//     while (window.isOpen())
//     {
//         // Process events
//         sf::Event event;
//         while (window.pollEvent(event))
//         {
//             // Close window: exit
//             if (event.type == sf::Event::Closed)
//                 window.close();
//         }

//         // Clear screen
//         window.clear();

//         // Draw everything here...

//         // Update the window
//         window.display();

//         // wait
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//     }

//     return 0;
// }
