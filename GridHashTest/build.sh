#!/bin/bash
g++ -c GridHashTest.cpp
g++ GridHashTest.o -o particles -lsfml-graphics -lsfml-window -lsfml-system
./particles
