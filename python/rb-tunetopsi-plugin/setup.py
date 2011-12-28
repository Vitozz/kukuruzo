#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='rb-tunetopsi-plugin',
      version='0.4',
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
               
