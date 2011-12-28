#-*- coding: utf-8 -*-
from xl import event
import os, string

TUNE_TO_PSI = None
PSI_TUNE_PATH = [os.environ["HOME"] + "/.psi",  os.environ["HOME"] + "/.cache/Psi+",  os.environ["HOME"] + "/.cache/Psi"]

def enable(exaile):
    if (exaile.loading):
        event.add_callback(_enable, 'exaile_loaded')
    else:
        _enable(None, exaile, None)

def _enable(eventname, exaile, nothing):
    global TUNE_TO_PSI
    global PSI_TUNE_PATH
    TUNE_TO_PSI = Tune_to_psi(exaile)
    event.add_callback(TUNE_TO_PSI.tune, 'playback_track_start')
    event.add_callback(TUNE_TO_PSI.GoFree,'playback_player_end')
    event.add_callback(TUNE_TO_PSI.onPause,'playback_player_pause')
    event.add_callback(TUNE_TO_PSI.GoFree, 'quit_application')

    print "TuneToPsi plugin enabled"

def disable(exaile):
    global TUNE_TO_PSI
    event.remove_callback(TUNE_TO_PSI.tune, 'playback_track_start')
    event.remove_callback(TUNE_TO_PSI.GoFree,'playback_player_end')
    event.remove_callback(TUNE_TO_PSI.onPause,'playback_player_pause')
    event.remove_callback(TUNE_TO_PSI.GoFree, 'quit_application')
    TUNE_TO_PSI.GoFree(None, None, None)
    print "TuneToPsi plugin disabled"

class Tune_to_psi(object):

    def __init__(self, exaile):
        self.exaile = exaile
        self.old_artist = None
        self.old_album = None
        self.old_titile = None
        self.tuned = False

    def tune(self, event, player, track, title="", artist="", album=""):
        title = track.get_tag_display('title')
        artist = track.get_tag_display('artist')
        album = track.get_tag_display('album')
        print title, artist, album
        if str(event) != 'playback_player_resume':
            if not self.check_last(artist, album, title):
                self.tuned = True
                self.old_artist = artist
                self.old_album = album
                self.old_titile = title
            else:
                self.tuned = False
        else:
            self.tuned = True        
        if self.tuned:
            tunedata = title+"\n"+artist+"\n"+album
            self.write_tune(tunedata)
            print "Tune - updated"

    def onPause(self, event, player, track):
        if player.is_playing():
            self.tune(event, player, track)
        else:
            self.GoFree(event, player, track)

    def check_last(self, arg1, arg2, arg3):
        #check if TAG exists
        if (self.old_artist and self.old_album and self.old_titile) == None:
            return False
        elif self.old_artist == arg1 and self.old_album == arg2 and self.old_titile == arg3:
            return True
        else:
            return False

    def to_file(self, tunestring, path):
        if (os.path.exists(path)):
            try:
                # open a tunefile
                tunefile = open((path + "/tune"), "w")
                tunefile.seek(0)
                tunefile.write(tunestring)
                # closing a tunefile
                tunefile.close()
            except os.error:
                os.sys.stderr.write("Can't write tune to %s"%path)
                os.sys.stderr.write(os.error.message)

    def write_tune(self, tunestring):
        """Writes tune information into tunefile"""
        for path in PSI_TUNE_PATH:
            self.to_file(tunestring, path)

    def GoFree(self, type, player, track):
        self.write_tune("")
        print "Tune Erased"
