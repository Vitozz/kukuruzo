import os,  gtk
from xlgui.preferences import widgets
from xl import xdg
from xl.nls import gettext as _


name = _("Tray Controls")
basedir = os.path.dirname(os.path.realpath(__file__))
ui = os.path.join(basedir, "remote.ui")

class PlayPausePreference(widgets.CheckPreference):
    default = False
    name = 'remote/playPause'
    
class NextPreference(widgets.CheckPreference):
    default = False
    name = 'remote/next'
        
class PrevPreference(widgets.CheckPreference):
    default = False
    name = 'remote/prev'

