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

setup(name='psi-history-merger',
      version=get_version(), 
      description='Psi/Psi+ history merger',
      long_description = "Merges history file of Psi/Psi+ jabber client",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      license = "GNU GPLv3",
      packages = ['historyMerge'],
      scripts=['hismerger'],
      data_files=[('/usr/share/applications', ['psihistorymerger.desktop']),
                    ('/usr/share/psihismerger', ['icon16.png']),
                    ('/usr/share/psihismerger', ['icon32.png']),
                    ('/usr/share/psihismerger', ['icon48.png']),
                 ],
      )
               
