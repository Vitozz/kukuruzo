#!/usr/bin/env python
#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Options work module
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

import os
import tarfile
from ConfigParser import ConfigParser

class Options:
    def __init__(self):
        self.settings = {'volume': None, 'mixer': None, 'card': 0,'extmixer': None, 'iconpack': None}
        self.CP = ConfigParser()
        self.cfgpath = os.environ["HOME"] + "/.pyalsavolume/config.cfg"
        if not os.path.exists(self.cfgpath):
            tmpath = os.environ["HOME"] + "/.pyalsavolume"
            if not os.path.exists(tmpath):
                os.mkdir(tmpath, 0o775)
            f = open(self.cfgpath, "wb")
            f.write("[Main]\niconpack = default\nextmixer = gnome-alsamixer\ntabspos = 0\n\n[Alsa]\ncard = 0\nmixer = PCM\nvolume = 100\n")
            f.close()
        self.CP.read(self.cfgpath)
        self.GetSettings()

    def GetSettings(self):
        self.settings['card']=self.CP.getint('Alsa', 'card')
        self.settings['mixer']=self.CP.get('Alsa', 'mixer')
        self.settings['volume']=self.CP.getint('Alsa', 'volume')
        self.settings['iconpack']=self.CP.get('Main', 'iconpack')
        self.settings['extmixer']=self.CP.get('Main', 'extmixer')
        self.settings['tabspos']=self.CP.getint('Main', 'tabspos')

    def SetSettings(self, card, mixer, volume, extmixer, iconpack,  tabspos):
        """ Writes configuration to config file"""
        self.CP.set('Main', 'iconpack', str(iconpack))
        self.CP.set('Main', 'extmixer', str(extmixer))
        self.CP.set('Main', 'tabspos', str(tabspos))
        self.CP.set('Alsa', 'card', str(card))
        self.CP.set('Alsa', 'mixer', str(mixer))
        self.CP.set('Alsa', 'volume', str(volume))

    def WriteConfig(self):
        #write
        with open(self.cfgpath, 'wb') as configfile:
            self.CP.write(configfile)
