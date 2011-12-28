#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
from ConfigParser import ConfigParser
from mycrypt import Coder

class OptWork:
    def __init__(self, parent):
        self.settings = {'server': None, 'user': None, 'remote_path': None,'password': None, 'mount_point': None, 'compress': None, 'port': None}
        self.coder = Coder()
        self.CP = ConfigParser()
        if str(os.sys.platform) != "win32":
            self.confpath = os.environ['HOME'] + "/.pysshclient/config.cfg"
            self.confdir = os.environ['HOME'] + "/.pysshclient"
            if not os.path.exists(self.confpath):
                if not os.path.exists(self.confdir):
                    os.mkdir(self.confdir, 0775)
                f = open(self.confpath, "wb")
                sod = "[Main]\nserver=RemoteServer\nport=22\nuser=RemoteUser\nremote_path=RemotePath\nmount_point=MountPoint\ncompress=0"
                f.write(sod)
                f.close()
            self.pwd_file = self.confdir + "/.pwd.key"
        else:
            if not os.path.exists('config.cfg'):
                f = open('config.cfg', "wb")
                sod = "[Main]\nserver=RemoteServer\nport=22\nuser=RemoteUser\nremote_path=RemotePath\nmount_point=MountPoint\ncompress=0"
                f.write(sod)
                f.close()
            self.confpath = 'config.cfg'
            self.pwd_file = '.pwd.key'
        self.CP.read(self.confpath)
        self.tmp_data = ""

    def GetSettings(self):
        server = self.CP.get('Main', 'server')
        if server:
            self.settings['server']= server
        else:
            self.settings['server']=""
        try:
            port = self.CP.getint('Main', 'port')
            self.settings['port'] = port
        except:
            self.settings['port'] = 22
        user = self.CP.get('Main', 'user')
        if user:
            self.settings['user']= user
        else:
            self.settings['user']=""
        remote_path = self.CP.get('Main', 'remote_path')
        if remote_path:
            self.settings['remote_path']= remote_path
        else:
            self.settings['remote_path']=""
        password = ""
        try:
            self.tmp_data = self.coder.GetFile(self.pwd_file)
            data = self.coder.CheckData(self.tmp_data)
            tmpdes = self.coder.GetCryptObject(data[1])
            password = self.coder.DecodeIt(tmpdes, data[0])
        except:
            pass
        if password:
            self.settings['password']= password
        else:
            self.settings['password']="temp"
        mount_point = self.CP.get('Main', 'mount_point')
        if mount_point:
            self.settings['mount_point']= mount_point
        else:
            self.settings['mount_point']=""
        self.settings['compress'] = self.CP.getint('Main', 'compress')
        #return self.settings

    def SetSettings(self, server, port, user, remote_path, password, mount_point, compressed):
        self.CP.set('Main', 'server', server)
        self.CP.set('Main', 'port', str(port))
        self.CP.set('Main', 'user', user)
        self.CP.set('Main', 'remote_path', remote_path)
        self.CP.set('Main', 'mount_point', mount_point)
        self.CP.set('Main', 'compress', str(compressed))
        tmp_hs = self.coder.GetHash(password)
        tmpdes = self.coder.GetCryptObject(tmp_hs)
        enc_ms = self.coder.EncodeIt(tmpdes,password)
        self.coder.SendFile(self.pwd_file, enc_ms, tmp_hs)

    def WriteConfig(self):
        #write
        with open(self.confpath, 'wb') as configfile:
            self.CP.write(configfile)
