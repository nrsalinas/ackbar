#include "search.hpp"

void fitness(SolutionB * rsearchSol, vector <Mesh*> &observations, double overPenalty,
	bool updateAll){
	rsearchSol->score = 0;
	rsearchSol->critA = 0;
	rsearchSol->critB = 0;
	if (updateAll){
		rsearchSol->spp2crit.clear();
		}

	for(int i = 0; i < observations.size(); i++){
		double popIncluded = 0.0; 
		double thisScore = 0.0;
		string status = observations[i]->getThreatStatus();
		vector <int> subcritA = observations[i]->getThreatSubcriteriaA();
		bool properA = false;
		for (int a = 0; a < subcritA.size(); a++){
			if ((subcritA[a] == 1) || (subcritA[a] == 2) || (subcritA[a] == 4)) {
				properA = true;
				break;
				}
			}

		for(int c = 0; c < rsearchSol->getSize(); c++){
			if (rsearchSol->getValue(c) > 0){
				popIncluded += (double) observations[i]->getValue(c);
				}
			}


		if ((status == "CR") || (status == "EN")) {
			if (popIncluded >= 0.005){
				rsearchSol->critA += 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(0);
					}
				}
			if ((popIncluded >= 0.01) & (properA)) {
				rsearchSol->critA += 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(2);
					}
				}
			if (popIncluded >= 0.95) {
				rsearchSol->critA += 1;
				if (updateAll){
					rsearchSol->spp2crit[i].push_back(4);
					}
				}


			} else if (status == "VU") {
				if (popIncluded >= 0.01){
					rsearchSol->critA += 1;
					if (updateAll){
						rsearchSol->spp2crit[i].push_back(1);
						}
					}
				if ((popIncluded >= 0.02) & (properA)) {
					rsearchSol->critA += 1;
					if (updateAll){
						rsearchSol->spp2crit[i].push_back(3);
						}
					}
				}


		if (popIncluded >= 0.1) {
			rsearchSol->critB += 1;
			if (updateAll){
				rsearchSol->spp2crit[i].push_back(5);
				}
			}

		if (popIncluded >= 0.01) {
			rsearchSol->critB += 1;
			if (updateAll){
				rsearchSol->spp2crit[i].push_back(6);
				}
			}


		}
	rsearchSol->score = rsearchSol->critA + rsearchSol->critB;
	}

