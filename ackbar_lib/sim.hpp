#include <math.h>
#include "data.hpp"

#ifndef SIM_HPP
#define SIM_HPP

vector<Mesh*> nestedTiles (int size, int tilesxsize, int staProb = 90);

vector<Mesh*> overTiles (int size, int tilesxsize, int staProb = 90);

vector<Mesh*> overTiles2 (int size, int tilesxtype, int staProb = 90);

#endif