#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

#ifndef SEARCH4PY_HPP
#define SEARCH4PY_HPP

void fitness(SolutionB * rsearchSol, vector <Mesh*> &observations, double overPenalty,
	bool updateAll);

void perturb(SolutionB * searchMesh, Mesh * meshTemplate, double hybrProb);

void perturbDiff(SolutionB * searchMesh, double variance);

void mergeSols(vector<SolutionB*> &population, int lower, int middle, int upper);

void sortSols(vector<SolutionB*> &population, int lower, int upper);

vector<SolutionB*> dropSearch(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize);

void borderExpansion (Mesh * mechita, int cellIndx, map<int, int> &exclMap, vector<int> &island);

double kulczynski(Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);

bool equal (Mesh * meshA, Mesh * meshB);

vector<SolutionB*> interCompl (SolutionB * solA, SolutionB * solB);

#endif