#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

#include "PhysicsSolver.hpp"
#include "Particle.hpp"
#include "SpatialHashing.hpp"
#include "FaultyUtilities.hpp"




// variable decleration
int sizeX = 1000;
int sizeY = 1000;

float const particleSize = 2.5;

int toSpawn = 6400;

// function declerations
void Draw(sf::RenderWindow& window);


// class instancing
PhysicsSolver physicsSolver = PhysicsSolver(sizeX, sizeY, 8, particleSize);


// rending declerations
sf::CircleShape circle(1.0f);

// vertex array method
size_t vertexBuffer = 100000;
sf::VertexArray quad(sf::PrimitiveType::Triangles, vertexBuffer);
sf::Texture sprite;



int main()
{
    int steps = 0;
    bool stopGap = true;
    bool stopGap2 = true;
    sf::RenderWindow window(sf::VideoMode({ (uint32_t)sizeX, (uint32_t)sizeY }), "Particles");

    for (int i = 0; i < vertexBuffer / 6; i++)
    {
        // texture cordinte assignment
        int index = i * 6;
        quad[index].texCoords = sf::Vector2f(0, 0);
        quad[index + 1].texCoords = sf::Vector2f(0, 512);
        quad[index + 2].texCoords = sf::Vector2f(512, 0);

        quad[index + 3].texCoords = sf::Vector2f(0, 0);
        quad[index + 4].texCoords = sf::Vector2f(512, 512);
        quad[index + 5].texCoords = sf::Vector2f(512, 0);

        // collor assignment

        float r = sin(((float)i + 500) * 0.001);
        float g = sin(r + 0.33 * 2 * (2 * acos(0)));
        float b = sin(r + 0.66 * 2 * (2 * acos(0)));
        sf::Color color = sf::Color(static_cast<uint8_t>(255.f * r * r), static_cast<uint8_t>(255.f * g * g), static_cast<uint8_t>(255.f * b * b));

        quad[index].color = color;
        quad[index + 1].color = color;
        quad[index + 2].color = color;

        quad[index + 3].color = color;
        quad[index + 4].color = color;
        quad[index + 5].color = color;
    }

    // rendering code
    circle.setPointCount(8);

    // vertex array method
    sprite.loadFromFile("./Assets\\CircleSprite.png"); // set to the texture path
    sprite.generateMipmap();
    sprite.setSmooth(true);
     
    // fps code
    sf::Clock clock;
    float currentTime = 0;
    int fps = 0;
    int avgFPS = 0;

    // text code
    sf::Font font;
    font.openFromFile("./Assets\\VIRUST.ttf"); // set to the font path


    sf::Text fpsText(font, "60");
    sf::Text particleCountText(font, std::to_string(physicsSolver.particles.size()));
    fpsText.setFillColor(sf::Color(0, 255, 0));
    particleCountText.setFillColor(sf::Color(255, 0, 0));
    fpsText.setPosition(sf::Vector2f(5, 0));
    particleCountText.setPosition(sf::Vector2f(5, 30));

    sf::RectangleShape gridObject(sf::Vector2f(physicsSolver.spatialHashing.cellSize, physicsSolver.spatialHashing.cellSize));
    gridObject.setFillColor(sf::Color::Black);
    gridObject.setOutlineThickness(1.f);
    gridObject.setOutlineColor(sf::Color(255, 255, 255, 100));


    // spawning initial particles
    for (int i = 0; i < toSpawn; i++)
    {
        //physicsSolver.AddParticle();
    }


    // main loop
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
        
       
        window.setFramerateLimit(120);
    }
}

// functions

void Draw(sf::RenderWindow& window)
{
    for (int i = 0; i < physicsSolver.particles.size(); i++)
    {
        int const index = i * 6;

        float speed = fabs((physicsSolver.particles[i].GetVelocity().x + physicsSolver.particles[i].GetVelocity().y) * 70) + 20;

        sf::Color color = sf::Color(speed , 0, 80);

        quad[index].color = color;
        quad[index + 1].color = color;
        quad[index + 2].color = color;

        quad[index + 3].color = color;
        quad[index + 4].color = color;
        quad[index + 5].color = color;

        // position assignment
        quad[index].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y);
        quad[index +1].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y);
        quad[index +2].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);

        quad[index +3].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);
        quad[index +4].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y);
        quad[index +5].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);
    }
    window.draw(quad, &sprite);
}



