#include "search4py.hpp"

void fitness(SolutionB * rsearchSol, vector <Mesh*> &observations, double overPenalty,
	bool updateAll){
	rsearchSol->score = 0;
	rsearchSol->critA = 0;
	rsearchSol->critB = 0;
	rsearchSol->ndmScore = 0.0;
	rsearchSol->extent = 0;

	double outerFactor = 0.25;

	if (updateAll){
		rsearchSol->spp2crit.clear();
		}

	for(int i = 0; i < observations.size(); i++){
		double innerPresences = 0;
		double outerPresences = 0;
		double popIncluded = 0.0;
		double thisScore = 0.0;
		string status = observations[i]->getThreatStatus();
		vector <int> subcritA = observations[i]->getThreatSubcriteriaA();
		bool properA = false;
		int suppA = 0;
		int suppB = 0;

		for (int a = 0; a < subcritA.size(); a++){
			if ((subcritA[a] == 1) || (subcritA[a] == 2) || (subcritA[a] == 4)) {
				properA = true;
				break;
				}
			}

		for(int c = 0; c < rsearchSol->getSize(); c++){
			if (rsearchSol->getValue(c) > 0){
				rsearchSol->extent += 1;
				}
			}

		for(int c = 0; c < rsearchSol->getSize(); c++){
			if (rsearchSol->getValue(c) > 0){
				popIncluded += observations[i]->getValue(c);
				if (observations[i]->getValue(c) > 0) {
					innerPresences += 1;
					}
				} else {
					if (observations[i]->getValue(c) > 0) {
						outerPresences += 1;
						}
					}
			}

		rsearchSol->ndmScore += (innerPresences / ((double)rsearchSol->extent + outerPresences * outerFactor));

		if ((status == "CR") || (status == "EN")) {
			if (popIncluded >= 0.005){
				//rsearchSol->critA += 1;
				suppA = 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(0);
					}
				}
			if ((popIncluded >= 0.01) & (properA)) {
				//rsearchSol->critA += 1;
				suppA = 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(2);
					}
				}
			if (popIncluded >= 0.95) {
				//rsearchSol->critA += 1;
				suppA = 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(4);
					}
				}


			} else if (status == "VU") {
				if (popIncluded >= 0.01){
					//rsearchSol->critA += 1;
					suppA = 1;
					if (updateAll){
						rsearchSol->spp2crit[i].push_back(1);
						}
					}
				if ((popIncluded >= 0.02) & (properA)) {
					//rsearchSol->critA += 1;
					suppA = 1;
					if (updateAll){
						rsearchSol->spp2crit[i].push_back(3);
						}
					}
				}


		if (popIncluded >= 0.1) {
			//rsearchSol->critB += 1;
			suppB = 1;
			if (updateAll){
				rsearchSol->spp2crit[i].push_back(5);
				}
			}

		if (popIncluded >= 0.01) {
			//rsearchSol->critB += 1;
			suppB = 1;
			if (updateAll){
				rsearchSol->spp2crit[i].push_back(6);
				}
			}

		rsearchSol->critA += suppA;
		rsearchSol->critB += suppB;
		}

	rsearchSol->score = rsearchSol->critA + rsearchSol->critB;
	}


void perturb(SolutionB * searchMesh, Mesh * meshTemplate, double hybrProb) {
	int threshold = (int) (hybrProb * 100.0);
	for (int i = 0 ; i < searchMesh->getSize(); i++) {
		if (rand() % 100 < threshold){
			if (meshTemplate->getValue(i) > 0.0){
				searchMesh->setValue(i, 1.0);
				} else {
					searchMesh->setValue(i, 0.0);
					}
			}
		}
	}


void perturbDiff(SolutionB * searchMesh, double variance) {
	int threshold = (int) (variance * 100.0);
	for (int i = 0; i < searchMesh->getSize(); i++){
		if (rand() % 100 < threshold){
			vector <int> neighs;
			neighs = searchMesh->getCellNeighs(i);
			for (int n = 0; n < neighs.size(); n++){
				if (searchMesh->getValue(i) != searchMesh->getValue(neighs[n])) {
					searchMesh->setValue(neighs[n], searchMesh->getValue(i));
					}
				}
			}
		}
	}

void mergeSols(vector<SolutionB*> &population, int lower, int middle, int upper){
	vector<SolutionB*> temp;
	int lowerish = lower;
	int middlish = middle + 1;

	while ((middlish <= upper) && (lowerish <= middle)) {
		if(population[lowerish]->score >= population[middlish]->score) {
			temp.push_back(population[lowerish]);
			lowerish++;
			} else {
				temp.push_back(population[middlish]);
				middlish++;
				}
		}

	while (middlish <= upper) {
		temp.push_back(population[middlish]);
		middlish++;
		}

	while (lowerish <= middle) {
		temp.push_back(population[lowerish]);
		lowerish++;
		}

	for (int i = lower; i <= upper; i++) {
		population[i] = temp[i-lower];
		}

	}

void sortSols(vector<SolutionB*> &population, int lower, int upper){
	if (lower < upper) {
		int middle = ((lower + upper) / 2);
		sortSols(population, lower, middle);
		sortSols(population, (middle + 1), upper);
		mergeSols(population, lower, middle, upper);
		}
	}


vector<SolutionB*> dropSearch(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize){
	//cout << "In dropSearch\n";
	int clusNum = -1;
	int thisClus;
	int thisObs;
	int thisCell;
	bool breakOuter;
	map<int, vector<int>>::iterator it;
	map<int, int> exclMap;
	vector<int> island;
	vector<SolutionB*> mySols;

	for (it = clusters.begin(); it != clusters.end(); it++) {
		if (clusNum < it->first){
			clusNum = it->first;
			}
		}
	clusNum += 1;

	for (int i = 0 ; i < observations[0]->getSize(); i++) {
		exclMap[i] = 0;
		}


	for (int t = 0; t < iters; t++) {
		breakOuter = false;
		thisClus = rand() % clusNum;
		thisObs = rand() % clusters[thisClus].size();
		thisObs = clusters[thisClus][thisObs];
		thisCell = rand() % observations[thisObs]->getSize();

		while (observations[thisObs]->getValue(thisCell) <= 0) {
			thisCell = rand() % observations[thisObs]->getSize();
			}

		exclMap[thisCell] = 1;
		island.push_back(thisCell);

		borderExpansion(observations[thisObs], thisCell, exclMap, island);

		SolutionB * aSol = new SolutionB(observations[thisObs]->getSize());
		aSol->neighsFromList(observations[thisObs]->getNeighborhood());

		for (int i = 0; i < aSol->getSize(); i++) {
			aSol->setValue(i, 0);
			}

		for (int i = 0; i < island.size(); i++) {
			aSol->setValue(island[i], 1);
			}

		fitness(aSol, observations, 0.3, false);

	/*	Assess competitors to aSol already in mySols
	Don't add to mySols Solution with the exact same
	shape is in the vector                            */

		for (int i = 0; i < mySols.size(); i++){
			if (equal(aSol, mySols[i])) {
				breakOuter = true;
				break;
				}
			}

		if(breakOuter) {
			continue;
			}

		mySols.push_back(aSol);
		//cout << "mySols now has " << mySols.size() << " elements\n";
		}

	/*##############################################
 	Get intersection and complements of solutions that partially overlap.
 	However, intersections and complements could be not continuous!
	Thus, move this code to the steady state perturbation routine

	bool addme = false;
	for (int i = 0; i < mySols.size(); i++) {
		for (int j = i + 1; j < mySols.size(); j++) {
			if (overlap(mySols[i], mySols[j])) {
				// interCompl
				vector <SolutionB *> newCand = interCompl(mySols[i], mySols[j]);
				for (int c = 0; c < newCand.size(); c++) {
					addme = false;

					for (int k = 0; k < mySols.size(); k++) {
						if (equal(newCand[c], mySols[k])) {
							break;
							} else {
								addme = true;
								}
						}
					if (addme) {
						fitness(newCand[c], observations, 0.3);
						mySols.push_back(newCand[c]);
						} else {
							delete newCand[c];
							}

					}
				}
			}
		} */

	sortSols(mySols, 0, (mySols.size() - 1));

	if (mySols.size() > outSize) {
		for (int i = outSize; i < mySols.size(); i++) {
			delete mySols[i];
			//cout << "Eliminating an element in mySols, now has " << mySols.size() << " elements" << endl;
			}
		mySols.resize(outSize);
		//cout << "Resizing mySols to " << outSize << endl;
		}

	//cout << "returning dropSearch" << endl;
	return mySols;
	}


void borderExpansion (Mesh * mechita, int cellIndx, map<int, int> &exclMap, vector<int> &island) {
	vector<int> thisNeighs = mechita->getCellNeighs(cellIndx);

	for (int n = 0; n < thisNeighs.size(); n++) {
		if ((mechita->getValue(thisNeighs[n]) > 0) & (exclMap[thisNeighs[n]] == 0)){
			exclMap[thisNeighs[n]] = 1;
			island.push_back(thisNeighs[n]);
			borderExpansion(mechita, thisNeighs[n], exclMap, island);
			}
		}

	}

double kulczynski(Mesh * meshA, Mesh * meshB){
	double dis = 0.0;
	double common = 0.0;
	double occupA = 0.0;
	double occupB = 0.0;
	for (int i = 0; i < meshA->getSize(); i++){
		if (meshA->getValue(i) > 0){
			occupA += 1.0;
			if (meshB->getValue(i) > 0) {
				common += 1.0;
				}
			}
		if (meshB->getValue(i) > 0){
			occupB += 1.0;
			}
		}
	//cout << "common: " << common << ", occupA: " << occupA << ", occupB: " << occupB << endl;
	dis = 1.0 - (((common / occupA) + (common / occupB)) / 2.0);
	return dis;
	}


bool overlap (Mesh * meshA, Mesh * meshB) {
	bool out = false;
	for (int i = 0; i < meshA->getSize(); i++){
		if ((meshA->getValue(i) > 0) && (meshB->getValue(i) > 0)){\
			out = true;
			break;
			}
		}
	return out;
	}


bool equal (Mesh * meshA, Mesh * meshB) {
	//cout << "In equal function" << endl;
	bool out = true;
	for (int i = 0; i < meshA->getSize(); i++){
		//cout << meshA->getValue(i) << ", "<< meshB->getValue(i) << endl;
		if (((meshA->getValue(i) > 0) && (meshB->getValue(i) == 0)) ||
			((meshA->getValue(i) == 0) && (meshB->getValue(i) > 0))) {
			//cout << "Index " << i << endl;
			out = false;
			break;
			}
		}
	//cout << "Leaving equal function." << endl;
	return out;
	}


/*###################################################################
#
# interCompl has to return continuous solutions
#
#####################################################################*/
vector<SolutionB*> interCompl (SolutionB * solA, SolutionB * solB) {
//	cout << "In interCompl function. Initial Solution objects:" << endl;
	vector<SolutionB*> out;
// a = A not B, b = B not A, ab = A and B
	SolutionB * a = new SolutionB(solA->getSize());
	a->neighsFromList(solA->getNeighborhood());
	SolutionB * b = new SolutionB(solA->getSize());
	b->neighsFromList(solA->getNeighborhood());
	SolutionB * ab = new SolutionB(solA->getSize());
	ab->neighsFromList(solA->getNeighborhood());
	//	a->prettyPrint();
	//	b->prettyPrint();
	//	ab->prettyPrint();



	for (int i = 0; i < solA->getSize(); i++){
		if (solA->getValue(i) > 0){
			if (solB->getValue(i) > 0) {
				ab->setValue(i, 1);
				} else {
					a->setValue(i, 1);
					}
			} else if (solB->getValue(i) > 0) {
				b->setValue(i, 1);
				}
		}

	//	cout << "\nSolution objects prefiltering:" << endl;
	//	a->prettyPrint();
	//	b->prettyPrint();
	//	ab->prettyPrint();

	if (a->isNull() || equal(a, solA)) {
	//		cout << "Deleting a." << endl;
		delete a;
		} else {
			out.push_back(a);
			}

	if (b->isNull() || equal(b, solB)) {
	//		cout << "Deleting b." << endl;
		} else {
			out.push_back(b);
			}

	if (ab->isNull()) {
	//		cout << "Deleting ab." << endl;
		} else {
			out.push_back(ab);
			}

	//		cout << "Leaving interCompl function." << endl;

	return out;
	}


void expand(vector<Mesh*> &observations, map<int, vector<int>> &clusters, map<int, int> &visited, int label, int border, double eps) {

	for (int i = 0; i < observations.size(); i++) {

		if ((visited[i] == 0) && (i != border)) {

			if (eps > kulczynski(observations[border], observations[i])) {

				clusters[label].push_back(i);
				visited[i] = 1;
				expand(observations, clusters, visited, label, i, eps);

				}

			}

		}

	}

map<int, vector<int>> dbscan(vector<Mesh*> &observations, double eps){

	map<int, vector<int>> out;
	map<int, int> visited;
	int label = -1;

	for (int i = 0; i < observations.size(); i++) {
		visited[i] = 0;
		}

	for (int i = 0; i < observations.size(); i++) {

		if (visited[i] == 0) {

			label += 1;
			out[label].push_back(i);
			visited[i] = 1;
			expand(observations, out, visited, label, i, eps);

			}
		}

	return out;

	}
