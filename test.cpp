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


int main(){

	srand((unsigned) time(0));

	vector<Mesh*> obs;
	obs = nestedTiles(6, 3);

	map<int, vector<int>>::iterator it;
	map<int, vector<int>> clusters = dbscan(obs, 0.2);

	for (it = clusters.begin(); it != clusters.end(); it++) {
		cout << "Cluster " << it->first << ":" << endl;
		for (int i = 0; i < it->second.size(); i++){
			cout << "  " << it->second[i] << endl;
			}
		}

	cout << endl;

	for (int x = 0; x < obs.size(); x++){
		delete obs[x];
		}

	return 0;

	}
