#!/usr/bin/env python
#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Localization module
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

import os,  locale
from ConfigParser import ConfigParser

class Locales:
    def __init__(self):
        self.slider_dic = {"slider": ""}
        self.settings_dic = {"label1": "", "label2": "", "label3": "", "label4": "",
                            "label5": "", "label6": "", "label7": "", "label8": "",
                            "is_autorun": "", "iconpacks": "", "switchtree": "", "dialogName": "",
                            "column0": "", "column1": "", "tabspos": ""}
        self.icon_dic = {"about_comments": "", "about_site_label": "", "about_author": "",
                        "restoreItem": "", "mixerItem": "", "muteItem": "", "icon_tooltip_mute": "", "icon_tooltip": ""}
        self.CP = ConfigParser()
        curr_locale = locale.getlocale()[0][0:2]
        self.localepath = os.environ["HOME"] + "/.pyalsavolume/lang/"+"%s.lng"%curr_locale
        if not os.path.exists(self.localepath):
            if os.path.exists("/usr/share/pyalsavolume/lang/"+"%s.lng"%curr_locale):
               self.localepath =  "/usr/share/pyalsavolume/lang/"+"%s.lng"%curr_locale
            else:
                self.localepath =  "/usr/share/pyalsavolume/lang/en.lng"
        homepath = os.environ["HOME"] + "/.pyalsavolume"
        if not os.path.exists(homepath):
            os.mkdir(homepath, 0775)
        langpath = os.environ["HOME"] + "/.pyalsavolume/lang"
        if not os.path.exists(langpath):
            os.mkdir(langpath, 0775)
        if self.localepath:
            self.CP.read(self.localepath)
        else:
            self.setDefault()
        self.getLocale()

    def getLocale(self):
        # Slider Frame
        self.slider_dic["slider"] = str(self.CP.get('SliderFrame',  'slider'))
        # Settings Frame
        self.settings_dic["label1"] = str(self.CP.get('SettingsFrame',  'label1'))
        self.settings_dic["label2"] = str(self.CP.get('SettingsFrame',  'label2'))
        self.settings_dic["label3"] = str(self.CP.get('SettingsFrame',  'label3'))
        self.settings_dic["label4"] = str(self.CP.get('SettingsFrame',  'label4'))
        self.settings_dic["label5"] = str(self.CP.get('SettingsFrame',  'label5'))
        self.settings_dic["label6"] = str(self.CP.get('SettingsFrame',  'label6'))
        self.settings_dic["label7"] = str(self.CP.get('SettingsFrame',  'label7'))
        self.settings_dic["label8"] = str(self.CP.get('SettingsFrame',  'label8'))
        self.settings_dic["is_autorun"] = str(self.CP.get('SettingsFrame',  'isautorun'))
        self.settings_dic["iconpacks"] = str(self.CP.get('SettingsFrame',  'iconpacks'))
        self.settings_dic["switchtree"] = str(self.CP.get('SettingsFrame',  'switchtree'))
        self.settings_dic["dialogName"] = str(self.CP.get('SettingsFrame',  'dialogName'))
        self.settings_dic["column0"] = str(self.CP.get('SettingsFrame',  'column0'))
        self.settings_dic["column1"] = str(self.CP.get('SettingsFrame',  'column1'))
        self.settings_dic["tabspos"] = str(self.CP.get('SettingsFrame',  'tabspos'))
        # TrayIcon
        self.icon_dic["about_comments"] = str(self.CP.get('TrayIcon',  'aboutcomments'))
        self.icon_dic["about_site_label"] = str(self.CP.get('TrayIcon',  'aboutsitelabel'))
        self.icon_dic["about_author"] = str(self.CP.get('TrayIcon',  'aboutauthor'))
        self.icon_dic["restoreItem"] = str(self.CP.get('TrayIcon',  'restoreItem'))
        self.icon_dic["mixerItem"] = str(self.CP.get('TrayIcon',  'mixerItem'))
        self.icon_dic["muteItem"] = str(self.CP.get('TrayIcon',  'muteItem'))
        self.icon_dic["icon_tooltip_mute"] = str(self.CP.get('TrayIcon',  'icontooltipmute'))
        self.icon_dic["icon_tooltip"] = str(self.CP.get('TrayIcon',  'icontooltip'))

    def setDefault(self):
            # Slider Frame
            self.slider_dic["slider"] = "Change volume"
            # Settings Frame
            self.settings_dic["label1"] = "Mixers"
            self.settings_dic["label2"] = "Switchers"
            self.settings_dic["label3"] = "Sound Card"
            self.settings_dic["label4"] = "Mixer"
            self.settings_dic["label5"] = "External Mixer Command"
            self.settings_dic["label6"] = "Options"
            self.settings_dic["label7"] = "Icons"
            self.settings_dic["label8"] = "Select Icon Package"
            self.settings_dic["is_autorun"] = "Autorun program on logon"
            self.settings_dic["iconpacks"] = "Icon Packages"
            self.settings_dic["switchtree"] = "On/Off the switch"
            self.settings_dic["dialogName"] = "Settings"
            self.settings_dic["column0"] = "Name of Switch"
            self.settings_dic["column1"] = "Status"
            self.settings_dic["tabspos"] = "Place Tabs Horisontally"
            # TrayIcon
            self.icon_dic["about_comments"] = "Simple program to change one\nof the ALSA mixers volume"
            self.icon_dic["about_site_label"] = "Program Website"
            self.icon_dic["about_author"] = " Vitaly Tonkacheyev "
            self.icon_dic["restoreItem"] = "Restore"
            self.icon_dic["mixerItem"] = "Mixer"
            self.icon_dic["muteItem"] = "Mute"
            self.icon_dic["icon_tooltip_mute"] = "Muted"
            self.icon_dic["icon_tooltip"] = "Sound Card: "        
