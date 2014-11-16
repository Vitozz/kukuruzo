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

setup(name='pysshclient',
      version=get_version(),
      description='pySSHClient package',
      long_description = "Simple program, that provides a graphical interface to connect to a remote directory using SSHFS. It is written on Python + PyGTK",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      packages = ['pySSHClient'],
      scripts=['pysshclient'],
      data_files=[('/usr/share/applications', ['pysshclient.desktop']),
                        ('/usr/share/pysshclient', ['ssh_icon.png']),
                        ('/usr/share/pysshclient', ['ssh_icon_24.png']),
                        ('/usr/share/pysshclient', ['ssh_icon_48.png']),
                        ('/usr/share/pysshclient', ['mainform.glade']),
                 ],
      )              
