#!/usr/bin/env python
#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Tray icon module
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

import sys, os, threading
from gi.repository import Gtk, GdkPixbuf
from gi.repository.Gdk import ScrollDirection

from .pySettingsFrame import SettingsFrame

class StatusIcc:
    # activate callback
    def on_hide_resore(self, widget, data=None):
        self.onLClick(widget)

    # popup callback
    def popup(self, widget, button, time, data=None):
        if button == 3:
            if data:
                data.show_all()
                data.popup(None, None, None, None, button, time)
                return True
        return False

    def scroll_event(self, button, event, data=None):
        if self.parent.volume_slider.is_sensitive():
            if event.direction == ScrollDirection.UP:
                direction = 2
                if self.parent.volume < 100:
                    self.parent.volume += direction
                else:
                    self.parent.volume = 100
            elif event.direction == ScrollDirection.DOWN:
                direction = -2
                if self.parent.volume >= 0:
                    self.parent.volume += direction
                else:
                    self.parent.volume = 0
            if self.parent.volume:
                self.parent.volume_object.set_value(self.parent.volume)
            else:
                print ("Error#18: self.parent.volume variable not initialized in scroll_event function in pyTrayIcon module")

    def onLClick(self, widget, data=None):
        if self.parent.window.get_property('visible'):
            self.parent.window.hide()
        else:
            self.parent.window.move(self.set_window_position()[0], self.set_window_position()[1])
            self.parent.window.show_all()
            self.parent.window.present()

    def onbutton(self, widget, event):
        if event.button == 2:
            if self.muteItem.get_active():
                self.parent.onMute(False)
            else:
                self.parent.onMute(True)

    def set_window_position(self):
        staticon_geometry = self.staticon.get_geometry()[2]
        if staticon_geometry.y <= 200:
            y_coords = staticon_geometry.y + staticon_geometry.height+2
        else:
            y_coords = staticon_geometry.y - self.parent.window.get_size()[1] - 4
        icon_w = staticon_geometry.width/2
        window_w = self.parent.window.get_size()[0]
        dx = abs(icon_w - window_w)/2
        return (staticon_geometry.x - dx/2, y_coords)

    def onQuit(self, widget, data=None):
        print ("Saving settings...")
        self.parent.SaveSettings()
        #Delete thread self.tr1
        if self.tr1:
            if self.tr1.isAlive():
                self.tr1._Thread__stop()
                del self.tr1
        Gtk.main_quit()

    def onMixer(self, widget, data=None):
        if self.parent.ext_mixer:
            command = self.parent.ext_mixer
        else:
            command = 'gnome-alsamixer'
        self.tr1 = threading.Thread(None, self.RunThread, name="t1", kwargs={"command": command})
        self.tr1.start()


    def RunThread(self, command):
        os.system(command)

    def onSettings(self, widget, data=None):
        sett_dialog = SettingsFrame(self.parent)
        sett_dialog.dialog.run()

    def onMute(self, widget, data=None):
        if widget.get_active():
            self.parent.onMute(True)
            self.SetIcon(0)
        else:
            self.parent.onMute(False)
            self.SetIcon(self.parent.volume)

    def SetIcon(self, value):
        if self.parent:
            self.staticon.set_from_file(self.parent.anim.GetIcon(value))
        else:
            print ("Error#17: self.parent variable not initialized in SetIcon function in pyTrayIcon module")

    def onAbout(self, widget, data=None):
        about = Gtk.AboutDialog()
        about.set_program_name("pyAlsaVolume")
        about.set_version("0.9.11")
        about.set_copyright("2009(c) %s (thetvg@gmail.com)"%self.parent.lang.icon_dic.get("about_author"))
        about.set_comments(self.parent.lang.icon_dic.get("about_comments"))
        about.set_website("http://sites.google.com/site/thesomeprojects/")
        about.set_website_label(self.parent.lang.icon_dic.get("about_site_label"))
        about.set_logo(GdkPixbuf.Pixbuf.new_from_file(self.parent.iconpack.get('logo')))
        about.run()
        about.destroy()


    def __init__(self, parent):
        self.parent = parent
        self.tr1 = None
        # create a new Status Icon
        self.staticon = Gtk.StatusIcon()
        #create Popup-menu
        self.menu = Gtk.Menu()
        #Restore
        self.restoreItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_GO_UP,  None)
        self.restoreItem.connect('activate', self.on_hide_resore)
        self.menu.append(self.restoreItem)
        self.restoreItem.set_label(self.parent.lang.icon_dic.get("restoreItem"))
        separator1 = Gtk.SeparatorMenuItem()
        self.menu.append(separator1)
        #Mixer
        self.mixerItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_MEDIA_RECORD,  None)
        self.mixerItem.connect('activate', self.onMixer)
        self.mixerItem.set_label(self.parent.lang.icon_dic.get("mixerItem"))
        self.menu.append(self.mixerItem)
        #Settings
        self.settingsItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_PREFERENCES,  None)
        self.settingsItem.connect('activate', self.onSettings)
        self.menu.append(self.settingsItem)
        #Mute
        self.muteItem = Gtk.CheckMenuItem(self.parent.lang.icon_dic.get("muteItem"))
        self.muteItem.connect('toggled', self.onMute)
        self.menu.append(self.muteItem)
        self.muteItem.set_active(self.parent.checked)
        separator2 = Gtk.SeparatorMenuItem()
        self.menu.append(separator2)
        #About
        self.aboutItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_ABOUT,  None)
        self.aboutItem.connect('activate', self.onAbout)
        self.menu.append(self.aboutItem)
        #Quit
        self.quitItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_QUIT,  None)
        self.quitItem.connect('activate', self.onQuit, self.staticon)
        self.menu.append(self.quitItem)
        self.SetIcon(self.parent.volume)
        self.status_tooltip = self.parent.mixer_name + " - " + str(self.parent.volume) + r"%"
        self.staticon.set_tooltip_text(self.status_tooltip)
        self.staticon.connect('activate', self.onLClick)
        self.staticon.connect('scroll-event', self.scroll_event)
        self.staticon.connect('button-press-event', self.onbutton)
        self.staticon.connect('popup_menu', self.popup, self.menu)
        self.staticon.set_visible(True)
