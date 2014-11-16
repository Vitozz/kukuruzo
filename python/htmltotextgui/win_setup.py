#!/usr/bin/env python
from distutils.core import setup
import py2exe
import glob
import os

def get_version ():
      f = open("version.txt", "r")
      f.seek(0)
      lines = f.read()
      f.close()
      return "".join(lines).split("\n")[0]

opts = {
    "py2exe": {
        "includes": "cairo, pango, pangocairo, atk, gobject, chardet",
        }
    }

setup(name='htmltotextgui',
      version=get_version(),
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
               
