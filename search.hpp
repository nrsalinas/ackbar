#include <iostream>
#include <vector>
#include <string>
#include <map>

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::map;

#ifndef SEARCH_HPP
#define SEARCH_HPP

// main search functions

vector< vector<Solution*> > metaAlt (vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmWeight);

vector< vector<Solution*> > dropSearchAlt (map<int, vector<int> > &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmWeight);

void solExpansionAlt (Solution* solita, map<int, double> &scoringGrid, map<int,int> &exclMap, int cellIndx);


// clustering functions

map<int, vector<int> > dbscan (vector<Mesh*> &observations, double eps);

void expand (vector<Mesh*> &observations, map<int, vector<int> > &clusters, map<int, int> &visited, int label, int border, double eps);


// scoring

map<int, double> presGrid (vector <Mesh*> &observations);

void complScore (Solution * rsearchSol, vector <Mesh*> &observations, double ndmWeight);


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
