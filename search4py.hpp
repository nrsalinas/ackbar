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

#ifndef SEARCH4PY_HPP
#define SEARCH4PY_HPP

void fitness(SolutionB * rsearchSol, vector <Mesh*> &observations, double outerFactor,
	double absFactor, bool updateAll);

void perturb(SolutionB * searchMesh, Mesh * meshTemplate, double hybrProb);

void perturbDiff(SolutionB * searchMesh);

void mergeSols(vector<SolutionB*> &population, int lower, int middle, int upper);

void sortSols(vector<SolutionB*> &population, int lower, int upper, string scoreType);

vector<SolutionB*> dropSearch(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmOutFactor, double ndmAbsFactor);

void borderExpansion (Mesh * mechita, int cellIndx, map<int, int> &exclMap, vector<int> &island);

double kulczynski(Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);

bool equal (Mesh * meshA, Mesh * meshB);

vector<int> inter (SolutionB * solA, SolutionB * solB);

void expand(vector<Mesh*> &observations, map<int, vector<int>> &clusters, map<int, int> &visited, int label, int border, double eps);

map<int, vector<int>> dbscan(vector<Mesh*> &observations, double eps);

void solExpansion(SolutionB* solita, vector<Mesh*> &observations, map<int,int> &exclMap, int cellIndx, double prescore, double ndmOutFactor, double ndmAbsFactor);

vector<SolutionB*> exhSearch (vector<Mesh*> &observations, double ndmOutFactor, double ndmAbsFactor);

#endif
