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

import sys
import datetime

version =  "0.1"
logfile = ""
argPass = True
today = datetime.datetime.now()
outfileRootDefault = today.strftime("Ackbar_output_%Y%m%d_%H%M%S")
bufferLog = "Admiral Ackbar ver. {0}\nAnalysis executed on {1}\n".format(version, today)


if len(sys.argv) > 2:
	raise IOError('Too many arguments were parsed to Ackbar.')
elif len(sys.argv) == 1:
	raise IOError('Configuration file should be parsed to Ackbar.')
else:
	with open(sys.argv[1], 'r') as config:
		for line in config:
			if len(line) > 5:
				print(line.rstrip())


exit(0)