#pragma once
#include <chrono>

#include "FaultyUtilitiesMT.hpp"
inline TaskSystem mt(20);

inline std::chrono::high_resolution_clock::time_point t1;
inline std::chrono::high_resolution_clock::time_point t2;
inline std::chrono::microseconds singleMS;


// variable decleration

uint32_t constexpr sizeX = 1920;
uint32_t constexpr sizeY = 1080;

inline float elapsedTime = 0;
inline float deltaTime = 0;

inline float constexpr particleSize = 2;

inline int constexpr toSpawn = 80000;

inline uint8_t ranR = 255; 
inline uint8_t ranG = 0;
inline uint8_t ranB = 80;
