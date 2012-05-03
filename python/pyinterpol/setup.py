#!/usr/bin/env python

from distutils.core import setup
from distutils.command.install_data import install_data

setup(name='pyinerpol',
      version='0.1',
      description='Simple interpolise app',
      long_description = "Simple programm to count interpolation",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      packages = ['pyInterpol'],
      scripts=['pyinterpol'],
      data_files=[('/usr/share/applications', ['pyinterpol.desktop']),
    	          ('/usr/share/pyinterpol', ['interpol.png']),
    	          ('/usr/share/pyinterpol', ['mForm.glade']),
                 ],
      )
               
