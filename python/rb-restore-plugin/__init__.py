# -*- coding: utf-8 -*-
# This is a Rhythmbox Restore plugin.
# Main features:
## - can restore last playing item when Rhythmbox player starts
## - can restore last playing item position
## - if appropriate option is selected the plugin can stop playing after recovery
## - if appropriate option is selected, the plugin will not restore the position of the last track
#
# Big thanks to Florian Dorn <florian.dorn@boku.ac.at> for the idea of a plugin and a part of the code!!!
#
import rb, rhythmdb
import urllib,  threading, time
import gobject, gtk, gtk.glade
import gconf,  os
import locale

SLEEP_TIME=0.5
ITERATIONS=10
IS_PLAY=1
IS_STOP=0
IS_POSITION=1
NO_POSITION=0
MIN_ELAPSED=2

class ConfigureDialog (object):
    """This class is responsible for the appearance of the Preferences dialog"""
    def __init__(self, glade_file, plugin, *args):
        self.args = args
        self.gladexml = gtk.glade.XML(glade_file)
        self.locale = locale
        self.dialog = self.gladexml.get_widget("preferences_dialog")
        self.plugin = plugin
        self.toggle1 = self.gladexml.get_widget("isplay")
        self.toggle2 = self.gladexml.get_widget("isplaypos")
        self.label = self.gladexml.get_widget("frameCaption")
        self.SetLocale()
        self.dialog.connect("response", self.dialog_response)
        self.toggle1.connect('toggled', self.isplay_toggled)
        self.toggle2.connect('toggled', self.isplaypos_toggled)
        if self.args:
            self.toggle1.set_active(self.args[0])
            self.toggle2.set_active(self.args[1])

    def dialog_response (self, dialog, response):
        dialog.hide()

    def SetLocale(self):
        #check russian locale
        if 'ru' in self.locale.getlocale()[0]:
            self.toggle1.set_label("Не воспроизводить при восстановлении")
            self.toggle2.set_label("Восстанавливать позицию последнего трека")
            self.dialog.set_title("Настройки восстановления")
            self.label.set_text("Опции")
        #check ukrainian locale
        elif 'ua' in self.locale.getlocale()[0]:
            self.toggle1.set_label("Не програвати при відновленні")
            self.toggle2.set_label("Відновлювати позицію останньої композиції")
            self.dialog.set_title("Налаштування відновлення")
            self.label.set_text("Опції")

    def isplay_toggled (self, togglebutton):
        if (togglebutton.get_active()):
            self.plugin.settings.set_gconf_key(isplay=1)
        else:
            self.plugin.settings.set_gconf_key(isplay=0)

    def isplaypos_toggled(self, widget):
        if widget.get_active():
            self.plugin.settings.set_gconf_key(isplaypos=1)
        else:
            self.plugin.settings.set_gconf_key(isplaypos=0)

    def get_dialog (self):
        return self.dialog

class RestoreSettings:
    """Special class to work with plugin settings"""
    def __init__(self, shell):
        self.shell = shell
        self.entry = None
        self.position = NO_POSITION
        self.isPlay = IS_STOP
        self.isPosition = NO_POSITION
        self.dbloaded = False
        self.elapsed = False
        self.gconf = gconf.client_get_default()
        self.gconf_keys = {'entry': '/apps/rhythmbox/plugins/restore/entry',
                                    'position': '/apps/rhythmbox/plugins/restore/position',
                                    'isplay': '/apps/rhythmbox/plugins/restore/isplay',
                                    'isplaypos': '/apps/rhythmbox/plugins/restore/isplaypos'
                                    }
        self.read_gconf()

    def writeSettings(self, entry):
        """Writes plugin settings to gconf"""
        db = self.shell.get_property("db")
        try:
            #imported part of code
            self.entry = db.entry_get(entry, rhythmdb.PROP_LOCATION)
            if self.entry and (self.entry[0:4] in ('http','file','ftp:')):
                #
                self.gconf.set_string(self.gconf_keys['entry'], self.entry)
                if self.isPosition == 1:
                    self.gconf.set_int(self.gconf_keys['position'], self.position)
                else:
                    self.gconf.set_int(self.gconf_keys['position'], NO_POSITION)
                self.gconf.set_int(self.gconf_keys['isplay'], self.isPlay)
                self.gconf.set_int(self.gconf_keys['isplaypos'], self.isPosition)
        except Exception,e:
            os.sys.stderr.write("Error in writeSettings function\n")
            os.sys.stderr.write(str(e)+"\n")

    def read_gconf(self):
        """Reads gconf keys into variables"""
        self.entry = self.gconf.get_string(self.gconf_keys['entry'])
        self.position = self.gconf.get_int(self.gconf_keys['position'])
        self.isPlay = self.gconf.get_int(self.gconf_keys['isplay'])
        if self.isPlay == None:
            self.isPlay = IS_PLAY
        self.isPosition = self.gconf.get_int(self.gconf_keys['isplaypos'])
        if self.isPosition == None:
            self.isPosition = NO_POSITION

    def set_gconf_key (self, **kwargs):
        """Sets settings changed in Configure Dialog"""
        kw = kwargs
        if kw.has_key("isplay"):
            self.isPlay = kw["isplay"]
            self.gconf.set_int(self.gconf_keys['isplay'], self.isPlay)
        if kw.has_key("isplaypos"):
            self.isPosition = kw["isplaypos"]
            self.gconf.set_int(self.gconf_keys['isplaypos'], self.isPosition)

class RertoreThread(threading.Thread):
    """Working thread class"""
    def __init__(self, parent):
        super(RertoreThread,  self).__init__()
        self.parent = parent

    def run(self):
        idented = False
        if self.parent.settings.entry:
            for i in range(ITERATIONS):
                if self.parent.getEntry():
                    idented = True
                    time.sleep(SLEEP_TIME)
                    break
                else:
                    time.sleep(SLEEP_TIME)
            if idented and self.parent.current_entry:
                while not self.parent.setEntry():
                    time.sleep(SLEEP_TIME)
                self.parent.settings.elapsed = False
                ec_evt = self.parent.shell.get_player().connect('elapsed_changed',self.parent.elapsed_changed)
                self.parent.events.append(ec_evt)

class restorePlugin(rb.Plugin):
    """Restore plugin class"""
    def __init__(self):
        rb.Plugin.__init__(self)
        gtk.gdk.threads_init()

    def onDBLoaded(self, db):
        """Runs on RhythmDB load-complete signal"""
        db.disconnect(self.dbloaded_evt)
        del self.dbloaded_evt
        self.runThreads()

    def runThreads(self):
        """Initialize working thread and run it"""
        self.work= RertoreThread(self)
        self.work.start()

    def getEntry(self):
        """Sets the current entry"""
        db = self.shell.get_property("db")
        try:
            can_load = False
            if self.settings.entry:
                decentry = urllib.unquote(self.settings.entry)
                if decentry[0:4] == "file":
                    if os.path.exists(decentry[7:]):
                        can_load = True
                else:
                    #imported part of code
                    filehandle = urllib.urlopen(self.settings.entry)
                    if (filehandle):
                        filehandle.close()
                    #
                        can_load = True
                if(can_load):
                    #imported part of code
                    self.current_entry = db.entry_lookup_by_location(self.settings.entry)
                    #
                    if self.current_entry:
                        self.source = self.shell.get_source_by_entry_type(self.current_entry.get_entry_type())
            return self.current_entry != None
        except Exception,e:
            os.sys.stderr.write("Error in GetEntry function\n")
            os.sys.stderr.write("TRACK %s\n" % self.settings.entry)
            os.sys.stderr.write("POS: %s\n" % self.settings.position)
            os.sys.stderr.write(str(e)+"\n")
            return False

    def setEntry(self):
        """Set Current Entry function"""
        sp = self.shell.get_player()
        if self.current_entry != None:
            if (self.settings.isPlay==IS_PLAY):
                gtk.gdk.threads_enter()
                if self.selectEntry(self.current_entry,  self.source):
                    self.settings.dbloaded = True
                else:
                    self.settings.dbloaded = False
                gtk.gdk.threads_leave()
            else:
                gtk.gdk.threads_enter()
                sp.play_entry(self.current_entry)
                gtk.gdk.threads_leave()
                self.settings.dbloaded = True
        else:
            self.settings.dbloaded = False
        return self.settings.dbloaded

    def selectEntry(self, entry, source, entry_view = None):
        """Selects the current entry in player playlist"""
        if entry:
            sp = self.shell.get_player()
            if source:
                sp.set_selected_source(source)
                try:                    
                    if not self.shell.get_property("visibility"):
                        self.shell.toggle_visibility()
                    entry_view = source.get_entry_view()
                except Exception, e:
                    os.sys.stderr.write("Error in SetEntry function\n")
                    os.sys.stderr.write(str(e)+"\n")
            if entry_view != None:
                try:
                    if entry_view.get_entry_contained(entry):
                        entry_view.select_entry(entry)
                        entry_view.scroll_to_entry(entry)
                        return True
                except Exception, e:
                    os.sys.stderr.write("Error in SetEntry function\n")
                    os.sys.stderr.write(str(e)+"\n")
                return False
            else:
                return False

    def playing_song_changed(self, sp, entry):
        """Function runs on playing-song-changed signal"""
        if entry != None:
            self.current_entry = entry

    def initEntry(self, sp, entry):
        """Checks if saved track is a current playing track"""
        return (entry == self.getPlayingEntry(sp))

    def getPlayingEntry(self, sp):
        """Returns the current playing track uri"""
        try:
            return sp.get_playing_entry().get_playback_uri()
        except:
            return None

    def setPosition(self, sp):
        """Sets current track position at player startup"""
        if self.initEntry(sp,  self.init_entry):
            try:
                sp.set_playing_time(self.settings.position)
                self.settings.elapsed = True
                del self.init_entry
            except Exception,e:
                self.settings.elapsed = False
                os.sys.stderr.write("Error in  setPosition function\n")
                os.sys.stderr.write(str(e)+"\n")

    def elapsed_changed(self, sp, elapsed):
        """Checks if first run and sets track position, else writes position to settings,
            Function runs on elapsed_changed signal"""
        if self.settings.dbloaded and (self.settings.isPosition == IS_POSITION) :
            if not self.settings.elapsed:
                gtk.gdk.threads_enter()
                self.setPosition(sp)
                gtk.gdk.threads_leave()
            if elapsed > MIN_ELAPSED:
                self.settings.position = elapsed

    def activate(self, shell):
        """Function runs when plugin activated"""
        self.shell = shell
        sp = shell.get_player()
        db = self.shell.get_property("db")
        self.current_entry = None
        self.events = []
        self.work = None
        self.source = None
        self.settings = RestoreSettings(self.shell)
        self.init_entry = self.settings.entry
        psc_evt =  sp.connect('playing-song-changed', self.playing_song_changed)
        self.events.append(psc_evt)
        self.dbloaded_evt = db.connect_after('load-complete', self.onDBLoaded)

    def deactivate(self, shell):
        """Function runs when plugin deactivated"""
        sp = self.shell.get_player ()
        if self.work.isAlive:
            self.work._Thread__stop()
            self.work.join(0)
            del self.work
        for event in self.events:
            if event:
                sp.disconnect(event)
                event = None
                del event
        self.settings.writeSettings(self.current_entry)
        del self.events
        del self.current_entry
        del self.settings
        del self.source
        del self.shell

    def create_configure_dialog(self, dialog=None):
        """Shows Restore Plugin Configuration Dialog"""
        if not dialog:
            glade_file = self.find_file("restore.glade")
            dialog = ConfigureDialog(glade_file, self, self.settings.isPlay, self.settings.isPosition).get_dialog()
        return dialog
