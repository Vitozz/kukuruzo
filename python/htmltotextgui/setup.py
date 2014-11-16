#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data
import os

def get_version ():
      f = open("version.txt", "r")
      f.seek(0)
      lines = f.read()
      f.close()
      return "".join(lines).split("\n")[0]

setup(name='htmltotextgui',
      version=get_version(),
      description='HTML To TXT File Converter',
      long_description = "Converts ebook-files from HTML to TXT format",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      scripts=['htmltotextgui'],
      data_files=[('/usr/share/applications', ['htmltotextgui.desktop']),
                    ('/usr/share/htmltotextgui', ['icon24.png']),
                    ('/usr/share/htmltotextgui', ['icon48.png']),
                    ('/usr/share/htmltotextgui', ['icon24.ico']),
                    ('/usr/share/htmltotextgui', ['main_win.glade']),
                 ],
      )
