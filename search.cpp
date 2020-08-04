#include "search.hpp"

vector< vector<Solution*> > metaAlt (vector<Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, double clusterEps, int iters, int outSize, double ndmWeight) {

	map<int, vector<int> > clusterSch;
	vector< vector<Solution*> > sols;

	clusterSch = dbscan(observations, clusterEps);

	sols = dropSearchAlt(clusterSch, observations, taxGroups, spp2groups, iters, outSize, ndmWeight);

	return sols;

}


vector< vector<Solution*> > dropSearchAlt (map<int, vector<int> > &clusters, vector <Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, int iters, int outSize, double ndmWeight){
	
	int clusNum = -1;
	int thisClus;
	int thisObs;
	int thisCell;
	bool breakOuter;
	double initscore;
	map<int, vector<int> >::iterator it;
	map<int, int> exclMap;
	//vector<int> island;
	vector<Solution*> mySols;
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
		thisObs = clusters[thisClus].at(thisObs);
		thisCell = rand() % observations.at(thisObs)->getSize();

		while (observations.at(thisObs)->getValue(thisCell) <= 0) {
			thisCell = rand() % observations.at(thisObs)->getSize();
		}

		Solution * asol = new Solution(observations.at(thisObs));
		asol->nullMe();
		asol->setValue(thisCell, 1);
		asol->extent = 1;

		asol->ndmScore = scoreGrid[thisCell];
		
		for (int j = 0; j < observations.at(0)->getSize(); j++) {
			exclMap[j] = 0;
		}
		exclMap[thisCell] = 1;

		for (int k = 0; k < observations.size(); k++) {
			asol->sppAreas[k] = observations.at(k)->getValue(thisCell);
		}

		solExpansionAlt(asol, scoreGrid, exclMap, thisCell);

		for (int i = 0; i < mySols.size(); i++){
			if (equal(asol, mySols.at(i))) {
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
			if ( overlap(mySols.at(i), mySols.at(j)) ) {
			
				Solution * comp0 = new Solution(mySols.at(j));
				comp0->origin = 1;
				Solution * comp1 = new Solution(mySols.at(i));
				comp1->origin = 1;
				Solution * inter = new Solution(mySols.at(i));
				inter->origin = 1;
				inter->nullMe();

				for (int c = 0; c < mySols.at(i)->getSize(); c++) {

					if (mySols.at(i)->getValue(c) > 0) {

						if (mySols.at(j)->getValue(c) > 0) {
						
							comp0->setValue(c, 0);
							comp1->setValue(c, 0);
							inter->setValue(c, 1);
							inter->extent += 1;
							inter->ndmScore += scoreGrid[c];
						
						} else {
						
							comp1->extent += 1;
							comp1->ndmScore += scoreGrid[c];
						
						}

					} else if (mySols.at(j)->getValue(c) > 0) {
						
						comp0->extent += 1;
						comp0->ndmScore += scoreGrid[c];

					}

				}

				inter->ndmScore /= (double) inter->extent;
				comp0->ndmScore /= (double) comp0->extent;
				comp1->ndmScore /= (double) comp1->extent;
				
				bool del0 = false;
				bool del1 = false;
				bool deli = false;

				/***********************************
				*  Assess Solution scores
				************************************/
				
				if (comp0->isNull() || !isContinuous(comp0)) {
					delete comp0;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp0, mySols.at(m)) ) {
							delete comp0;
							del0 = true;
							break;
						} 
					}
					
					if (!del0) {
						mySols.push_back(comp0);
					}
				}

				if (comp1->isNull() || !isContinuous(comp1)) {
					delete comp1;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(comp1, mySols.at(m)) ) {
							delete comp1;
							del1 = true;
							break;
						} 
					}
					
					if (!del1) {
						mySols.push_back(comp1);
					}
				}

				if (inter->isNull() || !isContinuous(inter)) {
					delete inter;
				} else {
					for (int m = 0; m < mySols.size(); m++) {
						if ( equal(inter, mySols.at(m)) ) {
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
	
	// Checking if assessing aggregated scores before sorting takes too much time

	for (int s = 0; s < mySols.size(); s++) {
		complScore(mySols.at(s), observations, taxGroups, spp2groups, ndmWeight);
	}

	sortSols(mySols, 0, (mySols.size() - 1), "aggregated");

	vector<int> setMap(mySols.size(), -1);
	
	vector< vector<int> > groups = solSets(mySols, setMap);

	vector< vector<Solution*> > newSols; //= mySols;

	int counter = 0;
	for (int g = 0; g < groups.size(); g++) {
		vector <Solution*> tmp;
		for (int h = 0; h < groups.at(g).size(); h++) {
			if (counter < outSize) {
				//complScore(mySols[groups[g][h]], observations, ndmWeight);
				tmp.push_back(mySols.at(groups.at(g).at(h)));
				counter += 1;
			} else {
				delete mySols.at(groups.at(g).at(h));
			}
		}
		if (tmp.size() > 0) {
			newSols.push_back(tmp);
		}
	}

	return newSols;
}


void solExpansionAlt(Solution* solita, map<int, double> &scoringGrid, map<int,int> &exclMap, int cellIndx) {

	vector<int> thisNeighs = solita->getCellNeighs(cellIndx);
	double prescore, postscore;

	for (int n = 0; n < thisNeighs.size(); n++) {
		
		if (exclMap[ thisNeighs.at(n) ] == 0){

			postscore = (solita->ndmScore * (double) solita->extent + scoringGrid[ thisNeighs.at(n) ]) / ((double) solita->extent + 1.0);

			if (postscore > solita->ndmScore) { 

				solita->setValue(thisNeighs.at(n), 1);
				solita->ndmScore = postscore;
				solita->extent += 1;
				exclMap[ thisNeighs.at(n) ] = 1;
				solExpansionAlt(solita, scoringGrid, exclMap, thisNeighs.at(n));

			}
		}
	}
}


map<int, vector<int> > dbscan (vector<Mesh*> &observations, double eps){

	map<int, vector<int> > out;
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


void expand (vector<Mesh*> &observations, map<int, vector<int> > &clusters, map<int, int> &visited, int label, int border, double eps) {

	for (int i = 0; i < observations.size(); i++) {

		if ((visited[i] == 0) && (i != border)) {

			if (eps > kulczynski(observations.at(border), observations.at(i))) {

				clusters[label].push_back(i);
				visited[i] = 1;
				expand(observations, clusters, visited, label, i, eps);

			}

		}

	}

}


map<int, double> presGrid (vector <Mesh*> &observations){
	map<int, double> out;

	for (int c = 0; c < observations.at(0)->getSize(); c++){
		out[c] = 0.0;
		for (int i = 0; i < observations.size(); i++){
			out[c] += observations.at(i)->getValue(c);
		}
	}
	return out;
}


void complScore (Solution * rsearchSol, vector <Mesh*> &observations, map<int, vector<int> > &taxGroups, map<int, int> &spp2groups, double ndmWeight) {
	rsearchSol->score = 0;
	rsearchSol->critA = 0;
	rsearchSol->critB = 0;
	rsearchSol->spp2crit.clear();
	map<int, int> scoringSpp;
	map<int, vector<int> > groupSpp;
	map<int, int> groupScore;

	if ((taxGroups.size() > 0) && (spp2groups.size() > 0)) {
		for (int k = 0; k < taxGroups.size(); k++) {
			groupScore[k] = 0;
		}
	}

	for(int i = 0; i < observations.size(); i++){
		string status = observations.at(i)->getThreatStatus();
		vector <int> subcritA = observations.at(i)->getThreatSubcriteriaA();
		bool properA = false;
		int suppA = 0;
		int suppB = 0;
		int suppBB = 0;
		int pass = 0;
		scoringSpp[i] = 0;
		double popIncluded = 0.0;

		for(int c = 0; c < rsearchSol->getSize(); c++){
			if (rsearchSol->getValue(c) > 0){
				popIncluded += observations.at(i)->getValue(c);
			}
		}

		for (int a = 0; a < subcritA.size(); a++){
			if ((subcritA.at(a) == 1) || (subcritA.at(a) == 2) || (subcritA.at(a) == 4)) {
				properA = true;
				break;
			}
		}

		if ((status == "CR") || (status == "EN")) {

			if (popIncluded >= 0.005){
				suppA = 1;
				pass = 1;
				scoringSpp[i] = 1;
				rsearchSol->spp2crit[i].push_back(0);
			}
		
			if ((popIncluded >= 0.01) & (properA)) {
				suppA = 1;
				pass = 1;
				scoringSpp[i] = 1;
				rsearchSol->spp2crit[i].push_back(2);
			}
			
			if (popIncluded >= 0.95) {
				suppA = 1;
				pass = 1;
				scoringSpp[i] = 1;
				rsearchSol->spp2crit[i].push_back(4);
			}

		} else if (status == "VU") {
			
			if (popIncluded >= 0.01){
				suppA = 1;
				pass = 1;
				scoringSpp[i] = 1;
				rsearchSol->spp2crit[i].push_back(1);
			}

			if ((popIncluded >= 0.02) & (properA)) {
				suppA = 1;
				pass = 1;
				scoringSpp[i] = 1;
				rsearchSol->spp2crit[i].push_back(3);
			}
		}

		if (popIncluded >= 0.1) {
			suppB = 1;
			pass = 1;
			scoringSpp[i] = 1;
			rsearchSol->spp2crit[i].push_back(5);
		}

		if ((taxGroups.size() > 0) && (spp2groups.size() > 0)) {
			if ((popIncluded >= 0.01) && (observations.at(i)->getRange() <= taxGroups[spp2groups[i]].at(0))) {
				groupScore[spp2groups[i]] += 1;
				groupSpp[spp2groups[i]].push_back(i);
				if (groupScore[spp2groups[i]] >= taxGroups[spp2groups[i]].at(1)) {
					suppB = 1;
				}
			}
		}

		rsearchSol->critA += suppA;
		rsearchSol->critB += suppB;
	}

	if ((taxGroups.size() > 0) && (spp2groups.size() > 0)) {
		for (int j = 0; j < groupScore.size(); j++) {
			if (groupScore[j] > taxGroups[j].at(1)) {
				for (int k = 0; k < groupSpp[j].size(); k++) {
					rsearchSol->spp2crit[groupSpp[j][k]].push_back(6);
					scoringSpp[groupSpp[j][k]] = 1;
				
				}
			}
		}
	}

	for (int i = 0; i < scoringSpp.size(); i++) {
		rsearchSol->score += scoringSpp[i];
	}

	rsearchSol->aggrScore = (double) rsearchSol->score + ndmWeight * rsearchSol->ndmScore;

}


vector< vector<int> > solSets (vector <Solution*> &initSols, vector<int> &setMap) {
	// retrieve groups of non-overlapping solutions.
	vector< vector<int> > out; // clusters of element indexes
	vector<int> visited(initSols.size(), 0);
	int cc = -1;

	for (int i = 0; i < initSols.size(); i++) {
		if (visited.at(i) == 0) {
			vector<int> thisgroup;
			visited.at(i) = 1;
			thisgroup.push_back(i);
			cc += 1;
			setMap.at(i) = cc;
			
			for (int j = i + 1; j < initSols.size(); j++) {
				if (visited.at(j) == 0) {
					bool addme = true;
					for (int k = 0; k < thisgroup.size(); k++) {
						if (overlap(initSols.at(thisgroup.at(k)), initSols.at(j))) {
							addme = false;
							break;
						}
					}
					if (addme == true) {
						visited.at(j) = 1;
						thisgroup.push_back(j);
						setMap.at(j) = cc; 
					}
				}
			}

			if (thisgroup.size() > 1) {
				out.push_back(thisgroup);
			}
		}
	}

	return out;
}


void sortSols (vector<Solution*> &population, int lower, int upper, string scoreType){
	if (lower < upper) {
		int middle = ((lower + upper) / 2);
		sortSols(population, lower, middle, scoreType);
		sortSols(population, (middle + 1), upper, scoreType);
		mergeSols(population, lower, middle, upper, scoreType);
	}
}


void mergeSols (vector<Solution*> &population, int lower, int middle, int upper, string scoreType){
	vector<Solution*> temp;
	int lowerish = lower;
	int middlish = middle + 1;
	double scorel, scorem;

	while ((middlish <= upper) && (lowerish <= middle)) {
		
		if (scoreType == "iucn") {
			scorel = (double) population.at(lowerish)->score;
			scorem = (double) population.at(middlish)->score;
		} else if (scoreType == "ndm") {
			scorel = population.at(lowerish)->ndmScore;
			scorem = population.at(middlish)->ndmScore;
		} else if (scoreType == "aggregated") {
			scorel = population.at(lowerish)->aggrScore;
			scorem = population.at(middlish)->aggrScore;
		}

		if(scorel >= scorem) {
			temp.push_back(population.at(lowerish));
			lowerish++;
		} else {
			temp.push_back(population.at(middlish));
			middlish++;
		}
	
	}

	while (middlish <= upper) {
		temp.push_back(population.at(middlish));
		middlish++;
	}

	while (lowerish <= middle) {
		temp.push_back(population.at(lowerish));
		lowerish++;
	}

	for (int i = lower; i <= upper; i++) {
		population.at(i) = temp.at(i-lower);
	}

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


bool isContinuous (Solution* solita) {

	int islands = 0;
	bool out = true;
	map <int, int> checked;
	
	for (int i = 0; i < solita->getSize(); i++) {
		checked[i] = 0;
	}

	for (int i = 0; i < solita->getSize(); i++) {
		if (checked[i] == 0) {
			checked[i] = 1;
			if (solita->getValue(i) > 0) {
				islands += 1;
				if (islands >= 2) {
					out = false;
					break;
				} else {
					islandCheck(solita, checked, i);
				}
			}
		}
	}

	return out;

}


int islandNumber(Solution * solita) {

	int out = 0;
	map <int, int> checked;
	
	for (int i = 0; i < solita->getSize(); i++) {
		checked[i] = 0;
	}


	for (int i = 0; i < solita->getSize(); i++) {
		if (checked[i] == 0) {
			checked[i] = 1;
			if (solita->getValue(i) > 0) {
				out += 1;
				islandCheck(solita, checked, i);
			}
		}
	}

	return out;

}


void islandCheck (Solution* solita, map <int, int> &checked, int cellIndx) {
	
	vector <int> neighs = solita->getCellNeighs(cellIndx);
	
	for (int n = 0; n < neighs.size(); n++) {
		if (checked[ neighs.at(n) ] == 0) {
			checked[ neighs.at(n) ] = 1;
			if (solita->getValue(neighs.at(n)) > 0) {
				islandCheck(solita, checked, neighs.at(n));
			}
		}	
	}
}


