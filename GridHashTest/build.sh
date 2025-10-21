#!/bin/bash
g++ -c GridHashTest.cpp
g++ GridHashTest.o -o3 particles -lsfml-graphics -lsfml-window -lsfml-system
./particles
