#-*- coding: utf-8 -*-

import os
import pygtk
pygtk.require('2.0')
import gtk
import gtk.glade

class Interpol():
	def __init__(self):
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
			self.gladefile = '/usr/share/pyinterpol/mForm.glade'
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
		about.set_version("0.2")
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
		args = (self.X1, self.X2, self.Xtr, self.Y1, self.Y2)
		answ = 0
		calcType = self.getCalcType(args)
		if calcType:
			answ = self.Interpolate(args)
		elif calcType != None:
			answ = self.Extrapolate(args)
		self.Yisk.set_text(str(answ))

	def getCalcType(self, args):
		if args[2] < args[1] and (args[2] > args[0]):
			return True
		elif args[2] > args[1] or (args[2] < args[0]):
			return False
		return None

	def Interpolate(self, args, tg=0, c=0, d=0):
		if abs(args[0] - args[1])>0:
			tg = abs(args[3]-args[4])/abs(args[0]-args[1])
			c = args[2] - min(args[0], args[1])
			d = c*tg;
			return (min(args[3], args[4]) + d);
		return 0

	def Extrapolate(self, args, tg=0, c=0, d=0):
		if abs(args[0]-args[1])>0:
			tg = abs(args[3]-args[4])/abs(args[0]-args[1])
			if args[2] > max(args[0], args[1]):
				c = args[2] - max(args[0], args[1])
				d = c*tg
				if args[4] > args[3]:
					return (max(args[3], args[4]) + d)
			elif args[2] < min(args[0], args[1]) :
				c = min(args[0], args[1]) - args[2]
				d = c*tg
				if args[4] > args[3]:
					return (min(args[3], args[4]) - d)
				else:
					return (max(args[3], args[4]) + d)
			else:
				return (min(args[3], args[4]) - d)
		return 0

	def OnDestroy(self, widget):
		gtk.main_quit()
