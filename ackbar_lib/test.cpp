/*#############################################################################
#
#	Copyright 2020 Nelson R. Salinas
#
#
#	This file is part of Ackbar.
#
#   Ackbar is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
# 	(at your option) any later version.
#
#	Ackbar is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with Ackbar. If not, see <http://www.gnu.org/licenses/>.
#
#############################################################################*/


#include "data.cpp"
#include "search.cpp"
#include "sim.cpp"

#include <iostream>
#include <vector>
#include <map>

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
	vector< vector<Solution*> > sols;
	obs = overTiles2(tileSide, 3, 100);
	for (uint i = 0; i < obs.size(); i++) {
		obs[i]->setRange(1000);
	}
	int midi = obs.size() / 2;

	map<int, vector<int> > gr;
	gr[0].push_back(10000);
	gr[0].push_back(2);
	gr[1].push_back(2000);
	gr[1].push_back(2);

	map<int, int> s2g;
	for (int i = 0; i < midi; i++) {
		s2g[i] = 0;
	}
	for (uint i = midi; i < obs.size(); i++) {
		s2g[i] = 1;
	}


	map<int, vector<int> >::iterator it;
	map<int, vector<int> > clusters = dbscan(obs, 0.2);

	/* for (it = clusters.begin(); it != clusters.end(); it++) {
		cout << "Cluster " << it->first << ":" << endl;
		for (uint i = 0; i < it->second.size(); i++){
			cout << "  " << it->second[i] << endl;
		}
	} */

	for (uint q = 0; q < obs.size(); q++) {
		cout << "Tile " << q << endl;
		printMe(obs[q], tileSide);
		cout << endl;
	}

	map <int, double> myScoGrid = presGrid(obs);
	map<int, double>::iterator it2;

	sols = dropSearchAlt(clusters, obs, gr, s2g, 200, 50, 1.0);

	int nuis;
	bool isco;
	
	for (uint q = 0; q < sols.size(); q++) {
		cout << "Group " << q << endl;
		for (uint r = 0; r < sols[q].size(); r++) {
			nuis = islandNumber(sols[q][r]);
			isco = isContinuous(sols[q][r]);

			cout << "Solution " << r << ": " << sols[q][r]->score << ", " << sols[q][r]->ndmScore
				<< ", " << sols[q][r]->aggrScore << ". Islands: " << nuis << endl;

			if (!isco) {
				cout << "NOT CONTINUOUS!!!" << endl;
			}

			printMe(sols[q][r], tileSide);
			cout << endl;
		}
	}


	for (uint x = 0; x < obs.size(); x++){
		delete obs[x];
	}

	
	for (uint x = 0; x < sols.size(); x++){
		for (uint y = 0; y < sols[x].size(); y++) {
			delete sols[x][y];
		}
	}

	return 0;

}
