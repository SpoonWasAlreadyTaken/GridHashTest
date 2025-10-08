#pragma once
#include <random>

std::random_device rd;

template <typename T>
T RandomNumber(T&& min, T&& max) // generates a random number based on input parameters
{
    std::uniform_int_distribution<> distr(min, max);
    return distr(rd);
}