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

# To execute test, from the package home directory:
# python -m unittest -v

import unittest
import os


from ackbar_lib import fileio, shapes

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

		self.assertEqual(len(thedata.points), 11, "fileio.InputData could not read the information of all taxa.")

		counter = 0
		for tax in thedata.points:
			counter += len(thedata.points[tax])

		self.assertEqual(counter, 178, "fileio.InputData could not read all datapoints.")

	
	def test_read_categories_csv(self):
		root = os.getcwd().rstrip("test")
		thecsv = root + "/data/plutarchia_occurrences.csv"
		thecat = root + "/data/plutarchia_categories.csv"
		thedata = fileio.InputData(thecsv)
		thedata.iucnFile(thecat)

		iucn_data = {'Plutarchia coronaria': {'category': 'VU', 'subcritA': []},
			'Plutarchia dasyphylla': {'category': 'EN', 'subcritA': []},
			'Plutarchia dichogama': {'category': 'EN', 'subcritA': []},
			'Plutarchia dolos': {'category': 'LC', 'subcritA': []},
			'Plutarchia falsa': {'category': 'EN', 'subcritA': [2]},
			'Plutarchia guascensis': {'category': 'EN', 'subcritA': []},
			'Plutarchia minor': {'category': 'EN', 'subcritA': []},
			'Plutarchia miranda': {'category': 'NT', 'subcritA': []},
			'Plutarchia monantha': {'category': 'VU', 'subcritA': []},
			'Plutarchia pubiflora': {'category': 'EN', 'subcritA': []},
			'Plutarchia rigida': {'category': 'VU', 'subcritA': []}}

		self.assertEqual(thedata.iucn, iucn_data, "IUCN categories could not be processed.")


	def test_groups(self):
		root = os.getcwd().rstrip("test")
		thecsv = root + "/data/plutarchia_occurrences.csv"
		group_assign = root + "/data/plutarchia_groups.csv"
		group_info = root + "/data/plutarchia_groups_info.csv"
		thedata = fileio.InputData(thecsv)
		thedata.groupFiles(group_assign, group_info)
		thedata.groups2search()

		intdictspp = {0: 0, 1: 0, 2: 0, 3: 0, 4: 0, 5: 0, 6: 0, 7: 0, 8: 0, 9: 0, 10: 0}
		intdictgr  = {0: (10000, 2)}

		self.assertEqual(thedata.spp2groupDict, intdictspp, "Group membership file could not process correctly.")
		self.assertEqual(thedata.groupDict, intdictgr, "Group information file could not process correctly.")


class Test_shapes(unittest.TestCase):

	def test_KBA_instance(self):

		theshp = os.getcwd().rstrip("test")
		theshp += "/data/col_center.shp"
		mykba = shapes.KBA(theshp, "Index")

		self.assertIsInstance(mykba, shapes.KBA, "shapes.KBA could not be instantiated.")
		

if __name__ == '__main__':
    unittest.main()