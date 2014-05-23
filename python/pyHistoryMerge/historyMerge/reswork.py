#!/usr/bin/env python
#-*- coding: utf-8 -*-
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

class loadResFile:

	def __init__(self):
		pass

	def get(self, project, fname):

		if str(os.sys.platform) == 'win32':
			plist = ()
			try:
				tmp_path = os.path.join(str(os.getcwd()).decode('UTF-8'), fname)
				if os.path.exists(tmp_path):
					return str(tmp_path)
			except Exception, error:
				os.sys.stderr.write("Error when getting path %s" %error)
		else:
			plist = ("/usr/share", "/usr/local/share", str(os.environ['HOME']) + "/.local/share", str(os.getcwd()))
			for path in plist:
				if path:
					try:
						tmp_path = os.path.join(path, project, fname)
						if os.path.exists(tmp_path):
							return str(tmp_path)
						else:
							tmp_path = os.path.join(path, fname)
							if os.path.exists(tmp_path):
								return str(tmp_path)
					except Exception, error:
						os.sys.stderr.write("Error when getting path %s" %error)
		return ""

