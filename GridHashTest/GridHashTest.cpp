#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <cmath>

#include "FaultyUtilitiesMT.hpp"
TaskSystem mt(10);

std::chrono::steady_clock::time_point t1;
std::chrono::steady_clock::time_point t2;
std::chrono::microseconds singleMS;

#include "PhysicsSolver.hpp"
#include "Particle.hpp"
#include "SpatialHashing.hpp"
#include "FaultyUtilities.hpp"




// variable decleration

uint32_t const sizeX = 1920;
uint32_t const sizeY = 1080;

float elapsedTime = 0;

float const particleSize = 2;

int const toSpawn = 80000;





// function declerations
void Draw(sf::RenderWindow& window);
void DrawRange(uint32_t start, uint32_t span, uint32_t leftOver);


// class instancing
PhysicsSolver physicsSolver = PhysicsSolver(sizeX, sizeY, 8, particleSize);


// rending declerations
sf::CircleShape circle(1.0f);

// vertex array method
size_t vertexBuffer = toSpawn * 6;//(int)((float)sizeX / particleSize) * (int)((float)sizeY / particleSize) * 6 * 2;
sf::VertexArray quad(sf::PrimitiveType::Triangles, vertexBuffer);
sf::Texture sprite;




int main()
{
    int steps = 0;
    bool stopGap = true;
    bool stopGap2 = true;
    sf::RenderWindow window(sf::VideoMode({ (uint32_t)sizeX, (uint32_t)sizeY }), "Particles", sf::State::Windowed);

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
    float ms = 0;
    float avgMS = 0;

    // text code
    sf::Font font;
    font.openFromFile("./Assets\\VIRUST.ttf"); // set to the font path


    sf::Text fpsText(font, "60");
    sf::Text msText(font, "0.0166666666s");
    sf::Text particleCountText(font, std::to_string(physicsSolver.particles.size()));
    fpsText.setFillColor(sf::Color(0, 255, 0));
    msText.setFillColor(sf::Color(255, 255, 255, 150));
    particleCountText.setFillColor(sf::Color(255, 0, 0));
    fpsText.setPosition(sf::Vector2f(5, 5));
    msText.setPosition(sf::Vector2f(5, 35));
    particleCountText.setPosition(sf::Vector2f(5, 70));

    sf::RectangleShape gridObject(sf::Vector2f(physicsSolver.spatialHashing.cellSize, physicsSolver.spatialHashing.cellSize));
    gridObject.setFillColor(sf::Color::Black);
    gridObject.setOutlineThickness(1.f);
    gridObject.setOutlineColor(sf::Color(255, 255, 255, 100));


    // spawning initial particles
    for (int i = 0; i < toSpawn; i++)
    {
        physicsSolver.AddParticle();
    }


    // main loop
    while (window.isOpen())
    {
        if (avgFPS > 60)
        {
            //physicsSolver.AddParticle();
        }

        particleCountText.setString(std::to_string(physicsSolver.particles.size()));

        currentTime = clock.restart().asSeconds();
        fps += 1 / currentTime;
        ms += currentTime;
        elapsedTime += currentTime;

        if (steps == 10)
        {
            avgFPS = fps / steps;
            avgMS = ms / steps;
            fpsText.setString(std::to_string(avgFPS));
            msText.setString(std::to_string(avgMS) + "s");
            fps = 0;
            ms = 0;
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
                physicsSolver.gravityMultiplier *= 2.f;
                std::cout << "Gravity: " << physicsSolver.gravity.x * physicsSolver.gravityMultiplier << " | " << physicsSolver.gravity.y * physicsSolver.gravityMultiplier << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            {
                physicsSolver.gravityMultiplier /= 2.f;
                std::cout << "Gravity: " << physicsSolver.gravity.x * physicsSolver.gravityMultiplier << " | " << physicsSolver.gravity.y * physicsSolver.gravityMultiplier << "\n";
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G) && stopGap)
            {
                physicsSolver.GravitySwitch();
                std::cout << "Gravity: " << physicsSolver.gravityON << "\n";
                stopGap = false;
                break;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && stopGap)
            {
                for (uint32_t p = 0; p < physicsSolver.particles.size(); p++) physicsSolver.particles[p].Stop();
                stopGap = false;
                break;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                float correct = sizeX / window.getSize().x;
                physicsSolver.Force(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * correct, 1);
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                float correct = sizeX / window.getSize().x;
                physicsSolver.Force(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * correct, -1);
            }
        }
        window.clear();

        Draw(window);

        window.draw(fpsText);
        window.draw(msText);
        window.draw(particleCountText);

        window.display();

        physicsSolver.PhysicsUpdate();

        if (stopGap2)
        {
            stopGap = true;
        }
        stopGap2 = !stopGap;
        
       
        //window.setFramerateLimit(120);
    }

    mt.~TaskSystem();
}

// functions

void Draw(sf::RenderWindow& window)
{
    t1 = std::chrono::high_resolution_clock::now();

    uint32_t count = physicsSolver.particles.size();
    uint32_t span = count / mt.ActiveThreads();
    uint32_t leftOver = count - (span * mt.ActiveThreads());

    for (uint8_t i = 0; i < mt.ActiveThreads(); i++)
    {
        mt.AddTask(DrawRange, i * span, span, leftOver * (i == mt.ActiveThreads() - 1));
    }
    mt.WaitForComplete();
    window.draw(quad, &sprite);
    t2 = std::chrono::high_resolution_clock::now();
    singleMS = duration_cast<std::chrono::microseconds>(t2 - t1);
    //std::cout << "Draw Time: " << singleMS.count() << "\n";
}


void DrawRange(uint32_t start, uint32_t span, uint32_t leftOver) // draws particles in selected range
{
    uint32_t end = start + span + leftOver;
    
    for (uint32_t i = start; i < end; i++)
    {
        int const index = i * 6;

        uint8_t speed = ((fabs(physicsSolver.particles[i].GetVelocity().x) + fabs(physicsSolver.particles[i].GetVelocity().y)) * 120) + 40;
        sf::Color color = sf::Color(speed, 0, 80);
        //sf::Color color = sf::Color(speed * 2 * acos(0), 0, 80);
        /*
        float d = elapsedTime + ((float)i * 0.0001) ;
        uint8_t r = 255.f * sin(d) * sin(d);
        uint8_t g = 255.f * sin(d + 0.33 * 2.f * acos(0)) * sin(d + 0.33 * 2.f * acos(0));
        uint8_t b = 255.f * sin(d + 0.66 * 2.f * acos(0)) * sin(d + 0.66 * 2.f * acos(0));
        sf::Color color = sf::Color(r, g, b);
        */

        quad[index].color = color;
        quad[index + 1].color = color;
        quad[index + 2].color = color;

        quad[index + 3].color = color;
        quad[index + 4].color = color;
        quad[index + 5].color = color;

        // position assignment
        quad[index].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y);
        quad[index + 1].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y);
        quad[index + 2].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);

        quad[index + 3].position = sf::Vector2f(physicsSolver.particles[i].position.x, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);
        quad[index + 4].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y);
        quad[index + 5].position = sf::Vector2f(physicsSolver.particles[i].position.x + physicsSolver.particles[i].size * 2, physicsSolver.particles[i].position.y + physicsSolver.particles[i].size * 2);
    }
}

