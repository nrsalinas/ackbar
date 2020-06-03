#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>
#include "Data.cpp"

using namespace std;

map<string, map<float, map<float, int>>> readFile (string input) {
	
	map<string, map<float, map<float, int>>> out;
	int counter = 0;
	int colcounter = 0;
	float lat, lon;
	string line, cell, taxon;

	ifstream myFile(input);
	
	if(!myFile.is_open()){
		throw std::runtime_error("Could not open file");
		}	

	if (myFile.good()) {

		while (getline(myFile, line)) {
			
			stringstream ss(line);
			colcounter = 0;
			
			if (counter > 0){

				while (getline(ss, cell, ',')) {
					//cout << colcounter << ": " << cell << " | ";

					if (colcounter == 0) {
						taxon = cell;

						} else if (colcounter == 1){
							lat = stof(cell);

							} else if (colcounter == 2) {
								lon = stof(cell);
								}

					colcounter += 1;
					}

				out[taxon][lat][lon] = 0;
				//cout << taxon << " | " << lat << " | " << lon << endl;
				}


			/*if (counter > 50) {
				break;
				}
			*/
			counter += 1;

			}
		}


	myFile.close();

	return out;

	}

vector<Mesh*> togrid(map<string, map<float, map<float, int>>> &indata, float cellHeight, float cellWidth,
	float xOffset = 0.0, float yOffset = 0.0) {

	int totCols, totRows, tRow, tCol, cellIdx;
	float originLat, originLon, spanLat, spanLon;
	float minlat = 1000;
	float maxlat = -1000;
	float minlon = 1000;
	float maxlon = -1000;
	map<string, map<float, map<float, int>>>::iterator itspp;
	map<float, map<float, int>>::iterator itlat;
	map<float, int>::iterator itlon;
	vector<Mesh*> tiles;

	// get grid span

	for (itspp = indata.begin(); itspp != indata.end(); itspp++) {

		for (itlat = indata[itspp->first].begin(); itlat != indata[itspp->first].end(); itlat++) {
	
			if (minlat > itlat->first) {
				minlat = itlat->first;
				}

			if (maxlat < itlat->first) {
				maxlat = itlat->first;
				}

			for (itlon = indata[itspp->first][itlat->first].begin(); itlon != indata[itspp->first][itlat->first].end(); itlon++) {
		
				if (minlon > itlon->first) {
					minlon = itlon->first;
					}

				if (maxlon < itlon->first) {
					maxlon = itlon->first;
					}
		
				}

			}

		}

	originLat = maxlat + yOffset;
	originLon = minlon - xOffset;
	spanLat = originLat - minlat;
	spanLon = maxlon - originLon;

	totCols = (int) ceil(spanLon / cellWidth);
	totRows = (int) ceil(spanLat / cellHeight);

	cout << originLat << ", " << originLon << endl;
	cout << spanLat << ", " << spanLon << endl;
	cout << "Rows: " << totRows << ", Cols: " << totCols << endl;

	// encode

	for (itspp = indata.begin(); itspp != indata.end(); itspp++) {

		Mesh * amesh = new Mesh( (totCols * totRows), itspp->first );
		amesh->setRowsCols(totRows, totCols);

		for (itlat = indata[itspp->first].begin(); itlat != indata[itspp->first].end(); itlat++) {
	
			for (itlon = indata[itspp->first][itlat->first].begin(); itlon != indata[itspp->first][itlat->first].end(); itlon++) {
				
				tCol = (int) ceil(((itlon->first - originLon) / spanLon) * (float) totCols);
				tRow = (int) ceil(((originLat - itlat->first) / spanLat) * (float) totRows);

				if (tCol == 0) {
					tCol = 1;
					}

				if (tRow == 0) {
					tRow = 1;
					}

				cellIdx = ((tRow-1) * totCols) + (tCol-1);
				amesh->setValue(cellIdx, 1);

				}

			}

		tiles.push_back(amesh);

		}

	return tiles;

	}

int main(){

	vector <Mesh*> mytiles;
	map<string, map<float, map<float, int>>> indata;
	string input = "orthaea.csv";
	indata = readFile(input);
	int nspp, sppoints;

	map<string, map<float, map<float, int>>>::iterator itspp;
	map<float, map<float, int>>::iterator itlat;
	map<float, int>::iterator itlon;

	nspp = 0;
	for (itspp = indata.begin(); itspp != indata.end(); itspp++) {
		nspp += 1;
		sppoints = 0;
		cout << itspp->first << endl;
		for (itlat = indata[itspp->first].begin(); itlat != indata[itspp->first].end(); itlat++) {
			for (itlon = indata[itspp->first][itlat->first].begin(); itlon != indata[itspp->first][itlat->first].end(); itlon++) {
				sppoints += 1;
				}
			}
		cout << sppoints << " points." << endl;
		}

	cout << nspp << " species." << endl;

	mytiles = togrid(indata, 7, 5);

	for (int i = 0; i < mytiles.size(); i++) {
		cout << mytiles[i]->getName() << endl;
		mytiles[i]->prettyPrint();
		delete mytiles[i];
		}

	return 0;
	}