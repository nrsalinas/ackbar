#include "search4py.hpp"


map<int, double> presGrid(vector <Mesh*> &observations){
	map<int, double> out;

	for (int c = 0; c < observations[0]->getSize(); c++){
		out[c] = 0.0;
		for (int i = 0; i < observations.size(); i++){
			out[c] += observations[i]->getValue(c);
		}
	}
	return out;
}


void solExpansionAlt (SolutionB* solita, map<int, double> &scoringGrid, map<int,int> &exclMap, int cellIndx) {

	vector<int> thisNeighs = solita->getCellNeighs(cellIndx);
	double prescore, postscore;

	for (int n = 0; n < thisNeighs.size(); n++) {
		//cout << "exclMap[thisNeighs[n]] : " << exclMap[thisNeighs[n]] << endl;
		if (exclMap[thisNeighs[n]] == 0){

			postscore = (solita->ndmScore * (double) solita->extent + scoringGrid[thisNeighs[n]]) / ((double) solita->extent + 1.0);

			if (postscore > solita->ndmScore) { 

				solita->setValue(thisNeighs[n], 1);
				solita->ndmScore = postscore;
				solita->extent += 1;
				exclMap[thisNeighs[n]] = 1;
				solExpansionAlt(solita, scoringGrid, exclMap, thisNeighs[n]);

			}
		}
	}
}


vector<SolutionB*> dropSearchAlt(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmWeight){
	//cout << "In dropSearch\n";
	int clusNum = -1;
	int thisClus;
	int thisObs;
	int thisCell;
	bool breakOuter;
	double initscore;
	map<int, vector<int>>::iterator it;
	map<int, int> exclMap;
	//vector<int> island;
	vector<SolutionB*> mySols;
	map<int, double> scoreGrid;

	for (it = clusters.begin(); it != clusters.end(); it++) {
		if (clusNum < it->first){
			clusNum = it->first;
		}
	}
	clusNum += 1;

	scoreGrid = presGrid(observations);


	for (int t = 0; t < iters; t++) {
		breakOuter = false;
		thisClus = rand() % clusNum;
		thisObs = rand() % clusters[thisClus].size();
		thisObs = clusters[thisClus][thisObs];
		thisCell = rand() % observations[thisObs]->getSize();

		while (observations[thisObs]->getValue(thisCell) <= 0) {
			thisCell = rand() % observations[thisObs]->getSize();
		}

		SolutionB * asol = new SolutionB(observations[thisObs]);
		asol->nullMe();
		asol->setValue(thisCell, 1);
		asol->extent = 1;

		asol->ndmScore = scoreGrid[thisCell];
		
		for (int j = 0; j < observations[0]->getSize(); j++) {
			exclMap[j] = 0;
		}
		exclMap[thisCell] = 1;

		for (int k = 0; k < observations.size(); k++) {
			asol->sppAreas[k] = observations[k]->getValue(thisCell);
		}

		solExpansionAlt(asol, scoreGrid, exclMap, thisCell);

		for (int i = 0; i < mySols.size(); i++){
			if (equal(asol, mySols[i])) {
				breakOuter = true;
				delete asol;
				break;
			}
		}

		if (breakOuter) {
			continue;
		} else {
			mySols.push_back(asol);
		}

	}

	/* Create new Solutions using the starting seed
	Come up with some new areas from the intersection zones of Solutions in the
	seed.
	*/

	/* Include an overlapping threshold to filter out solutions too 
	similar */
	
	int vecSi = mySols.size();
	for (int i = 0; i < vecSi; i++) {
		for (int j = i+1; j < vecSi; j++) {
			if ( overlap(mySols[i], mySols[j]) ) {
			
				SolutionB * comp0 = new SolutionB(mySols[j]);
				SolutionB * comp1 = new SolutionB(mySols[i]);
				SolutionB * inter = new SolutionB(mySols[i]);
				inter->nullMe();

				for (int c = 0; c < mySols[i]->getSize(); c++) {

					if ((mySols[i]->getValue(c) > 0) && (mySols[j]->getValue(c) > 0)) {
						comp0->setValue(c, 0);
						comp1->setValue(c, 0);
						inter->setValue(c, 1);
					}
				}
				
				/***********************************
				*  Check if new areas are continuous
				************************************/
				
				bool del0 = false;
				bool del1 = false;
				bool deli = false;

				if (comp0->isNull()) {
					delete comp0;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp0, mySols[m]) ) {
							delete comp0;
							del0 = true;
							break;
						} 
					}
					
					if (!del0) {
						//fitness(comp0, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
						mySols.push_back(comp0);
					}
				}

				if (comp1->isNull()) {
					delete comp1;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp1, mySols[m]) ) {
							delete comp1;
							del1 = true;
							break;
						} 
					}
					
					if (!del1) {
						mySols.push_back(comp1);
					}
				}

				if (inter->isNull()) {
					delete inter;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(inter, mySols[m]) ) {
							delete inter;
							deli = true;
							break;
						} 
					}
					
					if (!deli) {
						mySols.push_back(inter);
					}
				}

			}
		}
	}
	
	sortSols(mySols, 0, (mySols.size() - 1), "ndm");

	if (mySols.size() > outSize) {
		for (int i = outSize; i < mySols.size(); i++) {
			delete mySols[i];
		}
		mySols.resize(outSize);
	}

	for (int i = 0; i < mySols.size(); i++) {
		complScore(mySols[i], observations, ndmWeight);
	}

	return mySols;
}


void complScore(SolutionB * rsearchSol, vector <Mesh*> &observations, double ndmWeight) {
	rsearchSol->score = 0;
	rsearchSol->critA = 0;
	rsearchSol->critB = 0;
	rsearchSol->spp2crit.clear();

	for(int i = 0; i < observations.size(); i++){
		string status = observations[i]->getThreatStatus();
		vector <int> subcritA = observations[i]->getThreatSubcriteriaA();
		bool properA = false;
		int suppA = 0;
		int suppB = 0;
		double popIncluded = 0.0;

		for(int c = 0; c < rsearchSol->getSize(); c++){
			if (rsearchSol->getValue(c) > 0){
				popIncluded += observations[i]->getValue(c);
			}
		}

		for (int a = 0; a < subcritA.size(); a++){
			if ((subcritA[a] == 1) || (subcritA[a] == 2) || (subcritA[a] == 4)) {
				properA = true;
				break;
			}
		}

		if ((status == "CR") || (status == "EN")) {

			if (popIncluded >= 0.005){
				suppA = 1;
				rsearchSol->spp2crit[i].push_back(0);
			}
		
			if ((popIncluded >= 0.01) & (properA)) {
				suppA = 1;
				rsearchSol->spp2crit[i].push_back(2);
			}
			
			if (popIncluded >= 0.95) {
				suppA = 1;
				rsearchSol->spp2crit[i].push_back(4);
			}

		} else if (status == "VU") {
			
			if (popIncluded >= 0.01){
				suppA = 1;
				rsearchSol->spp2crit[i].push_back(1);
			}

			if ((popIncluded >= 0.02) & (properA)) {
				suppA = 1;
				rsearchSol->spp2crit[i].push_back(3);
			}
		}

		if (popIncluded >= 0.1) {
			suppB = 1;
			rsearchSol->spp2crit[i].push_back(5);
		}

		if (popIncluded >= 0.01) {
			suppB = 1;
			rsearchSol->spp2crit[i].push_back(6);
		}

		rsearchSol->critA += suppA;
		rsearchSol->critB += suppB;

	}

	rsearchSol->score = rsearchSol->critA + rsearchSol->critB;
	rsearchSol->aggrScore = (double) rsearchSol->score + ndmWeight * rsearchSol->ndmScore;

}


void fitness(SolutionB * rsearchSol, vector <Mesh*> &observations, double outerFactor, 
	double absFactor, double ndmWeight, bool updateAll){
	map<int, int> noSupp;
	double noObsCells = 0.0;
	rsearchSol->score = 0;
	rsearchSol->critA = 0;
	rsearchSol->critB = 0;
	rsearchSol->ndmScore = 0.0;
	rsearchSol->extent = 0;

	if (updateAll){
		rsearchSol->spp2crit.clear();
	}

	for(int c = 0; c < rsearchSol->getSize(); c++){
		if (rsearchSol->getValue(c) > 0){
			rsearchSol->extent += 1;
			noSupp[c] = 1;
		}
	}


	for(int i = 0; i < observations.size(); i++){
		double innerPresences = 0;
		double outerPresences = 0;
		double popIncluded = 0.0;
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
				popIncluded += observations[i]->getValue(c);
				if (observations[i]->getValue(c) > 0) {
					innerPresences += 1.0;
					noSupp[c] = 0;
				}
			} else {
				if (observations[i]->getValue(c) > 0) {
					outerPresences += 1.0;
				}
			}
		}

		if (outerFactor <= 0) {
			outerFactor = 0.000001;
		}
		rsearchSol->ndmScore += (innerPresences / ((double)rsearchSol->extent + outerPresences * outerFactor));
		//rsearchSol->ndmScore += (innerPresences / ((double)rsearchSol->extent + outerPresences / outerFactor));

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

	for (int c = 0; c < rsearchSol->getSize(); c++) {
		if (noSupp[c] == 1) {
			noObsCells += 1.0;
		}
	}


	rsearchSol->score = rsearchSol->critA + rsearchSol->critB;
	rsearchSol->ndmScore /= (1.0 + noObsCells * absFactor);
	rsearchSol->aggrScore = (double) rsearchSol->score + rsearchSol->ndmScore * ndmWeight;
	
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


void perturbDiff(SolutionB * searchMesh) {
	/* Works only on 1-degree neighborhoods
	threshold should be 0-99 */
	vector <int> neighs;
	bool changed = false;
	int index, nindex;
	double val, nval;

	while (!changed) {

		index = rand() % searchMesh->getSize();
		val = searchMesh->getValue(index);
		neighs = searchMesh->getCellNeighs(index);
		nindex = rand() % neighs.size();
		nval = searchMesh->getValue(nindex);
		
		if ((val == 0) && (nval > 0)) {
			searchMesh->setValue(nindex, 0);
			changed = true;

		} else if ((val > 0) && (nval == 0)) {
			searchMesh->setValue(nindex, 1);
			changed = true;	
		}
	}
}


void mergeSols(vector<SolutionB*> &population, int lower, int middle, int upper, string scoreType){
	vector<SolutionB*> temp;
	int lowerish = lower;
	int middlish = middle + 1;
	double scorel, scorem;

	while ((middlish <= upper) && (lowerish <= middle)) {
		
		if (scoreType == "iucn") {
			scorel = (double) population[lowerish]->score;
			scorem = (double) population[middlish]->score;
		} else if (scoreType == "ndm") {
			scorel = population[lowerish]->ndmScore;
			scorem = population[middlish]->ndmScore;
		} else if (scoreType == "aggregated") {
			scorel = population[lowerish]->aggrScore;
			scorem = population[middlish]->aggrScore;
		}

		if(scorel >= scorem) {
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


void sortSols(vector<SolutionB*> &population, int lower, int upper, string scoreType){
	if (lower < upper) {
		int middle = ((lower + upper) / 2);
		sortSols(population, lower, middle, scoreType);
		sortSols(population, (middle + 1), upper, scoreType);
		mergeSols(population, lower, middle, upper, scoreType);
		}
	}


vector<SolutionB*> dropSearch(map<int, vector<int>> &clusters, vector <Mesh*> &observations, int iters, int outSize, double ndmOutFactor, double ndmAbsFactor, double ndmWeight){
	//cout << "In dropSearch\n";
	int clusNum = -1;
	int thisClus;
	int thisObs;
	int thisCell;
	bool breakOuter;
	double initscore;
	map<int, vector<int>>::iterator it;
	map<int, int> exclMap;
	//vector<int> island;
	vector<SolutionB*> mySols;

	for (it = clusters.begin(); it != clusters.end(); it++) {
		if (clusNum < it->first){
			clusNum = it->first;
		}
	}
	clusNum += 1;

	for (int t = 0; t < iters; t++) {
		breakOuter = false;
		thisClus = rand() % clusNum;
		thisObs = rand() % clusters[thisClus].size();
		thisObs = clusters[thisClus][thisObs];
		thisCell = rand() % observations[thisObs]->getSize();

		while (observations[thisObs]->getValue(thisCell) <= 0) {
			thisCell = rand() % observations[thisObs]->getSize();
		}

		for (int j = 0; j < observations[0]->getSize(); j++) {
			exclMap[j] = 0;
		}
		exclMap[thisCell] = 1;

		SolutionB * asol = new SolutionB(observations[thisObs]);
		asol->nullMe();
		asol->setValue(thisCell, 1);
		fitness(asol, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
		initscore = asol->aggrScore;
		solExpansion(asol, observations, exclMap, thisCell, initscore, ndmOutFactor, ndmAbsFactor, ndmWeight);

		for (int i = 0; i < mySols.size(); i++){
			if (equal(asol, mySols[i])) {
				breakOuter = true;
				delete asol;
				break;
			}
		}

		if (breakOuter) {
			continue;
		}

		mySols.push_back(asol);
		//cout << "mySols now has " << mySols.size() << " elements\n";
	}

	/* Create new Solutions using the starting seed
	Come up with some new areas from the intersection zones of Solutions in the
	seed.
	*/

	/* Include an overlapping threshold to filter out solutions too 
	similar */
	
	int vecSi = mySols.size();
	for (int i = 0; i < vecSi; i++) {
		for (int j = i+1; j < vecSi; j++) {
			if ( overlap(mySols[i], mySols[j]) ) {
			
				SolutionB * comp0 = new SolutionB(mySols[j]);
				SolutionB * comp1 = new SolutionB(mySols[i]);
				SolutionB * inter = new SolutionB(mySols[i]);
				inter->nullMe();

				for (int c = 0; c < mySols[i]->getSize(); c++) {

					if ((mySols[i]->getValue(c) > 0) && (mySols[j]->getValue(c) > 0)) {
						comp0->setValue(c, 0);
						comp1->setValue(c, 0);
						inter->setValue(c, 1);
					}
				}
				
				/***********************************
				*  Check if new areas are continuous
				************************************/
				
				bool del0 = false;
				bool del1 = false;
				bool deli = false;

				if (comp0->isNull()) {
					delete comp0;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp0, mySols[m]) ) {
							delete comp0;
							del0 = true;
							break;
						} 
					}
					
					if (!del0) {
						fitness(comp0, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
						mySols.push_back(comp0);
					}
				}

				if (comp1->isNull()) {
					delete comp1;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp1, mySols[m]) ) {
							delete comp1;
							del1 = true;
							break;
						} 
					}
					
					if (!del1) {
						fitness(comp1, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
						mySols.push_back(comp1);
					}
				}

				if (inter->isNull()) {
					delete inter;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(inter, mySols[m]) ) {
							delete inter;
							deli = true;
							break;
						} 
					}
					
					if (!deli) {
						fitness(inter, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
						mySols.push_back(inter);
					}
				}

			}
		}
	}
	
	sortSols(mySols, 0, (mySols.size() - 1), "aggregated");


	if (mySols.size() > outSize) {
		for (int i = outSize; i < mySols.size(); i++) {
			delete mySols[i];
		}
		mySols.resize(outSize);
	}

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
		if ((meshA->getValue(i) > 0) && (meshB->getValue(i) > 0)){
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


vector<int> inter (SolutionB * solA, SolutionB * solB) {
	//	cout << "In interCompl function. Initial Solution objects:" << endl;
	vector<int> out;
	// a = A not B, b = B not A, ab = A and B

	for (int i = 0; i < solA->getSize(); i++){
		if ((solA->getValue(i) > 0) && (solB->getValue(i) > 0)) {
			out.push_back(i);
			}
		}

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


void solExpansion (SolutionB* solita, vector<Mesh*> &observations, map<int,int> &exclMap, int cellIndx, double prescore, double ndmOutFactor, double ndmAbsFactor, double ndmWeight) {

	vector<int> thisNeighs = solita->getCellNeighs(cellIndx);
	double postscore;

	for (int n = 0; n < thisNeighs.size(); n++) {
		//cout << "exclMap[thisNeighs[n]] : " << exclMap[thisNeighs[n]] << endl;
		if (exclMap[thisNeighs[n]] == 0){

			solita->setValue(thisNeighs[n], 1);
			fitness(solita, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);
			postscore = solita->aggrScore;
			// cout << "prescore: " << prescore << ", postscore: " << postscore << endl; 

			if (postscore > prescore) { 

				exclMap[thisNeighs[n]] = 1;
				solExpansion(solita, observations, exclMap, thisNeighs[n], postscore, ndmOutFactor, ndmAbsFactor, ndmWeight);

			} else {
				
				solita->setValue(thisNeighs[n], 0);
				fitness(solita, observations, ndmOutFactor, ndmAbsFactor, ndmWeight, false);

			}
		}
	}
}


vector<SolutionB*> exhSearch (vector<Mesh*> &observations, double ndmOutFactor, double ndmAbsFactor, double ndmWeight) {

	vector<SolutionB*> out;
	map<int, int> excluded;
	int meshSize = observations[0]->getSize();
	double initscore;
	bool breakOuter;

	for (int i = 0; i < meshSize; i++) {

		for (int j = 0; j < meshSize; j++) {
			excluded[j] = 0;
		}

		SolutionB * asol = new SolutionB(observations[0]);
		asol->nullMe();
		asol->setValue(i, 1);
		fitness(asol, observations, ndmOutFactor,ndmAbsFactor, ndmWeight, false);
		initscore = asol->aggrScore;
		
		if (initscore > 0) {

			breakOuter = false;
			solExpansion(asol, observations, excluded, i, initscore, ndmOutFactor,ndmAbsFactor, ndmWeight);

			for (int k = 0; k < out.size(); k++) {
				
				if (equal(asol, out[k])) {
					breakOuter = true;
					delete asol;
					break;

				}
			}

			if (breakOuter) {
				continue;
			}

			out.push_back(asol);

		} else {

			delete asol;

		}
	}

	for (int i = 0; i < out.size(); i++) {
		for (int j = i + 1; j < out.size(); j++) {
			break;
			if (overlap(out[i], out[j])) {
				if (out[i]->aggrScore > out[j]->aggrScore) {
					out[j]->aggrScore = 0.0;
					/*SolutionB* temp = out[k];
					out[k] = asol;
					asol = temp;
					delete asol;
					breakOuter = true;
					break;*/
				} else {
					out[j]->aggrScore = 0.0;
				}		
			} 
		}
	}

	return out;

}


vector<SolutionB*> meta(vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmOutFactor, double ndmAbsFactor, double ndmWeight){

	map<int, vector<int>> clusterSch;
	vector<SolutionB*> sols;

	clusterSch = dbscan(observations, clusterEps);

	sols = dropSearch(clusterSch, observations, iters, outSize, ndmOutFactor, ndmAbsFactor, ndmWeight);

	return sols;

}


vector<SolutionB*> metaAlt(vector<Mesh*> &observations, double clusterEps, int iters, int outSize, double ndmWeight) {

	map<int, vector<int>> clusterSch;
	vector<SolutionB*> sols;

	clusterSch = dbscan(observations, clusterEps);

	sols = dropSearchAlt(clusterSch, observations, iters, outSize, ndmWeight);

	return sols;

}