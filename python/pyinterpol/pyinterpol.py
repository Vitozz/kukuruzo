#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author - Vitaly Tonkacheyev <thetvg@gmail.com>

import pygtk
pygtk.require('2.0')
import gtk

from pyInterpol.pyInterpol import Interpol

if __name__ == "__main__":
    newFrame = Interpol()
    gtk.main()
