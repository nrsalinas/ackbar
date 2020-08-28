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
Github `wiki <https://github.com/nrsalinas/ackbar/wiki>`_ of the project.

*************
Requirements
*************

- Python 3 interpreter.
- C++ compiler.
- `Shapely <https://pypi.org/project/Shapely/>`_.
- `Fiona <https://pypi.org/project/Fiona/>`_.
- Atention to detail.

*************
Installation
*************

Ackbar can be installed through pip::

	pip install ackbar

*****
Usage
*****

All parameters are parsed through a configuration file, which should be the sole
argument::

	ackbar.py config.txt

Config file parameters are fully explained in the project 
`wiki <https://github.com/nrsalinas/ackbar/wiki>`_ page.

*********************
License
*********************

Copyright 2020 Nelson R. Salinas

Ackbar is available under the GNU General Public License version 3. See LICENSE.md
for more information. 


*******
Contact
*******

| Nelson R. Salinas  
| Instituto de Investigación de Recursos Biológicos Alexander von Humboldt  
| nsalinas@humboldt.org.co   
