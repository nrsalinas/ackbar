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

vector<Solution*> dropSearchAlt(map<int, vector<int> > &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmWeight);

void complScore(Solution * rsearchSol, vector <Mesh*> &observations, double ndmWeight);

void fitness(Solution * rsearchSol, vector <Mesh*> &observations, double outerFactor, double absFactor, double ndmWeight, bool updateAll);

void perturb(Solution * searchMesh, Mesh * meshTemplate, double hybrProb);

void perturbDiff(Solution * searchMesh);

void mergeSols(vector<Solution*> &population, int lower, int middle, int upper);

void sortSols(vector<Solution*> &population, int lower, int upper, string scoreType);

vector<Solution*> dropSearch(map<int, vector<int> > &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmOutFactor, double ndmWeight, double ndmAbsFactor);

void borderExpansion (Mesh * mechita, int cellIndx, map<int, int> &exclMap, vector<int> &island);

double kulczynski(Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);

bool equal (Mesh * meshA, Mesh * meshB);

vector<int> inter (Solution * solA, Solution * solB);

void expand(vector<Mesh*> &observations, map<int, vector<int> > &clusters, map<int, int> &visited, int label, int border, double eps);

map<int, vector<int> > dbscan(vector<Mesh*> &observations, double eps);

void solExpansion(Solution* solita, vector<Mesh*> &observations, map<int,int> &exclMap, int cellIndx, double prescore, double ndmOutFactor, double ndmAbsFactor, double ndmWeight);

vector<Solution*> exhSearch (vector<Mesh*> &observations, double ndmOutFactor, double ndmAbsFactor, double ndmWeight);

vector<Solution*> meta(vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmOutFactor, double ndmAbsFactor, double ndmWeight);

vector<Solution*> metaAlt(vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmWeight);

int islandNumber(Solution * solita);

void islandCheck (Solution* solita, map <int, int> &checked, int cellIndx) ;

bool isContinuous (Solution* solita);

#endif
