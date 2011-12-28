#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='exaile-tunetopsi-plugin',
      version='0.0.5',
      description='Exaile Tune To Psi Plugin',
      long_description = "Sends Current Track Information to Psi/Psi+ as Tune.",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      data_files=[  ('/usr/share/exaile/plugins/tunetopsi', ['__init__.py']),
                    ('/usr/share/exaile/plugins/tunetopsi', ['PLUGININFO']),
                 ],
      )
               
