from setuptools import setup, find_packages
from distutils.core import Extension

with open("README.rst", "r") as fh:
	long_descrip = fh.read()

setup(name = 'Ackbar',
	version = '0.1.1',
	author = 'Nelson R. Salinas',
	author_email = 'nsalinas@humboldt.org.co',
	url = 'https://github.com/nrsalinas/ackbar',
	description = 'A Python program to assess Key Biodiversity Areas (KBA) delimitation.',
	long_description = long_descrip,
	long_description_content_type = "text/x-rst",
	license = 'GNU GPL v. 3',
	keywords = 'biology conservation diversity',
	python_requires = '>=3',
	install_requires = ['fiona','shapely', 'pyproj'],
	ext_modules = [Extension('ackbar_lib.pydata', ['ackbar_lib/pydata.cpp'])],
	scripts = ['ackbar.py'],
	packages = ['ackbar_lib'],
	package_data = {'data':['config.txt', 'plutarchia_occurrences.csv',
		'plutarchia_categories.csv', 'plutarchia_groups.csv']}
	)
