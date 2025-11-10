#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>

#include "FaultyUtilitiesMT.hpp"
TaskSystem mt(20);

std::chrono::high_resolution_clock::time_point t1;
std::chrono::high_resolution_clock::time_point t2;
std::chrono::microseconds singleMS;


// variable decleration

uint32_t constexpr sizeX = 1920;
uint32_t constexpr sizeY = 1080;

float elapsedTime = 0;
float deltaTime = 0;

float constexpr particleSize = 2;

int constexpr toSpawn = 80000;


#include "PhysicsSolver.hpp"


// function declerations
void Draw(sf::RenderWindow& window);
void DrawRange(uint32_t start, uint32_t span, uint32_t leftOver);


// class instancing
PhysicsSolver physicsSolver = PhysicsSolver(8);


// rendering declerations
sf::CircleShape circle(1.0f);

// vertex array method
size_t vertexBuffer = toSpawn * 6;//(int)((float)sizeX / particleSize) * (int)((float)sizeY / particleSize) * 6 * 2;
sf::VertexArray quad(sf::PrimitiveType::Triangles, vertexBuffer);
sf::Texture sprite;




int main()
{
    int steps = 0;
    bool stopGap = true;
    sf::RenderWindow window(sf::VideoMode({ (uint32_t)sizeX, (uint32_t)sizeY }), "Particles", sf::State::Windowed);

    for (uint32_t i = 0; i < vertexBuffer / 6; i++)
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
    if(!sprite.loadFromFile("./Assets/CircleSprite.png"))
    {
        std::cout << "ERROR: FAILED TO LOAD SPRITE!!!";
        return 1;
    } // set to the texture path
    sprite.setSmooth(true);
     
    // fps code
    sf::Clock clock;
    int fps = 0.f;
    int avgFPS = 0;
    float ms = 0;
    float avgMS = 0;

    // text code
    sf::Font font;
    if (!font.openFromFile("./Assets/VIRUST.ttf"))
    {
        std::cout << "ERROR: FAILED TO LOAD FONT!!!";
        return 1;
    } // set to the font path


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

        deltaTime = clock.restart().asSeconds();

        fps += 1 / deltaTime;
        ms += deltaTime;
        elapsedTime += deltaTime;

        if (steps == 10)
        {
            avgFPS = fps / steps;
            avgMS = ms / steps;
            fpsText.setString(std::to_string(avgFPS));
            msText.setString(std::to_string(avgMS) + "s");
            fps = 0;
            ms = 0;
            steps = 0;
            stopGap = true;
        }
        steps++;

        
        while (const std::optional event = window.pollEvent())
        {

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && stopGap)
            {
                physicsSolver.gravity = physicsSolver.gravity.rotatedBy(sf::degrees(45));
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && stopGap)
            {
                physicsSolver.gravity = physicsSolver.gravity.rotatedBy(sf::degrees(-45));
                std::cout << "Gravity: " << physicsSolver.gravity.x << " | " << physicsSolver.gravity.y << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && stopGap)
            {
                physicsSolver.gravityMultiplier *= 2;
                std::cout << "Gravity: " << physicsSolver.gravity.x * physicsSolver.gravityMultiplier << " | " << physicsSolver.gravity.y * physicsSolver.gravityMultiplier << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && stopGap)
            {
                physicsSolver.gravityMultiplier /= 2;
                std::cout << "Gravity: " << physicsSolver.gravity.x * physicsSolver.gravityMultiplier << " | " << physicsSolver.gravity.y * physicsSolver.gravityMultiplier << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G) && stopGap)
            {
                physicsSolver.GravitySwitch();
                std::cout << "Gravity: " << physicsSolver.gravityON << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && stopGap)
            {
                for (uint32_t p = 0; p < physicsSolver.particles.size(); p++) physicsSolver.particles[p].Stop();
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Add) && stopGap)
            {
                physicsSolver.forceMultiplier *= 1.1;
                std::cout << "Force Multiplier: " << physicsSolver.forceMultiplier << "\n";
                stopGap = false;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Subtract) && stopGap)
            {
                physicsSolver.forceMultiplier /= 1.1;
                std::cout << "Force Multiplier: " << physicsSolver.forceMultiplier << "\n";
                stopGap = false;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                float correct = sizeX / window.getSize().x;
                physicsSolver.Force(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * correct, physicsSolver.forceMultiplier);
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                float correct = sizeX / window.getSize().x;
                physicsSolver.Force(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) * correct, -1 * physicsSolver.forceMultiplier);
            }
        }
        window.clear();

        Draw(window);

        window.draw(fpsText);
        window.draw(msText);
        window.draw(particleCountText);

        window.display();

        physicsSolver.PhysicsUpdate();
        
       
        //window.setFramerateLimit(80);
    }

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
    singleMS = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
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
        float d = elapsedTime + ((float)i * 0.00005) ;
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

