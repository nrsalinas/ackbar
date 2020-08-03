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
import datetime
import re

version =  "0.1"
logfile = ""
paramPass = True

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
	"cell_size" : None,
	"offset_lat" : None,
	"offset_lon" : None,
	"eps" : None,
	"iters" : None,
	"max_kba" : None,
	"congruency_factor" : None
	}

today = datetime.datetime.now()
outfileRootDefault = today.strftime("Ackbar_output_%Y%m%d_%H%M%S")
bufferLog = "Ackbar ver. {0}\nAnalysis executed on {1}\n\n".format(version, today)
helloLog = '''
********	Ackbar ver. {0}	********

A Python program to update and delimit Key Biodiversity Areas.

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

			for line in config:

				line = line.rstrip()
				line = re.sub(r'#.*', '', line, flags=re.DOTALL)
				line = re.sub(r'^\s+', '', line)
				line = re.sub(r'\s$', '', line)

				if len(line) > 5:

					par_name = re.sub(r'\s*=.*', '', line)
					par_val = re.sub(r'.*=\s*', '', line)

					if par_name and par_val and par_name in parameters:
					
						if re.search(r'_file$', par_name):

							if not os.path.isfile(par_val):

								raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` is not a file).'.format(par_name, par_val))

						if re.search(r'_directory$', par_name):

							if not os.path.isdir(par_val):

								raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` is not a directory).'.format(par_name, par_val))

						if par_name in ["cell_size", "offset_lat", "offset_lon", "eps", "congruency_factor", "iters", "max_kba"]:

							try:
								par_val = float(par_val)

							except ValueError as te:
								mess = str(te)

								if mess.startswith('could not convert string to float'):
									raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` should be a number).'.format(par_name, par_val))

								else:
									raise

							if par_name in ["iters", "max_kba"]:

								if par_val % 1 > 0:

									raise ValueError('Configuration file error: parameter `{0}` has not a valid value (`{1}` should be an integer).'.format(par_name, par_val))

							if par_name == "cell_size" and ((par_val < 0) or (par_val > 10)):

								raise ValueError("Configuration file error: `cell_size` value seems out of logical or practical range (`{0}`)".format(par_val))


						parameters[par_name] = par_val
		

		# Check mandatory params
		for manpar in ["distribution_file", "iucn_file", "cell_size"]:

			if parameters[manpar] is None:

				raise ValueError('Configuration file error: mandatory parameter `{0}` has not been set.'.format(manpar))

		kba_pars = 0

		for kbap in ["kba_species_file", "kba_directory", "kba_index"]:
			if not parameters[kbap] is None:
				kba_pars += 1

		if kba_pars > 0 and kba_pars < 3:

			raise ValueError("Configuration file error: not all the parameters required for including existing KBA were set (`kba_species_file`, `kba_directory`, and `kba_index`). Alternatively, ALL three parameters can be left blank to conduct an analysis without considering previous KBA information.")

		if (parameters["taxonomic_groups_file"] is None and not parameters["taxonomic_groups_file"] is None) or \
			(not parameters["taxonomic_groups_file"] is None and parameters["taxonomic_groups_file"] is None):

			raise ValueError("Configuration file error: only one of the taxonomic groups parameters has been set (`taxonomic_groups_file` and `taxonomic_assignments_file`). Both are required to assess criterion B2. Alternatively, both can be left blanck to conduct an analysis without applying criterion B2.")

		# optional parameters
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

		bufferLog += "Parameters set for the analysis:\n\n"

		for par in parameters:
#			print(par, " = ", parameters[par])
			bufferLog += "{0} = {1}\n".format(par, parameters[par])

		print(bufferLog)




exit(0)