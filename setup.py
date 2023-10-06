import os
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

INSTALL_REQUIRES = []

INCLUDE_DIRS = [
    "dubins/include",
]

ext_modules = [
    Pybind11Extension(
        name="dubins",
        cxx_std=14,
        sources=[
            "dubins/src/dubins_pybind.cpp",
            "dubins/src/dubins.cpp"
        ],
        include_dirs=INCLUDE_DIRS,
    ),
]

def read(filename):
    path = os.path.join(os.path.dirname(__file__), filename)
    contents = open(path).read()
    return contents


setup(
    name         = "dubins",
    version      = "1.0.1",
    description  = "Code to generate and manipulate dubins curves",
    long_description = read('README.rst'),
    author       = "Andrew Walker",
    author_email = "walker.ab@gmail.com",
    url          = "http://github.com/AndrewWalker/pydubins",
    license      = "MIT",
    classifiers  = [
        'Development Status :: 4 - Beta',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Natural Language :: English',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
        'Operating System :: POSIX :: Linux',
        'Topic :: Scientific/Engineering :: Mathematics',
    ],
    install_requires = INSTALL_REQUIRES,
    ext_modules  = ext_modules,
)

