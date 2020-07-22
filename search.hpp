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

map<int, double> presGrid(vector <Mesh*> &observations);

vector< vector<Solution*> > dropSearchAlt(map<int, vector<int> > &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmWeight);

vector< vector<int> > solSets (vector <Solution*> &initSols, vector<int> &setMap);

void complScore(Solution * rsearchSol, vector <Mesh*> &observations, double ndmWeight);

void mergeSols(vector<Solution*> &population, int lower, int middle, int upper);

void sortSols(vector<Solution*> &population, int lower, int upper, string scoreType);

double kulczynski(Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);

bool equal (Mesh * meshA, Mesh * meshB);

void expand(vector<Mesh*> &observations, map<int, vector<int> > &clusters, map<int, int> &visited, int label, int border, double eps);

map<int, vector<int> > dbscan(vector<Mesh*> &observations, double eps);

vector< vector<Solution*> > metaAlt(vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmWeight);

int islandNumber(Solution * solita);

void islandCheck (Solution* solita, map <int, int> &checked, int cellIndx) ;

bool isContinuous (Solution* solita);

#endif
