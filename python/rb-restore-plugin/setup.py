#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='rb-restore-plugin',
      version='0.9.3',
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
               
