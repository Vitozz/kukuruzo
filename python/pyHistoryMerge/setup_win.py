#!/usr/bin/env python
from distutils.core import setup
import py2exe
import glob

opts = {
    "py2exe": {
        "includes": "PyQt4, sip",
        }
    }

setup(name='psi-history-merger',
      version='0.1',
      description='Psi/Psi+ history merger',
      long_description = "Merges history file of Psi/Psi+ jabber client",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      windows = [
        {"script": "hismerger",
        "icon_resources": [(1, "icon16.ico"),(2, "icon32.ico"),(3, "icon48.ico")],
        }
      ],
      options=opts,
      )
               
