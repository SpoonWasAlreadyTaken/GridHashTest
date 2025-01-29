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



int resolutionX = 800;
int resolutionY = 800;


// function declerations

template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max);
void Draw(sf::RenderWindow& window);
void FillGrid();

class Particle
{
public:
    int ID;
    int gridID;
    sf::Color color;

    template <typename P, typename I>
    Particle(P&& xy, P&& a, I&& i)
    {
        position = std::forward<P>(xy);
        lastPosition = position;

        acceleration = std::forward<P>(a);

        ID = std::forward<I>(i);

        color = sf::Color(RandomNumber(0, 255), RandomNumber(0, 255), RandomNumber(0, 255));
    }

    sf::Vector2f& GetPosition()
    {
        return position;
    }
    sf::Vector2f& GetLastPosition()
    {
        return lastPosition;
    }
    void SetVelocity(sf::Vector2f v)
    {
        lastPosition = position - v;
    }
    sf::Vector2f GetVelocity()
    {
        return position - lastPosition;
    }
    sf::Vector2f& GetAcceleration()
    {
        return acceleration;
    }


private:
    sf::Vector2f position;
    sf::Vector2f lastPosition;
    sf::Vector2f acceleration;
};



class SpatialHashing
{
public:
    int gridResolution;
    int numberOfGrids;
    int gridRowsX;
    int gridRowsY;
    std::unordered_map<int, std::vector<int>> grid;



    SpatialHashing(int r)
    {
        gridResolution = r * 2;

        numberOfGrids = resolutionX * resolutionY / gridResolution;
        gridRowsX = resolutionX / gridResolution;
        gridRowsY = resolutionY / gridResolution;

        grid.reserve(numberOfGrids);

        for (int i = 0; i < numberOfGrids; i++)
        {
            grid.emplace(i, NULL);
        }
    }

    void ClearGrid()
    {
        for (int i = 0; i < grid.size(); i++)
        {
            grid[i].empty();
        }
    }
};


SpatialHashing hasher = SpatialHashing(5);


class PhysicsSolver
{
public:

    std::vector<Particle> particles;
    int nextID;
    float particleSize = 5;
    int initialVelocityX = 2500;
    int initialVelocityY = 250;
    float DT = 0.001;
    float subDT;

    PhysicsSolver()
    {
        particles.reserve(1000);
        nextID = 0;

        padding = particleSize * 2;
        subDT = DT / (float)substeps;

        edgeXNormal = sf::Vector2f((-1 / sqrt(1 * 1)), (0 / sqrt(1*1)));
        edgeYNormal = sf::Vector2f((0 / sqrt(1 * 1)), (1 / sqrt(1 * 1)));

        initialVelocityX /= subDT;
        initialVelocityY /= subDT;
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
            particles.at(index).GetAcceleration() += gravity / subDT;
            for (int i = 0; i < substeps; i++)
            {
                sf::Vector2f v = particles.at(index).GetPosition() - particles.at(index).GetLastPosition();

                particles.at(index).GetLastPosition() = particles.at(index).GetPosition();

                particles.at(index).GetPosition() += v * drag + particles.at(index).GetAcceleration() * (subDT * subDT);

                particles.at(index).GetAcceleration() = {};

                Collision(index, particles.at(index).GetPosition());
                // border collision detection
                if (particles.at(index).GetPosition().x > resolutionX - padding || particles.at(index).GetPosition().x < 0 + padding)
                {
                    if (particles.at(index).GetPosition().x < 0)
                    {
                        particles.at(index).GetPosition().x = 0;
                        particles.at(index).SetVelocity(Bounce(-edgeXNormal, particles.at(index).GetVelocity()));
                    }
                    if (particles.at(index).GetPosition().x > resolutionX - padding)
                    {
                        particles.at(index).GetPosition().x = resolutionX - padding;
                        particles.at(index).SetVelocity(Bounce(edgeXNormal, particles.at(index).GetVelocity()));
                    }

                }

                if (particles.at(index).GetPosition().y > resolutionY - padding || particles.at(index).GetPosition().y < 0 + padding)
                {
                    if (particles.at(index).GetPosition().y < 0)
                    {
                        particles.at(index).GetPosition().y = 0;
                        particles.at(index).SetVelocity(Bounce(-edgeYNormal, particles.at(index).GetVelocity()));
                    }
                    if (particles.at(index).GetPosition().y > resolutionY - padding)
                    {
                        particles.at(index).GetPosition().y = resolutionY - padding;
                        particles.at(index).SetVelocity(Bounce(edgeYNormal, particles.at(index).GetVelocity()));
                    }
                }


                // inter particle collision detection
            }
        }
    }
    
private:

    int padding;

    sf::Vector2f gravity = sf::Vector2f(0.f, 9.81f);

    float absorbtion = 1;
    float drag = 1;

    int substeps = 8;

    sf::Vector2f edgeXNormal;
    sf::Vector2f edgeYNormal;

    sf::Vector2f GetForces()
    {
        return sf::Vector2f(0, 0);
    }

    template <typename N, typename V>
    sf::Vector2f Bounce(N&& n, V&& v)
    {
        return (- 2 * (v.x * n.x + v.y * n.y) * n + v) * absorbtion;
    }

    void Collision(int& index, sf::Vector2f& pos)
    {
        int gridPosition = particles.at(index).gridID;
        int point;
        for (int y = 0; y < 3; y++)
        {
            point = gridPosition - ((y - 1) * hasher.gridRowsX);
            if (point < hasher.numberOfGrids && point >= 0)
            {
                point -= 1;
                for (int x = 0; x < 3; x++)
                {
                    if (x < hasher.gridRowsX && x >= 0)
                    {
                        if (!hasher.grid[point].empty())
                        {
                            for (int i = 0; i < hasher.grid[point].size(); i++)
                            {
                                if (hasher.grid[point].at(i) != index)
                                {
                                    float distance = fabs(sqrt(pow((pos.x - particles.at(hasher.grid[point].at(i)).GetPosition().x), 2) + pow((pos.y - particles.at(hasher.grid[point].at(i)).GetPosition().y), 2)));

                                    if (distance < particleSize * 2)
                                    {
                                        float change = ((particleSize * 2) - distance) / 2;
                                        particles.at(index).GetPosition() += ((pos - particles.at(hasher.grid[point].at(i)).GetPosition())).normalized() * change * DT;
                                        particles.at(i).GetPosition() -= ((pos - particles.at(hasher.grid[point].at(i)).GetPosition())).normalized() * change * DT;
                                    }
                                }
                            }
                        }
                    }
                    point += 1;
                }
            }
        }
    }
};




// variable declerations
PhysicsSolver solver = PhysicsSolver();

int toSpawn = 50;
sf::CircleShape circle(1.0f);



int main()
{

    sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "Particles");

    sf::Clock clock;
    circle.setPointCount(32);
    float currentTime = 0;
    int fps;

    sf::Font virust_font;
    virust_font.openFromFile("C:\\Users\\PREDATOR\\Documents\\VisualStudioStuff\\Fonts\\VIRUST.ttf");

    sf::Text fpsText(virust_font, "60");
    sf::Text particleCountText(virust_font, std::to_string(solver.particles.size()));
    fpsText.setFillColor(sf::Color(0, 255, 0));
    particleCountText.setFillColor(sf::Color(255, 0, 0));
    fpsText.setPosition(sf::Vector2f(5, 0));
    particleCountText.setPosition(sf::Vector2f(5, 30));

    for (int i = 0; i < toSpawn; i++)
    {
        solver.AddParticle();
        particleCountText.setString(std::to_string(solver.particles.size()));
    }

    FillGrid();

    while (window.isOpen())
    {

        currentTime = clock.restart().asSeconds();
        fps = 1.f / (currentTime);

        fpsText.setString(std::to_string(fps));
;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();


        Draw(window);
        window.draw(fpsText);
        window.draw(particleCountText);

        window.display();
        solver.Update();
        FillGrid();


        system("CLS");
        for (int i = 0; i < solver.particles.size(); i++)
        {
            std::cout << "Particle ID:" << solver.particles.at(i).ID << " Particle gridID: " << solver.particles.at(i).gridID << "\n";
        }
    }
}


// functions

void Draw(sf::RenderWindow& window)
{
    for (int i = 0; i < solver.particles.size(); i++)
    {
        circle.setPosition(solver.particles.at(i).GetPosition());
        circle.setScale(sf::Vector2f(solver.particleSize, solver.particleSize));
        circle.setFillColor(solver.particles.at(i).color);
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

void FillGrid()
{
    hasher.ClearGrid();
    int gridPosition;

    for (int i = 0; i < solver.particles.size(); i++)
    {
        gridPosition = (int)(solver.particles.at(i).GetPosition().x / hasher.gridResolution) + (hasher.gridRowsX * ((int)(solver.particles.at(i).GetPosition().y / hasher.gridResolution)));
        if (gridPosition < 0)
        {
            gridPosition = 0;
        }
        else if (gridPosition >= hasher.numberOfGrids)
        {
            gridPosition = hasher.numberOfGrids - 1;
        }

        solver.particles.at(i).gridID = gridPosition;
        hasher.grid[gridPosition].push_back(i);
    }
}