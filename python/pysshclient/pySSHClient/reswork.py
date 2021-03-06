#!/usr/bin/env python
#-*- coding: utf-8 -*-
#reswork.py
#

import os

class loadResFile:
    
    def __init__(self):
        pass

    def get(self, project, fname):
        plist = ("/usr/share", "/usr/local/share", str(os.environ['HOME']) + "/.local/share", str(os.getcwd()[:-len(project)]))
        for path in plist:
            if path:
                try:
                    tmp_path = os.path.join(path, project, fname)
                    if os.path.exists(tmp_path):
                        return str(tmp_path)
                except Exception, error:
                    os.sys.stderr.write("Error when getting path %s" %error)
        return ""

