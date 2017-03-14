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

import os, re, time, codecs

from PyQt5.QtCore import QObject, pyqtSignal

class FileManager(QObject):

	showinfo = pyqtSignal(str)

	def __init__(self):
		super(FileManager, self).__init__()
		self.regex = re.compile(r'\|(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})\|\d\|(\w+)\|[^|]+\|.+', re.IGNORECASE)
		self.fileDesc = re.compile(r'([^\\/]+)\.([^\\/.]+)$')
		self.isBackup = False

	def readFile(self, fileName):
		result = []
		try:
			mode=None
			if str(os.sys.platform) == 'win32':
				fileName = fileName.decode('UTF-8')
				mode="rb"
			else:
				mode="r"
			if os.access(fileName, os.F_OK|os.R_OK):
				f = open(fileName, mode)
				f.seek(0)
				for line in f.readlines():
					if line:
						item = []
						matchObj = self.regex.match(line)
						if matchObj:
							Y = int(matchObj.group(1))
							M = int(matchObj.group(2))
							D = int(matchObj.group(3))
							H = int(matchObj.group(4))
							M = int(matchObj.group(5))
							S = int(matchObj.group(6))
							u_time = self.convert_date((Y,M,D,H,M,S,-1,-1,-1))
							item.append(u_time)
							item.append(matchObj.group())
							result.append(item)
				f.close()
			else:
				msg="You have no permissions to read file %s"%fileName
				self.showinfo.emit(str(msg))
		except Exception as error:
			self.showinfo.emit(str(error))
		return result

	def writeFile(self, fileName, text):
		try:
			dirname=os.path.dirname(fileName)
			if os.access(dirname, os.F_OK|os.W_OK):
				self.backupFile(fileName)
				fl = codecs.open(fileName, "w", "UTF-8")
				fl.seek(0)
				for line in text:
					fl.write(line + "\n")
				fl.close()
				return True
			elif os.access(dirname, os.F_OK|os.R_OK):
				msg='Error! Read-only directory %s'%dirname
				self.showinfo.emit(str(msg))
			else:
				msg='Error! You have no permissions to write into %s'%dirname
				self.showinfo.emit(str(msg))
		except Exception as  error:
			self.showinfo.emit(str(error))
			return False

	def backupFile(self, fileName):
		if self.isBackup:
			if os.access(fileName, os.F_OK|os.W_OK):
				newname=fileName+'.bak'
				os.rename(fileName, newname)
			else:
				msg='Error! You have no write permissions to %s'%fileName
				self.showinfo.emit(str(msg))

	def convert_date(self, data):
		unix_time = time.mktime(data)
		return unix_time

	def merge_files(self, files):
		result = []
		if files:
			for hfile in files:
				if hfile:
					for line in hfile:
						if line:
							if not(line in result):
								result.append(line)
		return result

	def sortList(self, ilist):
		sortedList = []
		if ilist:
			sortedList = sorted(ilist, key=lambda x: x[1])
			sdlist = self.subsDuplicates(sortedList)
			if sdlist != sortedList:
				return sdlist
		return sortedList

	def subsDuplicates(self, ilist):
		nextIndex = 0
		slist = ilist
		listlen = len(slist)
		for index in range(listlen):
			currLine = slist[index]
			nextIndex = index + 1
			if nextIndex < listlen:
				nextLine = slist[nextIndex]
				if currLine[0] == nextLine[0]:
					matchI = self.regex.match(nextLine[1])
					if matchI:
						if str(matchI.group(7)) == 'to':
							slist[index] = nextLine
							slist[nextIndex] = currLine
		return slist

	def formatExitFile(self, ilist):
		result = []
		sortf = self.sortList(self.merge_files(ilist))
		if sortf:
			for line in sortf:
				if line:
					if line[1]:
						result.append(line[1])
		return result

	def setIsBackup(self, state):
		self.isBackup = state

	def getFileList(self, dirname):
		alist = []
		result = []
		if (os.path.exists(dirname) and os.path.isdir(dirname)):
			alist = os.listdir(dirname)
			for item in alist:
				if item:
					filename = os.path.join(dirname, item)
					fileStruc = [] #list of: 0 = full_path, 1 = file_name, 2 = file_ext
					if (not os.isdir(filename)):
						fileStruc.append(filename)
						matchObj = self.fileDesc.search(filename)
						if matchObj:
							fileStruc.append(matchObj.group(1))
							fileStruc.append(matchObj.group(2))
							if fileStruc[2] == 'history':
								result.append(fileStruc)
		return result

	def processDirs(self, dirlist):
		allFiles = []
		if dirlist:
			for directory in dirlist:
				filelist = self.getFileList(directory)
				for file_s in filelist:
					if file_s:
						allFiles.append(file_s)
		return allFiles

	def getDuplicates(self, allFileList):
		listlen = len(allFileList)
		result = []
		for index in range(listlen):
			duplicates=[]
			current = allFileList[index]
			next_index = index + 1
			if next_index < listlen:
				next_item = allFileList[next_index]
				if current[1] == next_item[1]:
					result.append((current, next_item))
		return result
