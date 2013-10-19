#!/usr/bin/env python
# coding: utf-8
# vim: set et sw=2:
#
# Copyright (C) 2009 - Tonkacheyev Vitaly
#
# Big thanks to Rhythmbox creators for player and plugin imstatus
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# The Rhythmbox authors hereby grant permission for non-GPL compatible
# GStreamer plugins to be used and distributed together with GStreamer
# and Rhythmbox. This permission is above and beyond the permissions granted
# by the GPL license by which Rhythmbox is covered. If you modify this code
# you may extend this exception to your version of the code, but you are not
# obligated to do so. If you do not wish to do so, delete this exception
# statement from your version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA.

import rhythmdb, rb
import os

NORMAL_SONG_ARTIST = 'artist'
NORMAL_SONG_TITLE  = 'title'
NORMAL_SONG_ALBUM  = 'album'
STREAM_SONG_ARTIST = 'rb:stream-song-artist'
STREAM_SONG_TITLE  = 'rb:stream-song-title'
STREAM_SONG_ALBUM  = 'rb:stream-song-album'
ARTIST = "artist"
ALBUM = "album"
TITLE = "title"
ENTRY = "entry"

class IMTunePlugin (rb.Plugin):
  def __init__ (self):
    rb.Plugin.__init__ (self)

  def activate (self, shell):
    self.shell = shell
    sp = shell.get_player ()
    self.psc_id  = sp.connect ('playing-song-changed',
                               self.playing_entry_changed)
    self.pc_id   = sp.connect ('playing-changed',
                               self.playing_changed)
    self.pspc_id = sp.connect ('playing-song-property-changed',
                               self.playing_song_property_changed)
    self.tune = {ENTRY: None, ARTIST: None, TITLE: None, ALBUM: None}
    self.old_tune = {ARTIST: None, ALBUM: None, TITLE: None}
    if sp.get_playing ():
        self.set_entry (sp.get_playing_entry ())

  def deactivate (self, shell):
    self.write_tune("")
    self.shell = None
    sp = shell.get_player ()
    sp.disconnect (self.psc_id)
    sp.disconnect (self.pc_id)
    sp.disconnect (self.pspc_id)

  def playing_changed (self, sp, playing):
    if playing:
        self.set_entry (sp.get_playing_entry ())
    else:
        self.old_tune[ARTIST] = None
        self.old_tune[ALBUM] = None
        self.old_tune[TITLE] = None
        self.tune[ENTRY] = None            
        self.write_tune("")

  def playing_entry_changed (self, sp, entry):
    if sp.get_playing ():
        self.set_entry (entry)

  def playing_song_property_changed (self, sp, uri, property, old, new):
    relevant = False
    if sp.get_playing () and property in (NORMAL_SONG_ARTIST, STREAM_SONG_ARTIST):
        self.tune[ARTIST] = new
        relevant = True
    elif sp.get_playing () and property in (NORMAL_SONG_TITLE, STREAM_SONG_TITLE):
        self.tune[TITLE] = new
        relevant = True
    elif sp.get_playing () and property in (NORMAL_SONG_ALBUM, STREAM_SONG_ALBUM):
        self.tune[ALBUM] = new
        relevant = True
    if relevant:
        self.set_tune ()

  def set_entry (self, entry):
    if entry == self.tune["entry"]:
        return
    self.tune["entry"] = entry
    self.set_tune_from_entry ()

  def set_tune_from_entry (self):
    db = self.shell.get_property ("db")
    self.tune[ARTIST] = db.entry_get (self.tune[ENTRY], rhythmdb.PROP_ARTIST)
    self.tune[TITLE]  = db.entry_get (self.tune[ENTRY], rhythmdb.PROP_TITLE)
    self.tune[ALBUM]  = db.entry_get (self.tune[ENTRY], rhythmdb.PROP_ALBUM)
    isStream = False
    if "category" in dir(self.tune[ENTRY].get_entry_type()):
        isStream = ( self.tune[ENTRY].get_entry_type().category == rhythmdb.ENTRY_STREAM)
    else:
        isStream = (self.tune[ENTRY].get_entry_type().props.category == rhythmdb.ENTRY_STREAM)
    if isStream:
        if not self.tune[ARTIST]:
            self.tune[ARTIST] = db.entry_request_extra_metadata (self.tune[ENTRY], STREAM_SONG_ARTIST)
        if not self.tune[TITLE]:
            self.tune[TITLE] = db.entry_request_extra_metadata (self.tune[ENTRY], STREAM_SONG_TITLE)
        if not self.tune[ALBUM]:
            self.tune[ALBUM] = db.entry_request_extra_metadata (self.tune[ENTRY], STREAM_SONG_ALBUM)
    self.set_tune ()

  def set_tune (self, isExists = False):
    """Sets the tune information"""
    if self.tune[ARTIST]:
        artist = str(self.tune[ARTIST])
    else:
        artist = "Unknown(Radio)"
    if self.tune[TITLE]:
        title = str(self.tune[TITLE])
    else:
        title = "Unknown"
    if self.tune[ALBUM]:
        album = str(self.tune[ALBUM])
    else:
        album = "Unknown"
    #Check tune if exists the same
    if not self.check_last(artist, album, title):
        isExists = True
        self.old_tune[ARTIST] = artist
        self.old_tune[ALBUM] = album
        self.old_tune[TITLE] = title
    else:
        isExists = False
    #If tune exists - call write function
    if isExists:
        tunedata = title+"\n"+artist+"\n"+album
        self.write_tune(tunedata)

  def check_last(self, arg1, arg2, arg3):
    """Checks track information, if tune data updated
        arguments are (artist, album, title)"""
    if (self.old_tune[ARTIST] and self.old_tune[ALBUM] and self.old_tune[TITLE]) == None:
        return False
    elif self.old_tune[ARTIST] == arg1 and self.old_tune[ALBUM] == arg2 and self.old_tune[TITLE] == arg3:
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
    psi_pathes=[os.environ["HOME"] + "/.psi",  os.environ["HOME"] + "/.cache/psi+",  os.environ["HOME"] + "/.cache/psi"]
    for path in psi_pathes:
        self.to_file(tunestring, path)
