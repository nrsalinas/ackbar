#############
Ackbar v. 0.1
#############

A Python program to assess Key Biodiversity Areas (KBA) delimitation.
The KBA standard requires detailed information from multiple sources in order
to take a practical decision about the boundaries of a new KBA.
This program only considers biological data (geographic occurrences of species) 
to suggests and rank areas where a thorough assessment should be conducted.
The output is a set of shapefiles of such areas. 

*************
Documentation
*************

Detailed documentation (rationale, installation, and usage) is hosted at the 
github `wiki <https://github.com/nrsalinas/ackbar/wiki>`_ of the project.

*************
Requirements
*************

- A Python 3 interpreter.
- C compiler.
- Shapely.
- Fiona.

*************
Installation
*************

Ackbar can be installed through pip:::

	pip install ackbar

*****
Usage
*****

All parameters are parsed through a configuration file, which should be the sole
argument:::

	ackbar.py config.txt

Config file parameters are fully explained in the project 
`wiki <https://github.com/nrsalinas/ackbar/wiki>`_ page.

*******
License
*******

GNU General Public License v. 3.

*******
Contact
*******

Nelson R. Salinas
Insituto de Investigación de Recursos Biológicos Alexander von Humboldt
nsalinas@humboldt.org.co   
