#include <iostream>
#include <vector>
#include <map>
#include "Data4py.cpp"
#include "search4py.cpp"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::map;


vector<Mesh*> nestedTiles (int size, int tilesxsize) {

	vector<Mesh*> out;
	int index, midx;

	if (size % 3 == 0) {

		for (int m = 0; m < 3; m++) {

			for (int r = 0; r < tilesxsize; r++) {

				out.push_back(new Mesh(size * size));
				
				for (int irow = 0; irow < size; irow++) {

					for (int icol = 0; icol < size; icol++) {

						midx = (m * tilesxsize) + r; 

						if (m == 0) {

							if ((irow < (size / 3)) && (icol < (size / 3))) {							
								index = (irow * size) + icol;
								out[midx]->setValue(index, 1);
								}

							} else if (m == 1) {

								if ((irow < (2 * size / 3)) && (icol < (2 * size / 3))) {
									index = (irow * size) + icol;
									out[midx]->setValue(index, 1);
									}

								} else {
									index = (irow * size) + icol;
									out[midx]->setValue(index, 1);
									}							
						}
					}
				}
			}
		}

	return out;

	}


int main(){

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


	for (int x = 0; x < obs.size(); x++){		
		delete obs[x];		
		}

	return 0;
	
	}