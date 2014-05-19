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

from PyQt4 import Qt

class FileManager:
	def __init__(self, parent=None):
		self.regex = re.compile(r'\|(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})\|\d\|\w+\|[^|]+\|[^|]+', re.IGNORECASE)
		self.regex1 = re.compile(r'\|\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\|\d\|(\w+)\|[^|]+\|[^|]+', re.IGNORECASE)
		self.parent = parent

	def readFile(self, fileName):
		result = []
		try:
			mode=None
			if str(os.sys.platform) == 'win32':
				fileName = fileName.decode('UTF-8')
				mode="rb"
			else:
				mode="r"
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
						u_time = self.convert_date([Y,M,D,H,M,S,-1,-1,-1])
						item.append(u_time)
						item.append(matchObj.group())
						result.append(item)
			f.close()
		except Exception as error:
			self.parent.showInfo(Qt.QString(str(error).decode('UTF-8')))
		return result

	def writeFile(self, fileName, text):
		try:
			fl = codecs.open(fileName, "w", "UTF-8")
			fl.seek(0)
			fl.write(text)
			fl.close()
			return True
		except Exception as  error:
			self.parent.showInfo(Qt.QString(str(error).decode('UTF-8')))
			return False

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
		for index in xrange(listlen):
			currLine = slist[index]
			nextIndex = index + 1
			if nextIndex < listlen:
				nextLine = slist[nextIndex]
				if currLine[0] == nextLine[0]:
					matchI = self.regex1.match(nextLine[1])
					if matchI:
						if str(matchI.group(1)) == 'to':
							slist[index] = nextLine
							slist[nextIndex] = currLine
		return slist

	def formatExitFile(self, ilist):
		result = []
		for line in ilist:
			if line:
				if line[1]:
					result.append(line[1].decode('UTF-8'))
		return result

