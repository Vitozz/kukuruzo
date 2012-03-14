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

setup(name='rb-restore-plugin',
      version=get_version(),
      description='Rhythmbox restore plugin',
      long_description = "Restores the played item when Rhythmbox is starting",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      data_files=[  ('/usr/lib/rhythmbox/plugins/restore', ['__init__.py']),
                    ('/usr/lib/rhythmbox/plugins/restore', ['restore.glade']),
                    ('/usr/lib/rhythmbox/plugins/restore', ['restore.rb-plugin']),
                 ],
      )
               
