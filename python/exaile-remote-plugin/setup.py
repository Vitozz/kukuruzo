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

setup(name='exaile-remote-plugin',
      version=get_version(),
      description='Control Exaile via tray icons',
      long_description = "Control Exaile via tray icons",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      data_files=[  ('/usr/share/exaile/plugins/remote', ['__init__.py']),
                    ('/usr/share/exaile/plugins/remote', ['Options.py']),
                    ('/usr/share/exaile/plugins/remote', ['PLUGININFO']),
                    ('/usr/share/exaile/plugins/remote', ['remote.ui']),
                 ],
      )
               
