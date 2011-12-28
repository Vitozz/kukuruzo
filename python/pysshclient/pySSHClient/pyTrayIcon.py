#!/usr/bin/env python
#-*- coding: utf-8 -*-
import sys
try:
    import pygtk
    pygtk.require('2.0')
    import gtk
except:
    sys.stderr.write('No Gtk Module Found')
    sys.exit(1)

class StatusIcc:
    # activate callback
    def on_hide_resore( self, widget, data=None):
        self.onLClick(widget)
    # popup callback
    def popup(self, widget, button, time, data=None):
        if button == 3:
            if data:
                data.show_all()
                data.popup(None, None, None, 3, time)

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
        about = gtk.AboutDialog()
        about.set_program_name("pySSHFSClient")
        about.set_version("0.3")
        about.set_copyright("Vitaly Tonkacheyev (thetvg@gmail.com)")
        about.set_comments("Simple GUI program to connect \nto remote folders using SSHFS")
        about.set_website("http://sites.google.com/site/thesomeprojects/")
        about.set_website_label("Program Website")
        about.set_logo(gtk.gdk.pixbuf_new_from_file(self.parent.emblem))
        about.run()
        about.destroy()


    def __init__(self, parent):
        self.parent = parent
        # create a new Status Icon
        self.staticon = gtk.StatusIcon()
        #create Popup-menu
        self.menu = gtk.Menu()
        #Restore
        self.restoreItem = gtk.ImageMenuItem(gtk.STOCK_GO_UP)
        self.restoreItem.connect('activate', self.on_hide_resore)
        self.menu.append(self.restoreItem)
        self.restoreItem.get_children()[0].set_label("Hide")
        separator1 = gtk.SeparatorMenuItem()
        self.menu.append(separator1)
        #Settings
        self.settingsItem = gtk.ImageMenuItem(gtk.STOCK_DISCONNECT)
        self.settingsItem.connect('activate', self.onDisconnect)
        self.menu.append(self.settingsItem)
        #About
        self.aboutItem = gtk.ImageMenuItem(gtk.STOCK_ABOUT)
        self.aboutItem.connect('activate', self.onAbout)
        self.menu.append(self.aboutItem)
        #Quit
        self.quitItem = gtk.ImageMenuItem(gtk.STOCK_QUIT)
        self.quitItem.connect('activate', self.onQuit, self.staticon)
        self.menu.append(self.quitItem)
        self.staticon.set_from_file(self.parent.icon)
        self.status_tooltip = "pySSHFS Client"
        self.staticon.set_tooltip(self.status_tooltip)
        self.staticon.connect('activate', self.onLClick)
        self.staticon.connect('popup_menu', self.popup, self.menu)
        self.staticon.set_visible(True)
