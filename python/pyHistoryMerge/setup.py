#!/usr/bin/env python

import os
from distutils.core import setup
isWin32 = True
if str(os.sys.platform) == "win32":
	import py2exe
	import glob
else:
	isWin32 = False
	from distutils.command.install_data import install_data


def get_version ():
	readArg = "r"
	if isWin32:
		readArg = "rb"
	f = open("version.txt", readArg)
	f.seek(0)
	lines = f.read()
	f.close()
	return "".join(lines).split("\n")[0]

if isWin32:
	opts = {
	"py2exe": {
			"includes": "PyQt4, sip",
		}
	}
	setup(name='psi-history-merger',
	version=get_version(),
	description='Psi/Psi+ history merger',
	long_description = "Merges history file of Psi/Psi+ jabber client",
	author='Vitaly Tonkacheyev',
	author_email='thetvg@gmail.com',
	url='http://sites.google.com/site/thesomeprojects/',
	maintainer='Vitaly Tonkacheyev',
	maintainer_email='thetvg@gmail.com',
	windows = [
		{"script": "hismerger",
		"icon_resources": [(1, "icon16.ico"),(2, "icon32.ico"),(3, "icon48.ico")],
		}
	],
	options=opts,
	)
else:
	setup(name='psi-history-merger',
	version=get_version(), 
	description='Psi/Psi+ history merger',
	long_description = "Merges history file of Psi/Psi+ jabber client",
	author='Vitaly Tonkacheyev',
	author_email='thetvg@gmail.com',
	url='http://sites.google.com/site/thesomeprojects/',
	maintainer='Vitaly Tonkacheyev',
	maintainer_email='thetvg@gmail.com',
	license = "GNU GPLv3",
	packages = ['historyMerge'],
	scripts=['hismerger'],
	data_files=[("share/applications", ['psihistorymerger.desktop']),
	     ("share/psihismerger", ['icon16.png']),
	     ("share/psihismerger", ['icon32.png']),
	     ("share/psihismerger", ['icon48.png']),
	     ],
	)
	
	
