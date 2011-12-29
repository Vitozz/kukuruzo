#!/usr/bin/env python
from distutils.core import setup
import py2exe
import glob

opts = {
    "py2exe": {
        "includes": "cairo, pango, pangocairo, atk, gobject, chardet",
        }
    }

setup(name='htmltotextgui',
      version='0.1',
      description='HTML To TXT File Converter',
      long_description = "Converts ebook-files from HTML to TXT format",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      windows = [
        {"script": "htmltotextgui",
        "icon_resources": [(1, "icon24.ico"),(2, "icon32.ico"),(3, "icon48.ico")]
        }
      ],
      options=opts,
      data_files=['icon24.ico',
                  'icon48.ico',
                  'main_win.glade',
                 ],
      )
               
