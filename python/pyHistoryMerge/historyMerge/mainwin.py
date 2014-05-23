 #!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2014
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
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

from PyQt4 import QtCore, QtGui

from filework import FileManager
from reswork import loadResFile

class MainWindow(QtGui.QMainWindow):

	def __init__(self, parent=None):
		QtGui.QMainWindow.__init__(self, parent)
		self.homeDir=None
		self.envPath=None
		if str(os.sys.platform) == 'win32':
			self.homeDir=os.getenv('APPDATA')
		else:
			self.homeDir=os.path.join(os.getenv('HOME'),'.local/share')
		try:
			self.envPath=os.path.join(self.homeDir,'psi+')
		except:
			self.envPath=self.homeDir
		self.fm = FileManager()
		self.readed=False
		#init window
		self.setWindowTitle("Psi/Psi+ history merger")
		self.resloader = loadResFile()
		iconname = self.resloader.get('psihismerger', 'icon32.png')
		self.setWindowIcon(QtGui.QIcon(iconname))
		#Adding widgets
		self.main_widget = QtGui.QWidget()
		self.setCentralWidget(self.main_widget)
		self.btnOpen = QtGui.QPushButton("Add")
		self.btnClean = QtGui.QPushButton("Clear")
		self.btnQuit = QtGui.QPushButton("Exit")
		self.btnSave = QtGui.QPushButton("Save")
		self.btnFN = QtGui.QPushButton("Choose")
		self.label = QtGui.QLabel('Savefile name')
		self.listV = QtGui.QListWidget()
		self.textEdit = QtGui.QLineEdit()
		self.backupCheckBox = QtGui.QCheckBox('Backup output file')
		#setup layouts
		self.main_layout = QtGui.QVBoxLayout()
		self.main_widget.setLayout(self.main_layout)
		self.buttons_layout = QtGui.QHBoxLayout()
		self.hlayout2 = QtGui.QHBoxLayout()
		self.main_layout.addWidget(self.listV)
		self.hlayout3 =QtGui.QHBoxLayout()
		self.main_layout.addLayout(self.hlayout3)
		self.hlayout3.addWidget(self.btnOpen)
		self.hlayout3.addWidget(self.btnClean)
		self.main_layout.addWidget(self.label)
		self.main_layout.addLayout(self.hlayout2)
		self.hlayout2.addWidget(self.textEdit)
		self.hlayout2.addWidget(self.btnFN)
		self.main_layout.addWidget(self.backupCheckBox)
		self.main_layout.addLayout(self.buttons_layout)
		self.buttons_layout.addWidget(self.btnSave)
		self.buttons_layout.addWidget(self.btnQuit)
		#connect signals
		self.fm.showinfo.connect(self.showInfo)
		self.btnOpen.clicked.connect(self.showDialog)
		self.btnClean.clicked.connect(self.clearList)
		self.btnQuit.clicked.connect(self.onExit)
		self.btnSave.clicked.connect(self.saveData)
		self.btnFN.clicked.connect(self.chooseFile)
		self.listV.itemDoubleClicked.connect(self.getListItem)
		self.backupCheckBox.stateChanged.connect(self.backupStateChanged)
		#disabling some buttons
		self.enableButtons(False)

	def showDialog(self):
		if self.readed:
			self.envPath = ""
		filenames=QtGui.QFileDialog.getOpenFileNames(parent=self, caption='Open files', directory=self.envPath, filter="History files (*.history)", options=QtGui.QFileDialog.DontUseNativeDialog|QtGui.QFileDialog.DontResolveSymlinks)
		for filename in filenames:
			if filename:
				self.listV.addItem(filename)
		if self.listV.count()>0:
			self.readed = True
			self.enableButtons(self.readed)


	def saveData(self):
		if self.readed:
			sfileName = str(self.textEdit.text())
			if sfileName:
				files = []
				for index in xrange(self.listV.count()):
					item = self.listV.item(index).text()
					if item:
						files.append(self.fm.readFile(str(item)))
				if len(files) > 0:
					text = self.fm.formatExitFile(files)
					if self.fm.writeFile(sfileName, text):
						self.clearList()
						self.showInfo("File sucessfully saved")
			else:
				self.showInfo("Savefile name not set!")


	def chooseFile(self):
		filename=QtGui.QFileDialog.getSaveFileName(self, caption='Choose file', options=QtGui.QFileDialog.DontUseNativeDialog)
		if filename:
			self.textEdit.setText(filename)

	def showInfo(self, text):
		reply = QtGui.QMessageBox.warning(self, "Information", text)

	def clearList(self):
		if self.listV.count() >0 :
			self.listV.clear()
			self.readed = False
			self.enableButtons(self.readed)

	def enableButtons(self, state):
		self.btnSave.setEnabled(state)
		self.btnClean.setEnabled(state)

	def getListItem(self, item):
		fpath=item.text()
		if fpath:
			self.textEdit.setText(fpath)

	def backupStateChanged(self, state):
		if state == QtCore.Qt.Checked:
			self.fm.setIsBackup(True)
		else:
			self.fm.setIsBackup(False)

	def onExit(self):
		QtGui.QApplication.quit()
