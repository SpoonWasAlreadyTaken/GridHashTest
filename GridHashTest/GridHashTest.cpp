#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>


void Draw();


int main()
{
    
    std::thread thDraw(Draw);

    while (true)
    {


    }

    thDraw.join();
}


void Draw()
{
    sf::RenderWindow window(sf::VideoMode({ 1000, 1000 }), "Particles");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}
