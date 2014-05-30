# -*- coding: utf-8 -*-
from distutils.core import setup
import py2exe

setup(name='compile-plugins',
      version='0.2',
      description='Psi-plus plugins compiler script',
      long_description = "Tool to build psi-plus plugins",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      console = ["compile-plugins.py"],
)