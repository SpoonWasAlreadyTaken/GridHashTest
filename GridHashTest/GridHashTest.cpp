#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <thread>
#include <conio.h>

using namespace std;

// ahh

int const boundX = 101;
int const boundY = 101;

char grid[boundY][boundX];

float friction = 0.9;



class Particle
{
public:

    int posX;
    int posY;

    int posXLast;
    int posYLast;

    float velocityX;
    float velocityY;

    template <typename T, typename Y>
    Particle(T&& x, T&& y, Y&& xV, Y&& yV)
    {
        posX = x;
        posY = y;

        velocityX = xV;
        velocityY = yV;

        posXLast = posX;
        posYLast = posY;
    }

    void UpdatePosition()
    {
        posXLast = posX;
        posYLast = posY;

        posX += velocityX;
        posY += velocityY;

        if (posX >= boundX)
        {
            posX = boundX - 1;
            velocityX *= -friction;
        }
        if (posX < 0)
        {
            posX = 0;
            velocityX *= -friction;
        }
        if (posY >= boundY)
        {
            posY = boundY - 1;
            velocityY *= -friction;
        }
        if (posY < 0)
        {
            posY = 0;
            velocityY *= -friction;
        }

        Gravity();
    }

    

private:
    void Gravity()
    {
        velocityY += 2.5;

        if (velocityY > 10)
        {
            velocityY = 10;
        }
    }

};


// function declerations
template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max);
void Draw();
void UpdateDraw();
void SpawnParticle();
void UpdateParticles();


// global variable declerations

bool gameRunning = true;

std::vector<Particle> particles;



// main function


int main()
{

    for (int i = 0; i < boundY; i++)
    {
        for (int j = 0; j < boundX; j++)
        {
            grid[i][j] = ' ';
        }
    }

    for (int i = 0; i < RandomNumber(3, 15); i++)
    {
        SpawnParticle();
    }

    int t = 0;

    while (gameRunning)
    {

        t++;

        UpdateParticles();

        Draw();

        if (t == 1)
        {
            t = 0;
            SpawnParticle();
        }

        /*
        if (_kbhit)
        {
            if (_getch() == 32)
            {
                SpawnParticle();
            }
        }
        */

        /*
        for (int i = 0; i < particles.size(); i++)
        {
            cout << "ID: " << i << "particle posX: " << particles.at(i).posX << "\n";
            cout << "ID: " << i << "particle posY: " << particles.at(i).posY << "\n";
            cout << "ID: " << i << "particle last posX: " << particles.at(i).posXLast << "\n";
            cout << "ID: " << i << "particle last posY: " << particles.at(i).posYLast << "\n";
        }
        Sleep(2000);
        system("CLS");
        */

    }
}



// functions
void Draw()
{
    UpdateDraw();
    system("CLS");

    for (int i = 0; i < boundY; i++)
    {
        for (int j = 0; j < boundX; j++)
        {
            std::cout << " " << grid[i][j];
        }

        std::cout << "\n";
    }

    Sleep(150);       
}

void UpdateDraw()
{
    for (int i = 0; i < particles.size(); i++)
    {
        grid[particles.at(i).posYLast][particles.at(i).posXLast] = ' ';
        grid[particles.at(i).posY][particles.at(i).posX] = (char)i + 32;
    }
}

void UpdateParticles()
{
    for (int i = 0; i < particles.size(); i++)
    {
        particles.at(i).UpdatePosition();
        
        for (int j = 0; j < particles.size(); j++)
        {
            if (i != j)
            {
                if (particles.at(i).posX == particles.at(j).posX && particles.at(i).posY == particles.at(j).posY)
                {
                    particles.at(i).velocityX *= -1;
                    particles.at(i).velocityY *= -1;
                }
            }
        }
    }
}

template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max) // generates a random number based on input parameters
{
    random_device rd;
    uniform_int_distribution<> distr(min, max);
    return distr(rd);
}

void SpawnParticle()
{
    particles.emplace_back(RandomNumber(0, boundX -1), RandomNumber(0, boundY -1), RandomNumber(-5, 5), -10);
}

