#-*- coding: utf-8 -*-

import os
import pygtk
pygtk.require('2.0')
import gtk
import gtk.glade

from .reswork import loadResFile

PROJECT="pyinterpol"
VERSION="0.3"

class Interpol():
	def __init__(self):
		self.loader = loadResFile()
		self.way = None
		self.X1 = 0
		self.X2 = 0
		self.Xtr = 0
		self.Y1 = 0
		self.Y2 = 0
		self.gladefile = ""
		if str(os.sys.platform) == "win32":
			self.gladefile = 'mForm.glade'
		else:
			self.gladefile = self.loader.get(PROJECT, "mForm.glade")
		if self.gladefile:
			self.widgetTree = gtk.glade.XML(self.gladefile)
		else:
			os.sys.exit(1)
		dic = {"on_exitItem_activate": self.OnExit, "on_aboutItem_activate": self.OnAbout,
			   "on_x1_value_changed": self.OnX1, "on_xtr_value_changed": self.OnXtr,
			   "on_x2_value_changed": self.OnX2, "on_y1_value_changed": self.OnY1,
			   "on_y2_value_changed": self.OnY2}
		self.widgetTree.signal_autoconnect(dic)
		self.window = self.widgetTree.get_widget('mainWindow')
		self.window.connect("destroy", self.OnDestroy)
		self.xtreb = self.widgetTree.get_widget('xtrlabel')
		self.Yisk = self.widgetTree.get_widget('yisk')

	def OnExit(self, widget):
		gtk.main_quit()

	def OnAbout(self, widget):
		about = gtk.AboutDialog()
		about.set_program_name("pyInterpolation")
		about.set_version(VERSION)
		about.set_copyright("2012(c) Vitaly Tonkacheyev (thetvg@gmail.com)")
		about.set_comments("Программа расчета линейной интерполяции (экстраполяции)")
		about.set_website("http://sites.google.com/site/thesomeprojects/")
		about.set_website_label("Program Website")
		about.run()
		about.destroy()

	def OnX1(self, widget):
		self.X1 = widget.get_value()
		self.Interpolation()

	def OnX2(self, widget):
		self.X2 =  widget.get_value()
		self.Interpolation()

	def OnY1(self, widget):
		self.Y1 =  widget.get_value()
		self.Interpolation()

	def OnY2(self, widget):
		self.Y2 = widget.get_value()
		self.Interpolation()

	def OnXtr(self, widget):
		self.Xtr = widget.get_value()
		self.Interpolation()

	def Interpolation(self):
		answ = 0
		if abs(self.X2 - self.X1) > 0:
			answ = (self.Xtr - self.X1)*(self.Y2 - self.Y1)/(self.X2 - self.X1) + self.Y1
		self.Yisk.set_text(str(answ))

	def OnDestroy(self, widget):
		gtk.main_quit()
