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


#include "sim.hpp"

vector<Mesh*> overTiles2 (int size, int tilesxtype, int staProb) {
	// staProp should be int 0-99
	vector<Mesh*> out;
	int index, nindex, midx, prob;
	double cellValue;

	if ((size % 2 == 0) && (size >= 6)) {

		for (int m = 0; m < 2; m++) {

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
								
								if ((irow < (size / 2)) && (icol < (size / 2))) {

									if ((r % 4 == 0) && (irow > 0) && (icol > 0)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 1) && (irow < (size / 2) - 1) && (icol < (size / 2) - 1)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 2) && (irow > 0) && (icol < (size / 2) - 1)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 3) && (irow < (size / 2) - 1) && (icol > 0)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

								}

							} else if (m == 1) {
								
								if ((irow >= (size / 2)) && (icol >= (size / 2))) {
									
									if ((r % 4 == 0) && (irow > size / 2) && (icol > size / 2)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 1) && (irow < size - 1) && (icol < size - 1)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 2) && (irow > size / 2) && (icol < size - 1)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}

									if ((r % 4 == 3) && (irow < size - 1) && (icol > size / 2)) {
										cellValue = 1.0 / pow((size / 2.0) - 1.0, 2.0);
										out[midx]->setValue(index, cellValue);
									}
								}

						/*	} else {
								// extent = size ** 2 - 2 * size + 1
								if ((irow > 0) && (icol > 0)) {
									cellValue = 1.0 / (size * size - 2.0 * size + 1.0);
									out[midx]->setValue(index, cellValue);
								} */
							} 
						}
					}
				}
			}
		}
	}

return out;

}


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
