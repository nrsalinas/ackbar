/*#############################################################################
#
#	Copyright 2020 Nelson R. Salinas
#
#
#	This file is part of Ackbar.
#
#   Ackbar is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
# 	(at your option) any later version.
#
#	Ackbar is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with Ackbar. If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "data.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::map;

#ifndef SEARCH_HPP
#define SEARCH_HPP

typedef unsigned int uint;

// main search functions

vector< vector<Solution*> > metaAlt (vector<Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, double clusterEps, int iters, int outSize, double ndmWeight);

vector< vector<Solution*> > dropSearchAlt (map<int, vector<int> > &clusters, vector <Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, int iters, int outSize, double ndmWeight);

void solExpansionAlt (Solution* solita, map<int, double> &scoringGrid, map<int,int> &exclMap, int cellIndx);


// clustering functions

map<int, vector<int> > dbscan (vector<Mesh*> &observations, double eps);

void expand (vector<Mesh*> &observations, map<int, vector<int> > &clusters, map<int, int> &visited, int label, uint border, double eps);


// scoring

map<int, double> presGrid (vector <Mesh*> &observations);

void complScore (Solution * rsearchSol, vector <Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, double ndmWeight);


// solution sorting

vector< vector<int> > solSets (vector <Solution*> &initSols, vector<int> &setMap);

void sortSols (vector<Solution*> &population, int lower, int upper, string scoreType);

void mergeSols (vector<Solution*> &population, int lower, int middle, int upper, string scoreType);


// operations between two Mesh*

bool equal (Mesh * meshA, Mesh * meshB);

double kulczynski (Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);


// operetions within a Solution*

bool isContinuous (Solution* solita);

int islandNumber (Solution * solita);

void islandCheck (Solution* solita, map <int, int> &checked, int cellIndx) ;

#endif
