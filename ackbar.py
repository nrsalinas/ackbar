#!/usr/bin/env python3

# Analysis and Corroboration of Key Biodiversity AReas  - Ackbar

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
#	along with Ackbar. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

import sys
import os
import shutil
import datetime
import re

import fileio
import pydata
import shapes

version =  "0.1"
logfile = ""
paramPass = True

critmap = {0: "A1a", 1: "A1b", 2: "A1c", 3: "A1d",4 : "A1e", 5: "B1", 6: "B2"}

parameters = {
	"distribution_file" : None,
	"iucn_file" : None,
	"taxonomic_groups_file" : None,
	"taxonomic_assignments_file" : None,
	"kba_species_file" : None,
	"kba_directory" : None,
	"kba_index" : None,
	"exclusion_directory" : None,
	"outfile_root" : None,
	"overwrite_output" : None,
	"cell_size" : None,
	"offset_lat" : None,
	"offset_lon" : None,
	"pop_max_distance": None,
	"eps" : None,
	"iters" : None,
	"max_kba" : None,
	"congruency_factor" : None
	}

today = datetime.datetime.now()
outfileRootDefault = today.strftime("Ackbar_output_%Y%m%d_%H%M%S")
bufferLog = "Ackbar ver. {0}\nAnalysis executed on {1}\n\n".format(version, today)
helloLog = '''
********************    Ackbar ver. {0}    ********************

A Python program to assist the delimitation and update of Key 
Biodiversity Areas.

Usage:

	ackbar.py configuration_file

A more detailed help can be accessed at www.github.com or by typing:

	ackbar.py --help
'''.format(version)

if len(sys.argv) > 2:
	raise IOError('Too many arguments were parsed to Ackbar.')

elif len(sys.argv) == 1:
	print(helloLog)

else:

	if sys.argv[1] == '--help' or sys.argv[1] == '-h':
		# Long help prompt
		pass

	elif os.path.isfile(sys.argv[1]):

		with open(sys.argv[1], 'r') as config:

			# Parse config info into `parameters` dictionary

			for line in config:

				line = line.rstrip()
				line = re.sub(r'#.*', '', line, flags=re.DOTALL)
				line = re.sub(r'^\s+', '', line)
				line = re.sub(r'\s$', '', line)

				if len(line) > 5:

					par_name = re.sub(r'\s*=.*$', '', line, flags=re.DOTALL)
					par_val = re.sub(r'^.*=\s*', '', line, flags=re.DOTALL)

					if par_name and par_val and par_name in parameters:
					
						parameters[par_name] = par_val
		

		## Check presence/absence of parameters
		# Check mandatory params
		for manpar in ["distribution_file", "iucn_file", "cell_size"]:

			if parameters[manpar] is None:

				raise ValueError('Configuration file error: mandatory parameter `{0}` has not been set.'.format(manpar))

		kba_pars = 0

		# Optional parameters
		for kbap in ["kba_species_file", "kba_directory", "kba_index"]:
			if not parameters[kbap] is None:
				kba_pars += 1

		if kba_pars > 0 and kba_pars < 3:

			raise ValueError("Configuration file error: not all the parameters required for including existing KBA were set (`kba_species_file`, `kba_directory`, and `kba_index`). Alternatively, ALL three parameters can be left blank to conduct an analysis without considering previous KBA information.")

		if (not parameters["taxonomic_groups_file"] and parameters["taxonomic_assignments_file"]) or \
			(parameters["taxonomic_groups_file"] and not parameters["taxonomic_assignments_file"]):

			raise ValueError("Configuration file error: only one of the taxonomic groups parameters has been set (`taxonomic_groups_file` and `taxonomic_assignments_file`). Both are required to assess criterion B2. Alternatively, both can be left blanck to conduct an analysis without applying criterion B2.")

		#
		# Check taxonomic group pars are parsed together
		#
		
		
		## Check parsed values are valid

		for fpar in filter(lambda x: re.search(r'_file$', x), parameters.keys()):

			if parameters[fpar] and not os.path.isfile(parameters[fpar]):

				raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` is not a file).'.format(fpar, parameters[fpar]))

		for dpar in filter(lambda x: re.search(r'_directory$', x), parameters.keys()):
		
			if parameters[dpar] and not os.path.isdir(parameters[dpar]):

				raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` is not a directory).'.format(dpar, parameters[dpar]))


		for par_name in ["cell_size", "offset_lat", "offset_lon", "eps", "congruency_factor", "iters", "max_kba", "pop_max_distance"]:

			par_val = parameters[par_name]

			if par_val:

				try:
					par_val = float(par_val)

					if par_val < 0:
						raise ValueError("Configuration file error: parameter `{0}` should be a positive number.".format(par_name))

					if par_name in ["iters", "max_kba"] and par_val % 1 > 0:

						raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` should be an integer).'.format(par_name, par_val))

					if par_name == "cell_size" and par_val > 10:

						raise ValueError("Configuration file error: `cell_size` value seems out of logical or practical range (`{0}`)".format(par_val))

					if par_name == "max_kba" and par_val < 1:

						raise ValueError("Configuration file error: `max_kba` value seems out of practical range (`{0}`)".format(par_val))

					parameters[par_name] = par_val


				except ValueError as te:
					mess = str(te)

					if mess.startswith('could not convert string to float'):
						raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` should be a number).'.format(par_name, par_val))

					else:
						raise


		if type(parameters["overwrite_output"]) == str:
			
			if re.search(r'true', parameters["overwrite_output"], re.I):
				parameters["overwrite_output"] = True

			elif re.search(r'False', parameters["overwrite_output"], re.I):
				parameters["overwrite_output"] = False

			else:
				mss = "\nConfiguration file error: value parsed as `overwrite_output` value is not valid ({0}). Parameter will be set as False.\n".format(parameters["overwrite_output"])
				parameters["overwrite_output"] = False
				bufferLog += mss
				print(mss, file=sys.stderr)

		else:
			parameters["overwrite_output"] = False


		if parameters["outfile_root"] is None:
			parameters["outfile_root"] = outfileRootDefault

		if parameters["offset_lat"] is None:
			parameters["offset_lat"] = 0

		if parameters["offset_lon"] is None:
			parameters["offset_lon"] = 0

		if parameters["eps"] is None:
			parameters["eps"] = 0.2

		if parameters["iters"] is None:
			parameters["iters"] = 1000

		if parameters["max_kba"] is None:
			parameters["max_kba"] = 20

		if parameters["congruency_factor"] is None:
			parameters["congruency_factor"] = 1

		if parameters["pop_max_distance"] is None:
			parameters["pop_max_distance"] = 0

		bufferLog += "Parameters set for the analysis:\n\n"

		for par in parameters:
			#print(par, " = ", parameters[par])
			bufferLog += "{0} = {1}\n".format(par, parameters[par])

	#print(bufferLog)

	### Output file/directory names

	new_trigger_file = parameters["outfile_root"] + "_trigger_spp_previous_KBA.csv"
	sol_dir = parameters["outfile_root"] + "_solution_shapefiles"
	logfile = parameters["outfile_root"] + "_log.txt"

	output_names = [new_trigger_file, sol_dir, logfile]

	### Check output files/directories exists

	if parameters["overwrite_output"] == True:
		for name in output_names:
			if os.path.exists(name):
				if os.path.isfile(name):
					os.remove(name)
				elif os.path.isdir(name):
					shutil.rmtree(name)

	else:
		for name in output_names:
			if os.path.exists(name):
				raise OSError("A file/directory named {0} already exists.".format(name))


	################################################################

	data = fileio.InputData(parameters["distribution_file"])
	data.iucnFile(parameters["iucn_file"])

	bufferLog += "\nNumber of species in distribution file: {0}\n\n".format(len(data.points))
	bufferLog += "\nUnique datapoints per species:\n\n"

	for sp in sorted(data.points):
		bufferLog += "{0}: {1}\n".format(sp, len(data.points[sp]))

	no_points = [x for x in data.iucn if not x in data.points]
	if len(no_points) > 0:
		bufferLog += "\nIUCN file contains {0} species with no data points:\n".format(len(no_points))
		for sp in no_points:
			bufferLog += "\t{0}\n".format(sp)

	no_iucn = [x for x in data.points if not x in data.iucn]
	if len(no_iucn) > 0:
		bufferLog += "\nIUCN file lacks {0} species present in the distribution file:\n".format(len(no_iucn))
		for sp in no_iucn:
			bufferLog += "\t{0}\n".format(sp)
	

	if parameters["taxonomic_groups_file"] and parameters["taxonomic_assignments_file"]:

		data.groupFiles(parameters["taxonomic_assignments_file"], parameters["taxonomic_groups_file"])
		
		no_points = [x for x in data.taxonGroups if not x in data.points]
		if len(no_points) > 0:
			bufferLog += "\nTaxon group assignments file contains {0} species with no data points:\n".format(len(no_points))
			for sp in no_points:
				bufferLog += "\t{0}\n".format(sp)

		no_groups = [x for x in data.points if not x in data.taxonGroups]
		if len(no_groups) > 0:
			bufferLog += "\nTaxon group assignments file lacks {0} present in the distribution file (The analysis will not be executed until you fix this):\n".format(len(no_groups))
			for sp in no_groups:
				bufferLog += "\t{0}\n".format(sp)

		groupAssign = {}
		for x in data.taxonGroups:
			groupAssign[data.taxonGroups[x]['group']] = 0
		miss_groups = [x for x in groupAssign.keys() if not x in data.taxonGroupsInfo.keys()]

		if len(miss_groups) > 0:
			bufferLog += "\nTaxonomic groups missing in the taxonomic groups file:\n"
			for y in miss_groups:
				bufferLog += "\t{0}\n".format(y)
	

	if parameters["kba_species_file"] and parameters["kba_directory"] and parameters["kba_index"]:

		old_kbas = shapes.KBA(parameters["kba_directory"], parameters["kba_index"])
		old_kbas.spp_inclusion(data)
		old_kbas.new_spp_table(new_trigger_file)


	tiles = data.getTiles(parameters["cell_size"], offsetLat = parameters["offset_lat"], 
		offsetLon = parameters["offset_lon"], maxDist = parameters["pop_max_distance"])


	if parameters["taxonomic_groups_file"] and parameters["taxonomic_assignments_file"]:

		#
		# Check if data.groupDict and data.spp2groupDict are appropriate dicts
		#

		mysols = pydata.metasearchAlt(tiles, parameters["eps"], parameters["iters"], 
			parameters["max_kba"], parameters["congruency_factor"], data.groupDict, 
			data.spp2groupDict)

	else:

		mysols = pydata.metasearchAlt(tiles, parameters["eps"], parameters["iters"], 
			parameters["max_kba"], parameters["congruency_factor"])

	for ig, group in enumerate(mysols):

		bufferLog += "\nSolution group {0}\n".format(ig)

		for isol, sol in enumerate(group):

			bufferLog += "\n\tSolution {0}:\n".format(isol)

			for spinx in sorted(sol.spp2crit, key = lambda x : tiles[x].getName()):

				bufferLog += "\t\t{0}: ".format(tiles[spinx].getName())
				tcrits = map(lambda x:  critmap[x], sol.spp2crit[spinx])
				tcritsstr = " ".join(map(str, tcrits))
				bufferLog += " {0}\n".format(tcritsstr)


	if len(mysols) > 0 and len(mysols[0]) > 0:

		shapes.solution2shape(mysols, data, sol_dir)

	with open(logfile, "w") as logh:
		logh.write(bufferLog)

exit(0)