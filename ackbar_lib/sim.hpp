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


#include <math.h>
#include "data.hpp"

#ifndef SIM_HPP
#define SIM_HPP

vector<Mesh*> nestedTiles (int size, int tilesxsize, int staProb = 90);

vector<Mesh*> overTiles (int size, int tilesxsize, int staProb = 90);

vector<Mesh*> overTiles2 (int size, int tilesxtype, int staProb = 90);

#endif