from setuptools import setup
from Cython.Build import cythonize

setup(ext_modules=cythonize("ackbar/pydata.pyx", compiler_directives={'language_level' : "3"}))