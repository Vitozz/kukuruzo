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

setup(name='rb-tunetopsi-plugin',
      version=get_version(),
      description='Rhythmbox tune to Psi plugin',
      long_description = "Sends tune information to Psi or Psi+ jabber client",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      data_files=[  ('/usr/lib/rhythmbox/plugins/tunetopsi', ['__init__.py']),
                    ('/usr/lib/rhythmbox/plugins/tunetopsi', ['tunetopsi.rb-plugin']),
                 ],
      )
               
