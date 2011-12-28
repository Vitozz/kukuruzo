#-*- coding: utf-8 -*-
from xl import event
from xl import settings
import os, gtk

TRAY_CONTROL = None
NEXT_OPTION='remote/next'
PLAY_OPTION='remote/playPause'
PREV_OPTION='remote/prev'
TRACK_START_CALLBACK='playback_track_start'
RESUME_CALLBACK='playback_player_resume'
END_CALLBACK='playback_player_end'
PAUSE_CALLBACK='playback_player_pause'
OPTIONS_CALLBACK='option_set'
icon_storage = {0:gtk.STOCK_MEDIA_PAUSE,  1:gtk.STOCK_MEDIA_PLAY,  2:gtk.STOCK_MEDIA_NEXT,  3:gtk.STOCK_MEDIA_PREVIOUS}
icon_names = ["Pause",  "Play",  "Next",  "Previous"]

try:
    import Options
    def get_preferences_pane():
        return Options
except: # fail gracefully if we cant set up the UI
    pass

def enable(exaile):
    if (exaile.loading):
        event.add_callback(_enable, 'exaile_loaded')
    else:
        _enable(None, exaile, None)

def _enable(eventname, exaile, nothing):
    global TRAY_CONTROL
    TRAY_CONTROL = TrayControl(exaile)
    event.add_callback(TRAY_CONTROL.onPlay, TRACK_START_CALLBACK)
    event.add_callback(TRAY_CONTROL.onPlay, RESUME_CALLBACK)
    event.add_callback(TRAY_CONTROL.onEnd, END_CALLBACK)
    event.add_callback(TRAY_CONTROL.onPause, PAUSE_CALLBACK)
    event.add_callback(TRAY_CONTROL.on_setting_change, OPTIONS_CALLBACK)
    print "TrayControl plugin enabled"

def disable(exaile):
    global TRAY_CONTROL
    event.remove_callback(TRAY_CONTROL.onPlay, TRACK_START_CALLBACK)
    event.remove_callback(TRAY_CONTROL.onPlay, RESUME_CALLBACK)
    event.remove_callback(TRAY_CONTROL.onEnd, END_CALLBACK)
    event.remove_callback(TRAY_CONTROL.onPause, PAUSE_CALLBACK)
    event.remove_callback(TRAY_CONTROL.on_setting_change, OPTIONS_CALLBACK)
    for icon in TRAY_CONTROL.icons:
        del icon.icon
    print "TrayControl plugin disabled"

class TrayControl(object):

    def __init__(self, exaile):
        self.exaile = exaile
        self.isplay = False
        self.isplayB = False
        self.isnextB = False
        self.isprevB = False
        self.player = None
        self.opts = opts()
        self.changed = self.opts.changed
        self.icons = self.opts.icons
        self.SetIcons(self.icons, self)
        # load settings
        for option in (PLAY_OPTION, NEXT_OPTION, PREV_OPTION):
            self.on_setting_change(OPTIONS_CALLBACK, None,"%s"%option)

    def AplySettings(self):
        for icon in self.icons:
            if icon != None:
                icon.icon.set_visible(False)
        self.icons[1].icon.set_visible(self.isnextB)
        self.icons[0].icon.set_visible(self.isplayB)
        self.icons[2].icon.set_visible(self.isprevB)

    def on_setting_change(self, name, object, data):
        self.changed = False
        isnext = settings.get_option(NEXT_OPTION, False)
        isplay = settings.get_option(PLAY_OPTION, False)
        isprev = settings.get_option(PREV_OPTION, False)
        if self.isnextB != isnext:
            self.isnextB = isnext
            self.changed = True
        if self.isplayB != isplay:
            self.isplayB = isplay
            self.changed = True
        if self.isprevB != isprev:
            self.isprevB = isprev
            self.changed = True
        if self.changed:
            self.AplySettings()
        
    def SetIcons(self,  icons,  parent):
        if not self.opts.is_set:
            icons[1] = StatusIcon(2,  parent)
            if self.isplay:
                icons[0] = StatusIcon(0, parent)
            else:
                icons[0] = StatusIcon(1, parent)
            icons[2] = StatusIcon(3, parent)
            self.opts.is_set = True
        
    def onPlay(self, event, player, track):
        self.isplay = True
        if self.icons[0]:
            self.icons[0].SetIcon(0)
            
    def onPause(self, event, player, track):
        self.isplay = False
        if self.icons[0]:
            self.icons[0].SetIcon(1)
        
    def onEnd(self,  event, player, track):
        self.isplay = False
        if self.icons[0]:
            self.icons[0].SetIcon(1)
        
class StatusIcon:
    def __init__(self, id, parent):
        self.icon = gtk.StatusIcon()
        self.status_tooltip = ""
        self.SetIcon(id)
        self.id = id
        self.parent = parent
        self.sp = self.parent.exaile
        self.playing = self.parent.isplay
        self.clicked = False
        self.icon.connect('activate', self.onLClick)
        self.icon.set_visible(True)
        
    def SetIcon(self, id):
        stok = None
        stok = icon_storage.get(id)
        self.status_tooltip = icon_names[id]
        self.icon.set_from_stock(stok)
        self.icon.set_tooltip(self.status_tooltip)
        
    def onLClick(self,  widget,  data=None):
        self.Action(self.id,  self.sp)
        
    def Action(self, id, sp):
        if id != None:
            if id == 0 or id == 1:
                if sp.player.is_paused() or sp.player.is_playing():
                    sp.player.toggle_pause()
                else:
                    sp.queue.play()
            if id == 2:
                sp.queue.next()
            if id == 3:
                sp.queue.prev()

class opts:
    def __init__(self):
        self.is_set = False
        self.changed = False
        self.playIcon = None
        self.nextIcon = None
        self.prevIcon = None
        self.icons = [self.playIcon, self.nextIcon, self.prevIcon]
