#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>



int resolutionX = 800;
int resolutionY = 800;


// function declerations

template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max);
void Draw(sf::RenderWindow& window);

class Particle
{
public:
    int ID;

    template <typename P, typename I>
    Particle(P&& xy, P&& vXvY, I&& i)
    {
        position = std::forward<P>(xy);

        velocity = std::forward<P>(vXvY);

        ID = std::forward<I>(i);

    }

    sf::Vector2f& GetPosition()
    {
        return position;
    }
    sf::Vector2f& GetVelocity()
    {
        return velocity;
    }


private:
    sf::Vector2f position;
    sf::Vector2f velocity;
};

class PhysicsSolver
{
public:

    std::vector<Particle> particles;
    int nextID;
    float particleSize = 3;
    int initialVelocityX = 25;
    int initialVelocityY = 25;

    PhysicsSolver()
    {
        particles.reserve(500);
        nextID = 0;

        padding = particleSize * 2;
    }



    void AddParticle()
    {
        particles.emplace_back(sf::Vector2f(RandomNumber(0, resolutionX), RandomNumber(0, resolutionY)), sf::Vector2f(RandomNumber(- initialVelocityX, initialVelocityX), RandomNumber(0, initialVelocityY)), nextID);
        nextID++;
    }

    void Update()
    {

        for (int index = 0; index < particles.size(); index++)
        {
            for (int i = 0; i < substeps; i++)
            {
                particles.at(index).GetPosition() += sf::Vector2f(particles.at(index).GetVelocity().x / substeps, particles.at(index).GetVelocity().y / substeps);

                particles.at(index).GetVelocity() += sf::Vector2f(gravity.x / substeps, gravity.y / substeps);

                if (particles.at(index).GetPosition().x > resolutionX - padding || particles.at(index).GetPosition().x < 0 + padding)
                {
                    particles.at(index).GetVelocity().x *= -friction;
                }
                if (particles.at(index).GetPosition().y > resolutionY - padding || particles.at(index).GetPosition().y < 0 + padding)
                {
                    particles.at(index).GetVelocity().y *= -friction;
                }
            }
        }


    }
    
private:

    int padding;

    sf::Vector2f gravity = sf::Vector2f(0 / 2, 9.8 / 2);

    float friction = 0.65;

    int substeps = 1;
};




// variable declerations
PhysicsSolver solver;
int toSpawn = 500;






int main()
{

    for (int i = 0; i < toSpawn; i++)
    {
        solver.AddParticle();
    }

    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Particles");

    Draw(window);


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();


        Draw(window);


        window.display();

        solver.Update();
    }
}


// functions

void Draw(sf::RenderWindow& window)
{
    sf::CircleShape circle(1.0f);
    for (int i = 0; i < solver.particles.size(); i++)
    {
        circle.setPosition(solver.particles.at(i).GetPosition());
        circle.setScale(sf::Vector2f(solver.particleSize, solver.particleSize));
        circle.setFillColor(sf::Color(255, 0, 255));
        window.draw(circle);
    }
}

template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max) // generates a random number based on input parameters
{
    std::random_device rd;
    std::uniform_int_distribution<> distr(min, max);
    return distr(rd);
}
