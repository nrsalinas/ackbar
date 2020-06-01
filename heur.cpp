#include <iostream>
#include <vector>
#include <map>
#include "Data.cpp"
#include "search.cpp"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::map;

int main(){

	srand((unsigned) time(0));

	int obsSize = 10;
	int rows = 5;
	int cols = 6;
	int tot = rows * cols;
	vector<Mesh*> observations;
	map <int, vector<int>> clusters;
	//vector<int> clusters = {0, 0, 1, 1, 1, 2, 2};
	clusters[0] = {0, 1};
	clusters[1] = {2, 3, 4};
	clusters[2] = {5, 6, 7};
	clusters[3] = {8, 9};

	for (int o = 0; o < obsSize; o++){
		observations.push_back(new Mesh(rows * cols));
		for (int r = 0; r < rows; r++){

			for (int c = 0; c < cols; c++) {

				if (o <= 1) {
					observations[o]->setThreatStatus("CR");
					observations[o]->newThreatSubcriteriaA(3);
					if ((r < 2) & (c < 2)){
						observations[o]->setValue(((r * cols) + c), 0.25);
					} else {
						observations[o]->setValue(((r * cols) + c), 0);
					}

				} else if (o <= 4) {
					observations[o]->setThreatStatus("EN");
					observations[o]->newThreatSubcriteriaA(1);
					if ((r > 2) & (c > 2)){
						observations[o]->setValue(((r * cols) + c), 0.1667);
					} else {
						observations[o]->setValue(((r * cols) + c), 0);
					}

				} else if (o <= 7) {
					observations[o]->setThreatStatus("VU");
					observations[o]->newThreatSubcriteriaA(2);
					if (((r < 2) & (c < 2)) || ((r > 2) & (c > 3))) {
						observations[o]->setValue(((r * cols) + c), 0.25);
					} else {
						observations[o]->setValue(((r * cols) + c), 0);
					}

				} else {
					observations[o]->setThreatStatus("VU");
					observations[o]->newThreatSubcriteriaA(2);
					if ( ((r <= 2) & (c >= 2)) || ((r >= 2) & (c <= 2)) ){
						observations[o]->setValue(((r * cols) + c), 0.05);
					} else {
						observations[o]->setValue(((r * cols) + c), 0);
					}
				}



				int thiscell = (r * cols) + c;
				vector <int> nr;
				vector <int> nc;

	//			cout << "Row " << r << " Col " << c << " Cell " << thiscell << endl;

				if (r > 0){
					nr.push_back(r-1);
				}
				if (r < (rows-1)){
					nr.push_back(r+1);
				}
				if (c > 0){
					nc.push_back(c-1);
				}
				if (c < (cols-1)){
					nc.push_back(c+1);
				}
				//nr.push_back(r);
				//nc.push_back(c);


				for (int m = 0; m < nr.size(); m++){
					int anothercell = (nr[m] * cols) + c;
	//				cout << "   Row " << nr[m] << " Col " << c << " Cell " << anothercell << endl;
					observations[o]->linkNeighs(thiscell, anothercell);
				}
				for (int n = 0; n < nc.size(); n++){
					int anothercell = (r * cols) + nc[n];
	//				cout << "   Row " << r << " Col " << nc[n] << " Cell " << anothercell << endl;
					observations[o]->linkNeighs(thiscell, anothercell);
				}

			}

		}
	}

	//vector<vector<int>> theNeighs = observations[0]->getNeighborhood();


	for (int o = 0; o < obsSize; o++){
		cout << o << ": ";
		observations[o]->prettyPrint();
	}
	cout << endl;


	vector<SolutionB*> solpop = dropSearch(clusters, observations, 50, 10);

/*	vector<SolutionB*> solpop;
	for (int p = 0; p < observations.size(); p++){
		solpop.push_back(new SolutionB(tot));
		solpop[p]->randomize();
		perturb(solpop[p], observations[p], 0.33);
		perturbDiff(solpop[p], 0.33);
		fitness(solpop[p], observations, 0.2);
		//cout << solpop[p]->score << endl;
		}

	sortSols(solpop, 0, (solpop.size()-1));
*/

//	solpop = dropSearch(clusters, observations, 1, 5);


/*	int popSize = 10;
	vector<Solution> solpop;
	for (int p = 0; p < popSize; p++){
		Solution thisSol;
		thisSol.solMesh = observations[0]->copy();
		solpop.push_back(thisSol);
		solpop[p].solMesh->randomize();
		solpop[p].solMesh->prettyPrint();
	}
	cout << endl;
*/

	

	for (int i = 0; i < solpop.size(); i++)	{
		solpop[i]->prettyPrint();
	}

	for (int i = 0; i < solpop.size(); i++){
		delete solpop[i];
	}

	for (int i = 0; i < observations.size(); i++){
		delete observations[i];
	}




}