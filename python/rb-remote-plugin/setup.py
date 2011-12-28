#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='rb-remote-plugin',
      version='0.3',
      description='Rhythmbox remote plugin',
      long_description = "Adds icons to tray to control player",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      data_files=[  ('/usr/lib/rhythmbox/plugins/remote', ['__init__.py']),
                    ('/usr/lib/rhythmbox/plugins/remote', ['plugin.glade']),
                    ('/usr/lib/rhythmbox/plugins/remote', ['remote.rb-plugin']),
                 ],
      )
               
