#include "sim.hpp"

vector<Mesh*> overTiles (int size, int tilesxtype, int staProb) {
	// staProp should be int 0-99
	vector<Mesh*> out;
	int index, nindex, midx, prob;
	double cellValue;

	if (size % 2 == 0) {

		for (int m = 0; m < 3; m++) {

			for (int r = 0; r < tilesxtype; r++) {

				out.push_back(new Mesh(size * size));
				midx = (m * tilesxtype) + r;

				for (int irow = 0; irow < size; irow++) {

					for (int icol = 0; icol < size; icol++) {

						prob = rand() % 100;
						index = irow * size + icol;

						//neighborhood
						if (irow > 0) {
							nindex = (irow - 1) * size + icol;
							out[midx]->linkNeighs(index, nindex);
							}

						if (irow < (size - 1)) {
							nindex = (irow + 1) * size + icol;
							out[midx]->linkNeighs(index, nindex);
							}

						if (icol > 0) {
							nindex = irow * size + icol - 1;
							out[midx]->linkNeighs(index, nindex);
							}

						if (icol < (size -1)){
							nindex = irow * size + icol + 1;
							out[midx]->linkNeighs(index, nindex);
							}

						if (staProb >= prob){

							if (m == 0) {
								// extent = size ** 2 / 2
								if (irow < (size / 2)) {
									cellValue = 1.0 / (size * size / 2.0);
									out[midx]->setValue(index, cellValue);
									}

								} else if (m == 1) {
									// extent = size ** 2 / 2
									if (icol < (size / 2)) {
										cellValue = 1.0 / (size * size / 2.0);
										out[midx]->setValue(index, cellValue);
										}

									} else {
										// extent = size ** 2 - 2 * size + 1
										if ((irow > 0) && (icol > 0)) {
											cellValue = 1.0 / (size * size - 2.0 * size + 1.0);
											out[midx]->setValue(index, cellValue);
											}
										}
							}
						}
					}
				}
			}
		}

	return out;

	}



vector<Mesh*> nestedTiles (int size, int tilesxtype, int staProb) {

	vector<Mesh*> out;
	int index, nindex, midx, prob;

	if (size % 3 == 0) {

		for (int m = 0; m < 3; m++) {

			for (int r = 0; r < tilesxtype; r++) {

				out.push_back(new Mesh(size * size));
				midx = (m * tilesxtype) + r;

				for (int irow = 0; irow < size; irow++) {

					for (int icol = 0; icol < size; icol++) {

						prob = rand() % 100;
						index = irow * size + icol;

						//neighborhood
						if (irow > 0) {
							nindex = (irow - 1) * size + icol;
							out[midx]->linkNeighs(index, nindex);
							}

						if (irow < (size - 1)) {
							nindex = (irow + 1) * size + icol;
							out[midx]->linkNeighs(index, nindex);
							}

						if (icol > 0) {
							nindex = irow * size + icol - 1;
							out[midx]->linkNeighs(index, nindex);
							}

						if (icol < (size -1)){
							nindex = irow * size + icol + 1;
							out[midx]->linkNeighs(index, nindex);
							}

						if (staProb >= prob){

							if (m == 0) {

								if ((irow < (size / 3)) && (icol < (size / 3))) {
									out[midx]->setValue(index, 1);
									}

								} else if (m == 1) {

									if ((irow < (2 * size / 3)) && (icol < (2 * size / 3))) {
										out[midx]->setValue(index, 1);
										}

									} else {
										out[midx]->setValue(index, 1);
										}
							}
						}
					}
				}
			}
		}

	return out;

	}
