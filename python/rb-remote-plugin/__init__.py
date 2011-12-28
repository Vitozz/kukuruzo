#-*- coding: utf-8 -*-
import rb,  rhythmdb
import gobject, gtk,  gtk.glade
import gconf
import locale
from os import sys

ICON_STORAGE = {0:gtk.STOCK_MEDIA_PAUSE,  1:gtk.STOCK_MEDIA_PLAY,  2:gtk.STOCK_MEDIA_NEXT,  3:gtk.STOCK_MEDIA_PREVIOUS}
ICON_NAMES = ("Pause",  "Play",  "Next",  "Previous")

class ConfigureDialog (object):
    def __init__(self, glade_file, plugin):
        self.gladexml = gtk.glade.XML(glade_file)
        dic = {"on_playPause_toggled": self.OnPlayPause,  "on_next_toggled": self.OnNext,
                "on_isscroll_toggled": self.OnScroll, "on_prev_toggled":self.OnPrev,
                "on_preferences_dialog_destroy": self.OnDestroy}
        self.gladexml.signal_autoconnect(dic)
        self.dialog = self.gladexml.get_widget("preferences_dialog")
        self.label = self.gladexml.get_widget("frameCaption")
        self.toggle1 = self.gladexml.get_widget("playPause")
        self.toggle2 = self.gladexml.get_widget("next")
        self.toggle3 = self.gladexml.get_widget("prev")
        self.toggle4 = self.gladexml.get_widget("isscroll")
        self.SetLocale()
        self.plugin = plugin
        self.dialog.connect("response", self.dialog_response)
        self.toggle1.set_active(self.plugin.settings.isplay)
        self.toggle2.set_active(self.plugin.settings.isnext)
        self.toggle3.set_active(self.plugin.settings.isprev)
        self.toggle4.set_active(self.plugin.settings.isscroll)
        

    def dialog_response (self, dialog, response):
        self.plugin.settings.writeSettings()
        self.plugin.settings.InitSettings(self.plugin.icons)
        dialog.hide()
        
    def SetLocale(self):
        if 'ru' in locale.getlocale()[0]:
            self.label.set_label('Кнопки в трее')
            self.toggle1.set_label('Воспроизведение / Пауза')
            self.toggle2.set_label('Следующий')
            self.toggle3.set_label('Предыдущий')
            self.toggle4.set_label('Управление громкости скроллом')
            self.dialog.set_title('Настройки')

    def OnPlayPause(self,  widget):
        if widget.get_active():
            self.plugin.settings.isplay = 1
            print self.plugin.settings.isplay
        else:
            self.plugin.settings.isplay = 0
    
    def OnNext(self,  widget):
        if widget.get_active():
            self.plugin.settings.isnext = 1
        else:
            self.plugin.settings.isnext = 0
    
    def OnPrev(self,  widget):
        if widget.get_active():
            self.plugin.settings.isprev = 1
        else:
            self.plugin.settings.isprev = 0
            
    def OnScroll(self, widget):
        if widget.get_active():
            self.plugin.settings.isscroll = 1
        else:
            self.plugin.settings.isscroll = 0
    
    def OnDestroy(self,  widget):
        self.plugin.settings.writeSettings()
        self.dialog.hide()
        return False
    
    def get_dialog (self):
        return self.dialog

class RemoteSettings:
    """class to work with Settings"""
    def __init__(self):
        self.isplay = 0
        self.isnext = 0
        self.isprev = 0
        self.isscroll = 0
        self.playIcon = None
        self.nextIcon = None
        self.prevIcon = None
        self.icons = [self.playIcon, self.nextIcon, self.prevIcon]
        self.gconf = gconf.client_get_default()
        self.gconf_keys = {0: '/apps/rhythmbox/plugins/remote/play',
                                    1: '/apps/rhythmbox/plugins/remote/next',
                                    2: '/apps/rhythmbox/plugins/remote/prev',
                                    3: '/apps/rhythmbox/plugins/remote/scrol'
                                    }
        self.readSettings()
        
    def writeSettings(self):
        try:
            self.gconf.set_int(self.gconf_keys[0], self.isplay)
            self.gconf.set_int(self.gconf_keys[1], self.isnext)
            self.gconf.set_int(self.gconf_keys[2], self.isprev)
            self.gconf.set_int(self.gconf_keys[3], self.isscroll)
        except Exception, e:
            sys.stderr.write("writeSettings function error \n")
            sys.stderr.write(str(e) + "\n")

    def readSettings(self):
        try:
            self.isplay = self.gconf.get_int(self.gconf_keys[0])
            self.isnext = self.gconf.get_int(self.gconf_keys[1])
            self.isprev = self.gconf.get_int(self.gconf_keys[2])
            self.isscroll = self.gconf.get_int(self.gconf_keys[3])
        except Exception,e:
            sys.stderr.write("readSettings function error \n")
            sys.stderr.write(str(e) + "\n")
            self.isplay = 1
            self.isnext = 1
            self.isprev = 1
            self.isscroll = 1
    
    def InitSettings(self, icons):
        for icon in icons:
            if icon != None:
                icon.icon.set_visible(False)
        icons[1].icon.set_visible(self.isnext)
        icons[0].icon.set_visible(self.isplay)
        icons[2].icon.set_visible(self.isprev)
    
#    def set_gconf_key (self, key,  val):
#        self.gconf.set_int(key, val)

class StatusIcon:
    """Class to work with status icons """
    def __init__(self, id, parent):
        self.icon = gtk.StatusIcon()
        self.status_tooltip = ""
        self.SetIcon(id)
        self.id = id
        self.parent = parent
        self.sp = self.parent.shell.get_player()
        #self.pb_clicked = int(self.sp.get_playing())
        self.icon.connect('activate', self.onLClick)
        self.icon.connect('scroll-event', self.onScroll)
        self.icon.set_visible(True)
        
    def SetIcon(self, id):
        """sets an icon by icon id"""
        stok = None
        global ICON_STORAGE
        stok = ICON_STORAGE.get(id)
        global ICON_NAMES
        self.status_tooltip =ICON_NAMES[id]
        self.icon.set_from_stock(stok)
        self.icon.set_tooltip(self.status_tooltip)
        
    def onLClick(self,  widget,  data=None):
        self.Action(self.id,  self.sp)

    def onScroll(self, button, event, data=None):
        if self.parent.settings.isscroll == 1:
            voltmp = 0
            voltmp = self.sp.get_volume()
            if event.direction == gtk.gdk.SCROLL_UP:
                direction = 0.02
                if voltmp < 1.0:
                    voltmp += direction
                    self.sp.set_volume(voltmp)
                else:
                    voltmp = 1.0
                    self.sp.set_volume(voltmp)
            else:
                direction = -0.02
                if voltmp >= 0:
                    voltmp += direction
                    self.sp.set_volume(voltmp)
                else:
                    voltmp = 0
                    self.sp.set_volume(voltmp)
        
    def Action(self, id, sp):
        if id != None:
            if id in (0, 1):
                    #self.SetIcon(id)
                    sp.playpause ()
            if id == 2:
                sp.do_next()
            if id == 3:
                sp.do_previous()
   
class remotePlugin(rb.Plugin):
    def __init__(self):
        rb.Plugin.__init__(self)

    def activate(self, shell):
        self.shell = shell
        sp = shell.get_player()
        self.pc_evt  = None
        self.settings = RemoteSettings()
        self.icons = self.settings.icons
        self.SetIcons(self.icons, self)
        self.settings.InitSettings(self.icons)
        self.pc_evt = sp.connect('playing-changed', self.playing_changed)
        
    def SetIcons(self, icons, parent):
        sp = self.shell.get_player()
        icons[1] = StatusIcon(2, parent)
        if sp.get_playing():
            icons[0] = StatusIcon(0, parent)
        else:
            icons[0] = StatusIcon(1, parent)
        icons[2] = StatusIcon(3, parent)

    def playing_changed (self, sp, playing):
        if sp.get_playing():
            self.icons[0].SetIcon(0)
        else:
            self.icons[0].SetIcon(1)
       
    def deactivate(self, shell):
        sp = shell.get_player()
        print "deactivate..."
        self.settings.writeSettings()
        if(self.pc_evt):
            sp.disconnect(self.pc_evt)
            self.pc_evt = None
        del self.shell
        del self.pc_evt
        for icon in self.icons:
            del icon.icon
        del self.settings

    def create_configure_dialog(self, dialog=None):
        if not dialog:
            glade_file = self.find_file('plugin.glade')
            dialog = ConfigureDialog(glade_file, self).get_dialog()
            dialog.present()
        return dialog
