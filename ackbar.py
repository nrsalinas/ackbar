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
#	along with Ackbar.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

import sys
import datetime
import re

version =  "0.1"
logfile = ""
paramPass = True
parameters = {}
today = datetime.datetime.now()
outfileRootDefault = today.strftime("Ackbar_output_%Y%m%d_%H%M%S")
bufferLog = "Ackbar ver. {0}\nAnalysis executed on {1}\n".format(version, today)
helloLog = '''
********	Ackbar ver. {0}	********

A Python program to update and delimit Key Biodiversity Areas.

Usage:

	ackbar.py configuration_file

A more detailed help can be accessed at www.chancro.com or by typing:

	ackbar.py --help
'''.format(version)

if len(sys.argv) > 2:
	raise IOError('Too many arguments were parsed to Ackbar.')

elif len(sys.argv) == 1:
	print(helloLog)

else:

	with open(sys.argv[1], 'r') as config:

		for line in config:

			line = re.sub(r'#.*', '', line, flags=re.DOTALL)
			line = re.sub(r'^\s+', '', line)
			line = re.sub(r'\s$', '', line)

			if len(line) > 5:

				par = re.split(r':', line)[0]
				par = re.sub(r'\s', '', par)

				if par in 









exit(0)