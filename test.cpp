#include <iostream>
#include <vector>
#include <map>
#include "Data4py.cpp"
#include "search4py.cpp"
#include "sim.cpp"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::map;


void printMe (Mesh* amesh, int side) {
	int index;
	for (int r = 0; r < side; r++) {
		for (int c = 0; c < side; c++) {
			index = r * side + c;
			if (amesh->getValue(index) > 0) {
				cout << 1;
			} else {
				cout << 0;
			}
		}
		cout << endl;
	}
}

int main(){

	srand((unsigned) time(0));
	int tileSide = 6;
	vector<Mesh*> obs;
	vector<SolutionB*> sols;
	obs = overTiles2(tileSide, 4, 100);

	map<int, vector<int>>::iterator it;
	//map<int, vector<int>> clusters = dbscan(obs, 0.2);

	/* for (it = clusters.begin(); it != clusters.end(); it++) {
		cout << "Cluster " << it->first << ":" << endl;
		for (int i = 0; i < it->second.size(); i++){
			cout << "  " << it->second[i] << endl;
		}
	} */

	for (int q = 0; q < obs.size(); q++) {
		cout << "Tile " << q << endl;
		printMe(obs[q], tileSide);
		cout << endl;
	}


/*	SolutionB* sola = new SolutionB(obs[0]);
	sola->nullMe();
	sola->setValue(6, 1);
	cout << "Solution init:" << endl;
	printMe(sola, tileSide);
	cout << endl;
	
	map<int,int> emap;
	for (int i = 0; i < obs.size(); i++) {
		emap[i] = 0;
	}

	fitness(sola, obs, 0.001, false);
	emap[6] = 1;
	solExpansion(sola, obs, emap, 6, sola->aggrScore, 0.001);

	cout << "Solution expanded:" << endl;
	printMe(sola, tileSide);
	cout << endl;
*/

	/*
	SolutionB* solb = new SolutionB(obs[0]);
	solb->nullMe();
	solb->setValue(28, 1);
	fitness(solb, obs, 0.2, false);
	cout << "Another Solution" << endl;
	cout << solb->score << ", " << solb->ndmScore << ", " << solb->aggrScore << endl;
	printMe(solb, tileSide);
	cout << endl;
	*/


	//delete sola;
	//delete solb;

	//*
	//sols = dropSearch(clusters, obs, 100, 10, 0.01, 1.0);
	sols = meta(obs, 0.2, 100, 10, 0.01, 1.0);
	//sortSols(sols, 0, (sols.size() - 1), "aggregated");
	
	for (int q = 0; q < sols.size(); q++) {
		cout << "Solution " << q << ": " << sols[q]->score << ", " << sols[q]->ndmScore
			<< ", " << sols[q]->aggrScore << endl;
		printMe(sols[q], tileSide);
		cout << endl;
	}
	//*/

	for (int x = 0; x < obs.size(); x++){
		delete obs[x];
	}

	//*
	for (int x = 0; x < sols.size(); x++){
		delete sols[x];
	}
	//*/

	return 0;

}
