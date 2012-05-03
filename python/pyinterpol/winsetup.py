#!/usr/bin/env python
# winsetup.py
from distutils.core import setup
import py2exe#, gio
#import glob

opts = {
    "py2exe": {
        "includes": "cairo, pango, pangocairo, atk, gobject",
        }
    }

setup(
    name = "pyInterpol",
    description = "Simple interpolise app",
    long_description = "Simple programm to count interpolation",
    version = "0.1",
    author="Vitaly Tonkacheyev",
    author_email="thetvg@gmail.com",
    windows = [
        {"script": "pyinterpol.py",
        "icon_resources": [(1, "f(x).ico")]
        }
    ],
    packages = ["pyInterpol"],
    options=opts,
    data_files=["mForm.glade", "interpol.png",],
)