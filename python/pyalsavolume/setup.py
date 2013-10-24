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

setup(name='pyalsavolume',
      version=get_version(), 
      description='Tray ALSA volume changer',
      long_description = "Simple programm to change the volume of one of the ALSA mixer from the system tray",
      author='Vitaly Tonkacheyev',
      author_email='thetvg@gmail.com',
      url='http://sites.google.com/site/thesomeprojects/',
      maintainer='Vitaly Tonkacheyev',
      maintainer_email='thetvg@gmail.com',
      license = "GNU GPLv3",
      packages = ['pyAlsaVolume'],
      scripts=['pyalsavolume'],
      data_files=[('/usr/share/applications', ['pyalsavolume.desktop']),
                    ('/usr/share/pyalsavolume/icons', ['icons/volume.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/volume_ico.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon0.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon20.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon40.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon60.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon80.png']),
                    ('/usr/share/pyalsavolume/icons', ['icons/tb_icon100.png']),
                    ('/usr/share/pyalsavolume/lang', ['lang/en.lng']),
                    ('/usr/share/pyalsavolume/lang', ['lang/ru.lng']),
                    ('/usr/share/pyalsavolume/lang', ['lang/ua.lng']),
                    ('/usr/share/pyalsavolume', ['pySettingsFrame.glade']),
                    ('/usr/share/pyalsavolume', ['pySliderFrame.glade']),
                 ],
      )
               
