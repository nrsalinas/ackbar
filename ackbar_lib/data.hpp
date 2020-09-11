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
##############################################################################*/

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

#ifndef DATA_HPP
#define DATA_HPP

typedef unsigned int uint;


class Mesh {

	public:

		Mesh();
		Mesh(int size, string inname = "NN", string threatCat = "LC");
		~Mesh();

		void setName(string newName);
		int getSize();
		string getName();
		void setRange(double newRange);
		double getRange();
		bool isNull();
		void nullMe();

		void setValue(int index, double value);
		double getValue(int index);

		void resetNeighborhood();
		void linkNeighs(int indexA, int indexB);
		vector<vector<int> > getNeighborhood();
		vector<int> getCellNeighs(int index);
		void neighsFromList(vector<vector<int> > origNeighs);

		void setThreatStatus(string newStatus);
		string getThreatStatus();
		void newThreatSubcriteriaA(int subcri);
		vector<int> getThreatSubcriteriaA();
		
	
		Mesh * copy();
		void randomize();

	private:

		int geometry; // 0 = square, 1 = hexagonal, 2 = triangular
		string name;
		double range;
		vector< vector<int> > neighborhood; 	    
		string threatStatus; // 'CR', 'EN', or 'VU'
		vector<int> threatSubcriteriaA; // 1, 2, 3, or 4
		//vector<Cell*> cellColl;
		vector<double> values;
		double means[2];
		double stadevs[2];

};


class Solution : public Mesh {

	public:

		Solution();
		Solution(int size);
		Solution(Mesh * mother);
		~Solution();
		int critA;
		int critB;
		int score;
		double ndmScore;
		double aggrScore;
		int extent;
		int origin;

		map<int, double> spp2extent;
		map<int, vector<int> > spp2crit;
		/* keys are indexes in observations vector. Values are vectors integers that 
		associates to KBA criteria. Code options are:
			0 = A1a
			1 = A1b
			2 = A1c
			3 = A1d
			4 = A1e
			5 = B1
			6 = B2*/ 
		map<int, double> sppAreas;

};


#endif