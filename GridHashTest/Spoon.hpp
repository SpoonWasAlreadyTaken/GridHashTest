#pragma once
#include <random>

template <typename T, typename Y>
int RandomNumber(T&& min, Y&& max) // generates a random number based on input parameters
{
    std::random_device rd;
    std::uniform_int_distribution<> distr(min, max);
    return distr(rd);
}