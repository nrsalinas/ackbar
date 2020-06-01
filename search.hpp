void perturb(SolutionB * searchMesh, Mesh * meshTemplate, float hybrProb);

void perturbDiff(SolutionB * searchMesh, float variance);

void mergeSols(vector<SolutionB*> &population, int lower, int middle, int upper);

void sortSols(vector<SolutionB*> &population, int lower, int upper);
/*
//vector<Solution> optim(vector<Mesh*> observations, int iters, float hybrProb,
//	int lambda, int mu);
*/
void fitness(SolutionB * rsearchSol, vector <Mesh *> &observations, double overPenalty,
	bool updateAll = false);

vector<SolutionB*> dropSearch(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize);

void borderExpansion (Mesh * mechita, int cellIndx, map<int, int> &exclMap, vector<int> &island);

float kulczynski(Mesh * meshA, Mesh * meshB);

bool overlap (Mesh * meshA, Mesh * meshB);

bool equal (Mesh * meshA, Mesh * meshB);

vector<SolutionB*> interCompl (SolutionB * solA, SolutionB * solB);