#!/usr/bin/env python3

# Analysis and Corroboration of Key Biodiversity AReas  - Ackbar
# Key Biodiversity Area Boundaries - Kebab

###############################################################################
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
#	along with Ackbar.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

import argparse
import datetime

version =  "0.1"
logfile = ""
argPass = True
today = datetime.datetime.now()
outfileRootDefault = today.strftime("Ackbar_output_%Y%m%d_%H%M%S")
bufferLog = "Admiral Ackbar ver. {0}\nAnalysis executed on {1}\n".format(version, today)

parser = argparse.ArgumentParser(description = 'Ackbar: a Python software to delimit and update Key Biodiversity Areas.')

parser.add_argument('-d', '--distribution_file', required = True, dest = 'distrofile', metavar = '<infile>', action = 'store', help = 'Input distribution file in csv format. See manual for a detailed guideline.')

parser.add_argument('-e', '--evaluation_file', required = True, dest = 'evalfile', metavar = '<infile>', action = 'store', help = 'Input file of IUCN evaluations in csv format. See manual for a detailed guideline.')

parser.add_argument('-k', '--kba_directory', dest = 'kbaDir', metavar = '<directory>', action = 'store', help = 'Path to shapefiles of previously delimited KBA. See manual for a detailed guideline.')

parser.add_argument('-l', '--excluded_areas', dest = 'exclDir', metavar = '<directory>', action = 'store', help = 'Path to shapefiles of areas to exclude from analysis. See manual for a detailed guideline.')

parser.add_argument('-o', '--outfile_root', required = True, dest = 'outfileRoot', metavar = '<outfile_root_name>', action = 'store', help = 'Outfiles name root.')

parser.add_argument('-w', '--cell_width', required = True, dest = 'cellWidth', metavar = '<#>', action = 'store', type = float, help = 'Grid cell width in geographic degrees.')

parser.add_argument('-g', '--cell_height', required = True, dest = 'cellHeight', metavar = '<#>', action = 'store', type = float, help = 'Grid cell height in geographic degrees.')

parser.add_argument('-x', '--long_offset', dest = 'lonOffset', metavar = '<#>', action = 'store', default = 0, type = float, help = 'Longitudinal offset of the W border of the grid in relation to the westernmost point in the infile. Should be a positive value in geographic degrees. Default = 0.')

parser.add_argument('-y', '--lat_offset', dest = 'latOffset', metavar = '<#>', action = 'store', default = 0, type = float, help = 'Latitudinal offset of the N border of the grid in relation to the northernmost point in the infile. Should be a positive value in geographic degrees. Default = 0.')

parser.add_argument('-c', '--cohesion', dest = 'cohesion', metavar = '<#>', default = 0.3, action = 'store', type = float, help = 'Clustering cohesion parameter (0.0--1.0). Default = 0.3.')

parser.add_argument('-v', '--version', action = 'version', version = "Ackbar v. {0}".format(version))

args = parser.parse_args()

print (args)

exit(0)