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
	obs = overTiles2(tileSide, 3, 100);

	map<int, vector<int>>::iterator it;
	map<int, vector<int>> clusters = dbscan(obs, 0.2);

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

	map <int, double> myScoGrid = presGrid(obs);
	map<int, double>::iterator it2;
	/*
	cout << "\nScoring grid:\n" << endl;
	for (it2 = myScoGrid.begin(); it2 != myScoGrid.end(); it2++) {
		cout << it2->first << " | " << it2->second << endl;
	}
	cout << endl; */

	/*
	SolutionB* sola = new SolutionB(obs[0]);
	sola->nullMe();
	sola->setValue(27, 1);
	cout << "Solution init:" << endl;
	printMe(sola, tileSide);
	cout << endl;
	
	map<int,int> emap;
	for (int i = 0; i < sola->getSize(); i++) {
		emap[i] = 0;
	}

	emap[27] = 1;
	solExpansionAlt(sola, myScoGrid, emap, 27);

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

	/*
	//sols = dropSearch(clusters, obs, 100, 10, 0.01, 1.0);
	//sols = meta(obs, 0.2, 100, 10, 0.01, 0.01, 3);
	//sortSols(sols, 0, (sols.size() - 1), "aggregated");
	//*/

	sols = dropSearchAlt(clusters, obs, 20, 5, 1.0);
	
	for (int q = 0; q < sols.size(); q++) {
		cout << "Solution " << q << ": " << sols[q]->score << ", " << sols[q]->ndmScore
			<< ", " << sols[q]->aggrScore << endl;
		printMe(sols[q], tileSide);
		cout << endl;
	}


	for (int x = 0; x < obs.size(); x++){
		delete obs[x];
	}

	
	for (int x = 0; x < sols.size(); x++){
		delete sols[x];
	}

	return 0;

}
