# docs/conf.py

# -- Path setup --------------------------------------------------------------

import os
import sys

# If your extensions (or modules to document with autodoc) are in
# the project root, add that directory to sys.path here:
#    sys.path.insert(0, os.path.abspath('..'))

# -- Project information -----------------------------------------------------

project = 'SITypes'
author = 'Philip J. Grandinetti'
# The full version, including alpha/beta/rc tags
release = '0.1.0'
# The short X.Y version
version = release

# -- General configuration ---------------------------------------------------

# Sphinx extensions
extensions = [
    'breathe',
]

# Paths that contain templates, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files to ignore.
exclude_patterns = []

# -- Breathe configuration --------------------------------------------------

# Tell Breathe where the Doxygen XML lives (relative to this conf.py).
# This should match OUTPUT_DIRECTORY = doxygen and XML_OUTPUT = xml in Doxyfile,
# resulting in docs/doxygen/xml.
breathe_projects = {
    "SITypes": os.path.abspath(
        os.path.join(os.path.dirname(__file__), "doxygen", "xml")
    ),
}
breathe_default_project = 'SITypes'


# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

# If you have no static files at all (you removed _static), clear this:
html_static_path = []

# ——— Breathe/C domain fixes ———
# Ensure that C files and headers use the C domain in Sphinx/breathe
breathe_domain_by_extension = {
    "c": "c",
    "h": "c",
}
primary_domain = 'c'
