#!/usr/bin/python
#-*-coding=utf-8-*-
import os

class Compiler:
	def __init__(self):
		self.opts, self.args = None, None
		self.OUTPUT = 0
		self.ERROR = 1
		self.curr_dir = os.getcwd()
		self.psi_path, self.src_plugins_dir = '', ''
		self.multicore = ""
		self.dirlist = []
		self.mingw32_path, self.qmake_path = '', ''
		self.out_path = ''
		self.readOpts()

	def readOpts(self):
		isTest = False
		options = os.sys.argv[1:]
		with_args=[]
		simple=[]
		for option in options:
			if '=' in option:
				with_args.append(str(option).split('='))
			else:
				simple.append(option)
		if not options:
			self.usage()
			os.sys.exit(0)
		for option in simple:
			if option in ('-h', '--help'):
				self.usage()
				os.sys.exit(0)
			elif option in ('-t', '--test'):
				isTest = True
		for option in with_args:
			if option[0] in ('-p', '--path'):
				if not self.psi_path:
					self.psi_path = option[1]
			if option[0] in ('-m', '--mingw32-path'):
				if not self.mingw32_path:
					self.mingw32_path = option[1]
			if option[0] in ('-q', '--qmake-path'):
				if not self.qmake_path:
					self.qmake_path = option[1]
			if option[0] in ('-j', '--threads'):
				if not self.multicore:
					self.multicore = str(option[1])
			if option[0] in ('-o', '--out'):
				if not self.out_path:
					self.out_path = option[1]
		if isTest:
			self.Debug(self.OUTPUT, 'psi path = %s ' % self.psi_path)
			self.Debug(self.OUTPUT, 'mingw path = %s' % self.mingw32_path)
			self.Debug(self.OUTPUT, 'qmake = %s' % self.qmake_path)
			self.Debug(self.OUTPUT, 'cores = %s' % self.multicore)
			self.Debug(self.OUTPUT, 'output path = %s' % self.out_path)
		self.runCompile(isTest)

	def usage(self):
		message = """
Usage: %s [args] [options]


args:

 -h\t\t\t--help\t\t\tThis help
 -p=PSI_SRC_PATH\t--path=PSI_SRC_PATH\tSet Psi sources path
 -o=PATH\t\t--out=PATH\t\tSet output path for compiled plugins
 -j=THREADS\t\t--threads=THREADS\t\tSet nuber of make threads
 -t\t\t\t--test\t\t\tTest only. Not run real commands

Additional args:
 -m=PATH\t--mingw32-path=PATH\tPath to mingw32-make
 -q=PATH\t--qmake-path=PATH\tPath to qmake

Examples:
compile-plugins -o=C:\Temp
compile-plugins -p=C:\src\psi-plus -o=C:\Temp
compile-plugins -t -m=C:\Mingw32 -q=C:\Qt\\bin -p=C:\src\psi-plus -o=C:\Temp
""" % os.sys.argv[0]
		self.Debug(self.OUTPUT, message)

	def getPath(self):
		if not self.psi_path:
			self.psi_path = str(os.getcwd())
		try:
			check_path = os.path.join(self.psi_path, 'psi.pro')
			if check_path:
				if not os.path.exists(check_path):
					message = """
There are no psi+ source code files in %s directory,
please run this command from psi-plus source code directory,
or use -p PSI_SRC_PATH flag to set it manually
""" % "".join(self.psi_path)
					self.Debug(self.ERROR, message)
					self.usage()
					os.sys.exit(1)
				else:
					self.src_plugins_dir = os.path.join(self.psi_path, "src", "plugins", "generic")
					for directory in os.listdir(self.src_plugins_dir):
						full_path =str(os.path.join(self.src_plugins_dir, directory))
						if os.path.isdir(full_path):
							self.dirlist.append(full_path)
		except os.error:
			self.Debug(self.ERROR, os.error.message)

	def CompilePlugin(self, dirname, is_test):
		mccommand, qmcommand, mcommand, command = '', '', '', ''
		make_cmd = 'mingw32-make'
		if self.multicore:
			make_cmd += " -j%s"%self.multicore
		if dirname:
			plugin_path = dirname
			if self.mingw32_path:
				attrs = str(os.path.join(self.mingw32_path, make_cmd))
				mccommand = "%s clean && %s distclean" % (attrs, attrs)
				mcommand = "%s -f makefile.release" % attrs
			else:
				mccommand = "%s clean && %s distclean" % (make_cmd, make_cmd)
				mcommand = "%s -f makefile.release" % make_cmd
			if self.qmake_path:
				qmcommand = "%s" % str(os.path.join(self.qmake_path, "qmake"))
			else:
				qmcommand = "qmake"
			os.chdir(plugin_path)
			if is_test:
				self.Debug(self.OUTPUT, mccommand)
				self.Debug(self.OUTPUT, qmcommand)
				self.Debug(self.OUTPUT, mcommand)
			else:
				os.system(mccommand)
				os.system(qmcommand)
				os.system(mcommand)
			if not self.out_path:
				command = "copy /Y %s\\release\\*.dll %s\\" % (plugin_path , self.curr_dir)
			else:
				command = "copy /Y %s\\release\\*.dll %s\\" % (plugin_path, self.out_path)
			try:
				if is_test:
					self.Debug(self.OUTPUT, command)
				else:
					os.system(command)
			except os.error:
				self.Debug(self.ERROR, "can't copy a library from %s" % plugin_path)
				self.Debug(self.ERROR, os.error.message)
			os.chdir(self.curr_dir)

	def runCompile(self, is_test):
		self.getPath()
		print ('dirlist', self.dirlist)
		for subdir in self.dirlist:
			if subdir:
				changes = os.path.join(subdir, 'changelog.txt')
				if os.path.exists(changes):
					self.CompilePlugin(subdir, is_test)

	def Debug(self, msgtype, message):
		"""msg types self.OUTPUT - otuput self.ERROR - error"""
		output="%s\n" % str(message)
		if msgtype == self.OUTPUT:
			os.sys.stdout.write(output)
		elif msgtype == self.ERROR:
			os.sys.stderr.write(output)


if __name__ == "__main__":
	compiler = Compiler()
	if not os.sys.argv[1:]:
		compiler.usage()
