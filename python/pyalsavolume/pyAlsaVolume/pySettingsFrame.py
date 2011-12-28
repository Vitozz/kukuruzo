#!/usr/bin/env python
#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Settings Window
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
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

import os,  re
try:
    import pygtk
    pygtk.require('2.0')
except:
    pass
try:
    import gtk
    import gtk.glade
except:
    os.sys.stderr.write('No Gtk Module Found in pySettingsFrame module\n')
    os.sys.exit(1)

class SettingsFrame:
    def __init__(self, parent):
        #initialize main form with variables
        self.parent = parent
        self.card = self.parent.card
        self.cards = self.parent.cards
        self.items = []
        self.iconpath = os.environ["HOME"] + "/.pyalsavolume/icons"
        self.iconpack = "default"
        #Settings frame interfase
        self.gladefile = '/usr/share/pyalsavolume/pySettingsFrame.glade'
        self.wTree = gtk.glade.XML(self.gladefile)
        dic = {"on_ok_button_pressed": self.onOk, "on_cancel_button_pressed": self.onCancel, "on_is_autorun_toggled": self.onAutorun}
        self.wTree.signal_autoconnect(dic)
        self.dialog = self.wTree.get_widget('settingsDialog')
        self.dialog.connect("close", self.onClose)
        self.dialog.connect("destroy", self.onClose)
        self.mixerBox = self.wTree.get_widget('mixerBox')
        self.cardBox = self.wTree.get_widget('sndcardbox')
        self.switchtree = self.wTree.get_widget('switchtree')
        self.extended_mixer = self.wTree.get_widget('ext_mixer')
        self.is_autorun = self.wTree.get_widget('is_autorun')
        self.iconpacks = self.wTree.get_widget('iconpacks')
        self.tabspos = self.wTree.get_widget('tabspos')
        #init the mixers combobox liststore
        self.mixers = gtk.ListStore(str)
        self.mixerBox.set_model(self.mixers)
        cell = gtk.CellRendererText()
        self.mixerBox.pack_start(cell, True)
        self.mixerBox.add_attribute(cell, 'text', 0)
        #init the sound cards combobox liststore
        self.sndcards = gtk.ListStore(str)
        self.cardBox.set_model(self.sndcards)
        cell = gtk.CellRendererText()
        self.cardBox.pack_start(cell, True)
        self.cardBox.add_attribute(cell, 'text', 0)
        #append items to sound card combo-box
        self.append_items(self.sndcards, self.cards)
        #Switches
        self.tree_store = gtk.TreeStore( str, bool )
        #self.AppendSwitches(self.tree_store)
        self.switchtree.set_model(self.tree_store)
        self.renderer = gtk.CellRendererText()
        self.renderer.set_property( 'editable', False )
        self.renderer1 = gtk.CellRendererToggle()
        self.renderer1.set_property('activatable', True)
        self.renderer1.connect( 'toggled', self.col1_toggled_cb, self.tree_store )
        self.column0 = gtk.TreeViewColumn(self.parent.lang.settings_dic.get("column0"), self.renderer, text=0)
        self.column1 = gtk.TreeViewColumn(self.parent.lang.settings_dic.get("column1"), self.renderer1 )
        self.column1.add_attribute( self.renderer1, "active", 1)
        self.switchtree.append_column( self.column0 )
        self.switchtree.append_column( self.column1 )
        #Connecting combo-boxes signals
        self.mixerBox.connect("changed", self.onChange)
        self.cardBox.connect("changed", self.onCardChange)
        self.curr_item = None
        #get_iconpacks
        self.iconlist = self.getIconList()
        self.icons_store = gtk.ListStore( str )
        self.iconpacks.set_model(self.icons_store)
        textrender = gtk.CellRendererText()
        self.iconpacks.pack_start(textrender, True)
        self.iconpacks.add_attribute(textrender, 'text', 0)
        self.append_items(self.icons_store, self.iconlist)
        # iconpack signal
        self.iconpacks.connect("changed", self.IconpackChanged)
        #Get Saved Settings
        self.cardBox.set_active(self.card)
        self.mixerBox.set_active(self.GetSaved(self.parent))
        self.iconpacks.set_active(self.getPack(self.parent.iconpack_name))
        self.doAutorun("r", "")
        self.tabspos.connect("toggled",  self.onTabPos)
        if self.parent.tabpos:
            self.tabspos.set_active(True)
        else:
            self.tabspos.set_active(False)
        self.setDlgLocale()

    def AppendSwitches(self,  model):
        self.clear_text(model)
        if len(self.parent.switches)>0:
            for item in self.parent.switches:
                if item:
                    model.append( None, (item, None) )
        else:
            print 'Error#10: % card has no switches in pySettingsFrame module' % self.parent.cards[self.card]

    def col1_toggled_cb(self, cell, path, model):
        model[path][1] = not model[path][1]
        if model[path][1]:
            try:
                tmp_mixer = self.parent.SetTempMixer(self.parent.switches[int(path)], self.card)
                tmp_mixer.setmute(0)
            except:
                try:
                    tmp_mixer = self.parent.SetTempMixer(self.parent.switches[int(path)], self.card)
                    tmp_mixer.setrec(0)
                except:
                    print 'Error#08: "%s" maybe a multiswitch. Impossible to identify the switch in pySettingsFrame module' % self.parent.switches[int(path)]
        else:
            try:
                tmp_mixer = self.parent.SetTempMixer(self.parent.switches[int(path)], self.card)
                tmp_mixer.setmute(1)
            except:
                try:
                    tmp_mixer = self.parent.SetTempMixer(self.parent.switches[int(path)], self.card)
                    tmp_mixer.setrec(1)
                except:
                    print 'Error#09: "%s" maybe a multiswitch. Impossible to identify the switch in pySettingsFrame module' % self.parent.switches[int(path)]
        return

    def set_switches(self, switch, setting):
        if self.tree_store:
            self.tree_store.set_value(self.tree_store.get_iter(switch), 1, setting)
        else:
            print "Error#11: self.tree_store object not initialized in set_switches function in pySettingsFrame module"

    def clear_text(self, widget):
        if widget:
            widget.clear()
        else:
            print "Error#12: widget variable not initialized in  clear_text function in pySettingsFrame module"

    def append_items(self, widget, itemlist):
        if widget:
            if itemlist:
                for item in itemlist:
                    if item:
                        widget.append(['%s'%item])
            else:
                print  "Error#13:  itemlist variable not initialized in append_items function in pySettingsFrame module"
        else:
            print  "Error#14:  widget variable not initialized in append_items function in pySettingsFrame module"

    def onOk(self, widget):
        self.parent.tabpos = int(self.tabspos.get_active())
        self.parent.card = self.card
        if self.curr_item:
            self.parent.mixer_name = self.curr_item
            self.parent.SetMixer(self.curr_item)
        else:
            self.curr_item = self.items[0]
        if self.iconpack:
            self.parent.SetIconPack(self.iconpack)
        ext_mixer = self.extended_mixer.get_text()
        if ext_mixer:
            self.parent.ext_mixer = ext_mixer
        else:
            self.parent.ext_mixer = 'gnome-alsamixer'
        
        self.parent.SaveSettings()
        self.onClose(widget)

    def onCancel(self, widget):
        self.onClose(widget)

    def onClose(self, widget):
        self.dialog.destroy()

    def onCardChange(self, widget):
        self.card = widget.get_active()
        self.parent.GetMixers(self.card)
        self.parent.getValidMixers()
        self.items = self.parent.valid_mixers
        self.AppendSwitches(self.tree_store)
        self.InitSwitches()
        self.clear_text(self.mixers)
        self.append_items(self.mixers, self.items)


    def onChange(self, widget):
        index = widget.get_active()
        model = widget.get_model()
        if index >= 0:
            self.curr_item = model[index][0]

    def InitSwitches(self):
        parent = self.parent
        switches = parent.switches
        if switches:
            for switch in switches:
                if switch:
                    try:
                        tmp = parent.SetTempMixer(switch, self.card).getmute()[0]
                        if tmp == 1:
                            self.set_switches(switches.index(switch), 0)
                        else:
                            self.set_switches(switches.index(switch), 1)
                    except:
                        try:
                            tmp = parent.SetTempMixer(switch, self.card).getrec()[0]
                            if tmp == 1:
                                self.set_switches(switches.index(switch), 0)
                            else:
                                self.set_switches(switches.index(switch), 1)
                        except:
                            print 'Error#15: "%s" maybe a multiswitch. Impossible to identify the switch in pySettingsFrame module' % switch
        else:
            print  "Error#16:  switches variable not initialized in InitSwitches function in pySettingsFrame module"

    def GetSaved(self, parent):
        if parent.mixer.mixer():
            if parent.mixer.mixer() in self.items:
                mixer_result = self.items.index(parent.mixer.mixer())
            else:
                mixer_result = 0
        else:
            mixer_result = 0
        if parent.ext_mixer:
            self.extended_mixer.set_text(parent.ext_mixer)
        else:
            self.extended_mixer.set_text('gnome-alsamixer')
        return mixer_result

    def doAutorun(self,  parm, arg,  text =""):
        file_path = os.path.join(os.environ.get('HOME'),  ".config/autostart/pyalsavolume.desktop")
        if arg:
            text = "[Desktop Entry]\nEncoding=UTF-8\nName=pyAlsaVolume\nComment=Changes the volume of ALSA from the system tray\nExec=/usr/bin/pyalsavolume\nType=Application\nVersion=0.9.2\nX-GNOME-Autostart-enabled=%s\nComment[ru]=Регулятор громкости ALSA."%arg
        if parm == "w":
            if os.path.exists(file_path):
                f = open(file_path,  'w')
                f.seek(0)
                f.write(text)
                f.close()
            else:
                prevdir = os.getcwd()
                os.chdir(os.path.join(os.environ.get('HOME'), ".config"))
                if not os.path.exists("autostart"):
                    os.mkdir("autostart")
                os.chdir(prevdir)
                f = open(file_path, 'w')
                f.write(text)
                f.close()
        elif parm == "r":
             if os.path.exists(file_path):
                istrue = ""
                f= open(file_path,  'r')
                f.seek(0)
                lines = f.read()
                f.close()
                re1 = re.compile("(?:[^=\n]{25})(?:\s{0,2}=\s{0,2})(\w{4,5})")
                m = re1.search(lines)
                if m:
                    istrue = m.group(1)
                if istrue == "true":
                    self.is_autorun.set_active(True)
                elif istrue == "false":
                    self.is_autorun.set_active(False)

    def onAutorun(self,  widget):
        if self.is_autorun.get_active():
            self.doAutorun("w",  "true")
        else:
            self.doAutorun("w",  "false")

    def getIconList(self):
        icofiles = []
        result = []
        result.append("default")
        if not os.path.exists(self.iconpath):
            os.mkdir(self.iconpath, 0775)
        icofiles = os.listdir(self.iconpath)
        if icofiles:
            for icon in icofiles:
                if "tar.gz" in icon:
                    result.append(icon)
        return result

    def getPack(self,  pack):
        if pack in self.iconlist:
            return self.iconlist.index(pack)
        else:
            return 0

    def IconpackChanged(self,  widget):
        index = widget.get_active()
        model = widget.get_model()
        if index >= 0:
            self.iconpack = model[index][0]

    def setDlgLocale(self):
        labels = [ self.wTree.get_widget("label1"), self.wTree.get_widget("label2"), self.wTree.get_widget("label3"),
                    self.wTree.get_widget("label4"), self.wTree.get_widget("label5"), self.wTree.get_widget("label6"),
                    self.wTree.get_widget("label7"), self.wTree.get_widget("label8") ]
        i = 1
        for label in labels:
            label.set_text(self.parent.lang.settings_dic.get("label%d"%i))
            i+=1
        self.is_autorun.set_label(self.parent.lang.settings_dic.get("is_autorun"))
        self.switchtree.set_tooltip_text(self.parent.lang.settings_dic.get("switchtree"))
        self.iconpacks.set_tooltip_text(self.parent.lang.settings_dic.get("iconpacks"))
        self.dialog.set_title(self.parent.lang.settings_dic.get("dialogName"))
        self.tabspos.set_label(self.parent.lang.settings_dic.get("tabspos"))

    def onTabPos(self,  widget):
        tabwidget = self.wTree.get_widget('tabwidget')
        if widget.get_active():
            tabwidget.set_tab_pos(gtk.POS_TOP)
        else:
            tabwidget.set_tab_pos(gtk.POS_LEFT)
