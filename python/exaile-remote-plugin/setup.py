#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='exaile-remote-plugin',
      version='0.2.2',
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
               
