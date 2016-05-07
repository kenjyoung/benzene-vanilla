from distutils.core import setup, Extension
import numpy

# define the extension module
networkEvaluator = Extension('networkEvaluator', sources=['networkEvaluator.cpp'])

# run the setup
setup(ext_modules=[networkEvaluator], include_dirs=[numpy.get_include()])