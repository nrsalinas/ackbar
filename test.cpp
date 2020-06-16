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
	obs = overTiles(tileSide, 1, 100);

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

	cout << endl;

	sols = exhSearch (obs);
	sortSols(sols, 0, (sols.size() - 1), "aggregated");
	
	for (int q = 0; q < sols.size(); q++) {
		cout << "Solution " << q << ": " << sols[q]->score << ", " << sols[q]->ndmScore << endl;
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
