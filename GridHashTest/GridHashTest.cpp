#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>
#include <unordered_map>

#include "PhysicsSolver.hpp"
#include "Particle.hpp"
#include "SpatialHashing.hpp"
#include "Spoon.hpp"





int sizeX = 800;
int sizeY = 800;


// function declerations

void Draw(sf::RenderWindow& window);


PhysicsSolver physicsSolver = PhysicsSolver(sizeX, sizeY, 8);


int toSpawn = 1000;
sf::CircleShape circle(1.0f);



int main()
{
    int steps = 0;
    bool stopGap = true;
    bool stopGap2 = true;
    sf::RenderWindow window(sf::VideoMode({ (uint32_t)sizeX, (uint32_t)sizeY }), "Particles");

    sf::Clock clock;
    circle.setPointCount(8);
    float currentTime = 0;
    int fps = 0;
    int avgFPS = 0;

    sf::Font virust_font;
    virust_font.openFromFile("C:\\Users\\PREDATOR\\Documents\\VisualStudioStuff\\Fonts\\VIRUST.ttf");

    sf::Text fpsText(virust_font, "60");
    sf::Text particleCountText(virust_font, std::to_string(physicsSolver.particles.size()));
    fpsText.setFillColor(sf::Color(0, 255, 0));
    particleCountText.setFillColor(sf::Color(255, 0, 0));
    fpsText.setPosition(sf::Vector2f(5, 0));
    particleCountText.setPosition(sf::Vector2f(5, 30));

    //sf::RectangleShape gridObject(sf::Vector2f(hasher.gridResolution, hasher.gridResolution));
    //gridObject.setFillColor(sf::Color::Black);
    //gridObject.setOutlineThickness(1.f);
    //gridObject.setOutlineColor(sf::Color(255, 255, 255, 100));

    for (int i = 0; i < toSpawn; i++)
    {
        //physicsSolver.AddParticle();
    }

    while (window.isOpen())
    {
        if (avgFPS > 60)
        {
            physicsSolver.AddParticle();
        }

        particleCountText.setString(std::to_string(physicsSolver.particles.size()));

        currentTime = clock.restart().asSeconds();
        fps += 1.f / (currentTime);

        if (steps == 10)
        {
            avgFPS = fps / steps;
            fpsText.setString(std::to_string(avgFPS));
            fps = 0;
            steps = 0;
        }
        steps++;

        
        while (const std::optional event = window.pollEvent())
        {
            bool alreadyPressed = false;

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                physicsSolver.gravity = physicsSolver.gravity.rotatedBy(sf::degrees(45));
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            {
                physicsSolver.gravity = physicsSolver.gravity.rotatedBy(sf::degrees(-45));
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            {
                physicsSolver.gravity *= 2.f;
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                physicsSolver.gravity /= 2.f;
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G) && stopGap)
            {
                physicsSolver.GravitySwitch();
                std::cout << "Gravity: " << physicsSolver.gravityON << "\n";
                stopGap = false;
                break;
            }
        }
        window.clear();
        Draw(window);
        window.draw(fpsText);
        window.draw(particleCountText);

        window.display();

        physicsSolver.PhysicsUpdate();
        if (stopGap2)
        {
            stopGap = true;
        }
        stopGap2 = !stopGap;
    }
}

// functions

void Draw(sf::RenderWindow& window)
{
    for (int i = 0; i < physicsSolver.particles.size(); i++)
    {
        circle.setPosition(physicsSolver.particles.at(i).position);
        circle.setScale(sf::Vector2f(physicsSolver.particleSize, physicsSolver.particleSize));
        circle.setFillColor(physicsSolver.particles.at(i).color);
        window.draw(circle);
    }
}



