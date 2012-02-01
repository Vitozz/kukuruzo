#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
try:
    from gi.repository import Gtk, GdkPixbuf 
except Exception, error:
    os.sys.stderr.write("Error importing Gtk modules %s" %error)
    os.sys.exit(1)

class StatusIcc:
    # activate callback
    def on_hide_resore( self, widget, data=None):
        self.onLClick(widget)
    # popup callback
    def popup(self, widget, button, time, data=None):
        if button == 3:
            if data:
                data.show_all()
                data.popup(None, None, None, None, button, time)
                return True
        return False

    def onLClick(self, widget, data=None):
        if self.parent.window.get_property('visible'):
            self.restoreItem.get_children()[0].set_label("Restore")
            self.parent.window.hide()
        else:
            self.restoreItem.get_children()[0].set_label("Hide")
            self.parent.window.show_all()
            self.parent.window.present()

    def onQuit(self, widget, data=None):
        self.parent.onDestroy(widget)

    def onDisconnect(self, widget):
        self.parent.OnDisconnPress(widget)

    def onAbout(self, widget, data=None):
        about = Gtk.AboutDialog()
        about.set_program_name("pySSHFSClient")
        about.set_version("0.4")
        about.set_copyright("Vitaly Tonkacheyev (thetvg@gmail.com)")
        about.set_comments("Simple GUI program to connect \nto remote folders using SSHFS")
        about.set_website("http://sites.google.com/site/thesomeprojects/")
        about.set_website_label("Program Website")
        about.set_logo(GdkPixbuf.Pixbuf.new_from_file(self.parent.emblem))
        about.run()
        about.destroy()


    def __init__(self, parent):
        self.parent = parent
        # create a new Status Icon
        self.staticon = Gtk.StatusIcon()
        #create Popup-menu
        self.menu = Gtk.Menu()
        #Restore
        self.restoreItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_GO_UP, None)
        self.restoreItem.connect('activate', self.on_hide_resore)
        self.menu.append(self.restoreItem)
        self.restoreItem.get_children()[0].set_label("Hide")
        separator1 = Gtk.SeparatorMenuItem()
        self.menu.append(separator1)
        #Settings
        self.settingsItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_DISCONNECT, None)
        self.settingsItem.connect('activate', self.onDisconnect)
        self.menu.append(self.settingsItem)
        #About
        self.aboutItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_ABOUT, None)
        self.aboutItem.connect('activate', self.onAbout)
        self.menu.append(self.aboutItem)
        #Quit
        self.quitItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_QUIT, None)
        self.quitItem.connect('activate', self.onQuit, self.staticon)
        self.menu.append(self.quitItem)
        self.staticon.set_from_file(self.parent.icon)
        self.status_tooltip = "pySSHFS Client"
        self.staticon.set_tooltip_text(self.status_tooltip)
        self.staticon.connect('activate', self.onLClick)
        self.staticon.connect('popup_menu', self.popup, self.menu)
        self.staticon.set_visible(True)
