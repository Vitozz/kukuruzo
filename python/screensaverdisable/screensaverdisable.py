#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author - Vitaly Tonkacheyev <thetvg@gmail.com>

import os
from gi.repository import Gtk, Gdk, GdkX11, GdkPixbuf

class StatusIcc:
	def onLClick(self, widget, data=None):
		self.actItem.set_active(not self.enabled)

	# popup callback
	def popup(self, widget, button, time, data=None):
		if button == 3:
			if data:
				data.show_all()
				data.popup(None, None, None, None, button, time)
				return True
		return False

	def onbutton(self, widget, event):
		if event.button == 2:
			self.actItem.set_active(not self.enabled)

	def onQuit(self, widget, data=None):
		self.disableScreensaver(self.enabled)
		Gtk.main_quit()

	def Activate(self):
		command = ""
		self.disableScreensaver(self.enabled)
		self.SetIcon()

	def disableScreensaver(self, active):
		command = ""
		if active:
			command = "xdg-screensaver suspend %s"%self.xid
		else:
			command = "xdg-screensaver resume %s"%self.xid
		os.system(command)

	def onActivate(self, widget, data=None):
		if self.actItem.get_active():
			self.enabled = True
		else:
			self.enabled = False
		self.Activate()

	def SetIcon(self):
		if self.enabled:
			self.staticon.set_from_stock(Gtk.STOCK_APPLY)
			self.status_tooltip = "ScreenSaver Disabled"
			self.staticon.set_tooltip_text(self.status_tooltip)
		else:
			self.staticon.set_from_stock(Gtk.STOCK_CANCEL)
			self.status_tooltip = "ScreenSaver Enabled"
			self.staticon.set_tooltip_text(self.status_tooltip)

	def __init__(self, win_xid):
		self.enabled = True
		self.xid = win_xid
		# create a new Status Icon
		self.staticon = Gtk.StatusIcon()
		#create Popup-menu
		self.menu = Gtk.Menu()
		#Activated
		self.actItem = Gtk.CheckMenuItem("Activated")
		self.actItem.connect('toggled', self.onActivate)
		self.menu.append(self.actItem)
		self.actItem.set_active(True)
		separator2 = Gtk.SeparatorMenuItem()
		self.menu.append(separator2)
		#Quit
		self.quitItem = Gtk.ImageMenuItem.new_from_stock(Gtk.STOCK_QUIT,  None)
		self.quitItem.connect('activate', self.onQuit, self.staticon)
		self.menu.append(self.quitItem)
		#
		self.Activate()
		self.staticon.connect('activate', self.onLClick)
		self.staticon.connect('button-press-event', self.onbutton)
		self.staticon.connect('popup_menu', self.popup, self.menu)
		self.staticon.set_visible(True)


if __name__ == "__main__":
	win = Gtk.Window()
	win.show_all()
	win.connect("delete-event", Gtk.main_quit)
	win_xid = win.get_window().get_xid()
	win.hide()
	status_icc = StatusIcc(win_xid)
	Gtk.main()
