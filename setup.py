from setuptools import setup, find_packages
from distutils.core import Extension

setup(name = 'Ackbar',
	version = '0.1',
	author = 'Nelson R. Salinas',
	author_email = 'nsalinas@humboldt.org.co',
	url = 'https://github.com/nrsalinas/ackbar',
	description = 'A Python program to assess Key Biodiversity Areas (KBA) delimitation.',
	license = 'GNU GPL v. 3',
	keywords = 'biology conservation diversity protected areas',
	python_requires = '>=3',
	install_requires = ['fiona','shapely'],
	ext_modules = [Extension('ackbar_lib.pydata', ['ackbar_lib/pydata.cpp'])],
	scripts = ['ackbar.py'],
	packages = ['ackbar_lib'],
	package_data = {'data':['config.txt']}
	)
