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

# To execute test, from the package hoe directory:
# python -m unittest -v

import unittest
import os
from ackbar_lib import fileio

pydata_imported = False

try:
	from ackbar_lib import pydata
	pydata_imported = True

except:
	pass




class Test_data(unittest.TestCase):

	def test_import_pydata(self):
		self.assertTrue(pydata_imported, "Module pydata could not be imported.")

	def test_Mesh_init(self):
		ame = pydata.Meshpy(5, "A name", "CR")
		self.assertIsInstance(ame, pydata.Meshpy, "pydata.Meshpy could not be instantiated.")

	def test_Mesh_values(self):
		ame = pydata.Meshpy(5, "A name", "CR")
		ame.setValue(1, 15)
		self.assertTrue(ame.getValue(1) == 15, "Value setter and/or getter methods of pydata.Meshpy broken.")

	def test_Mesh_category_getter(self):
		ame = pydata.Meshpy(5, "A name", "CR")
		self.assertTrue(ame.getThreatStatus() == "CR", "IUCN category getter method of pydata.Meshpy broken.")

	def test_Mesh_subcriteria(self):
		ame = pydata.Meshpy(5, "A name", "CR")
		ame.newThreatSubcriteriaA(2)
		self.assertTrue(ame.getThreatSubcriteriaA() == [2], "IUCN subcriteria setter and/or getter methods of pydata.Meshpy broken.")


class Test_exemplary_data(unittest.TestCase):

	def test_sample_data_location(self):
		root = os.getcwd().rstrip("test")
		thecsv = root + "/data/plutarchia_occurrences.csv"
		self.assertTrue(os.path.isfile(thecsv), "Sample distribution file not found.")		

		thecsv = thecsv = root + "/data/plutarchia_categories.csv"
		self.assertTrue(os.path.isfile(thecsv), "Sample categories file not found.")

class Test_fileio(unittest.TestCase):

	def test_read_distribution_csv(self):
		thecsv = os.getcwd().rstrip("test")
		thecsv += "/data/plutarchia_occurrences.csv"
		thedata = fileio.InputData(thecsv)

		self.assertIsInstance(thedata, fileio.InputData, "fileio.InputData could not be instantiated")

		self.assertEqual(len(thedata.points), 10, "fileio.InputData could not read the information of all taxa.")

		counter = 0
		for tax in thedata.points:
			counter += len(thedata.points[tax])

		self.assertEqual(counter, 177, "fileio.InputData could not read all datapoints.")

	
	def test_read_categorie_csv(self):
		pass

if __name__ == '__main__':
    unittest.main()