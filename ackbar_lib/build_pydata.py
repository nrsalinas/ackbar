from setuptools import setup
from Cython.Build import cythonize

setup(ext_modules=cythonize("ackbar_lib/pydata.pyx", compiler_directives={'language_level' : "3"}))