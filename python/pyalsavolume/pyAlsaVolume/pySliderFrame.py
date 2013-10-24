#!/usr/bin/env python
#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Main Window
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

import os,  sys,  tarfile
from gi.repository import Gtk
from gi.repository.Gdk import EventType

try:
    import alsaaudio
except:
    sys.stderr.write('No python-alsaaudio module found in pySliderFrame module\n')
    sys.exit(1)

from .pyOptions import Options
from .pyTrayIcon import StatusIcc
from .animate import Animate
from .pyLocalize import Locales
from .reswork import loadResFile

class SliderFrame:
    def __init__(self):
        self._name = "pyalsavolume"
        #find aviable sound cards
        self.alsa = alsaaudio
        self.cards = self.alsa.cards()
        #initialize interface
        self.loader = loadResFile()
        self.gladefile = self.loader.get(self._name, "pySliderFrame.glade")
        if (not os.path.exists(self.gladefile)):
            sys.stderr.write('No pySliderFrame.glade file found\n')
            sys.exit(1)
        self.wTree = Gtk.Builder()
        self.wTree.add_from_file(self.gladefile)
        dic = {"on_volume_slider_value_changed": self.onVolumeChange, "on_volumeFrame_destroy_event": self.onDestroy}
        self.wTree.get_objects()
        self.wTree.connect_signals(dic)
        self.window = self.wTree.get_object('volumeFrame')
        self.volume_object = self.wTree.get_object('volume')
        self.window.connect("destroy", self.onDestroy)
        self.volume_slider = self.wTree.get_object('volume_slider')
        self.window.connect("focus-out-event", self.hideInactive)
        self.lang = Locales()
        self.volume_slider.set_tooltip_text(self.lang.slider_dic.get('slider'))
        # Read config
        self.opts = Options()
        self.tabpos = self.opts.settings.get('tabpos')
        self.card = self.GetCard(self.opts.settings.get('card'))
        #set init values for variables
        self.mixer = None
        self.mixer_name = ""
        self.mixers = []
        self.mixer_id = []
        self.switches = []
        self.valid_mixers = []
        self.multi_mixers = []
        self.mixer_type = None
        self.iconpack = {'logo':  '',  'icon0': '',  'icon20': '',  'icon40': '',  'icon60': '',  'icon80': '',  'icon100': ''}
        self.iconpath = os.environ["HOME"] + "/.local/share/pyalsavolume/icons/"
        self.tmp_dir = os.environ["HOME"] + "/.cache/pyalsavolume"
        self.iconpack_name = self.opts.settings.get('iconpack')
        self.anim = None
        self.SetIconPack(self.iconpack_name)
        self.playback_cap = ('Playback Volume', 'Joined Playback Volume', 'Joined Playback Mute', 'Volume', 'Joined Volume')
        self.rec_cap = ('Capture Mute',  'Joined Capture Mute',  'Capture Exclusive')
        self.mute_cap = ('Mute',  'Joined Mute',  'Playback Mute')
        #get mixers list
        self.mixers = self.GetMixers(self.card)
        #check mixers (valid or multi or checkers)
        self.getValidMixers()
        #get mixer from config
        self.mixer_name, self.mixer = self.GetMixer(self.opts.settings.get('mixer'))
        #get mixer type (playback or capture or multi)
        self.mixer_type = self.getMixerType(self.mixer)
        self.ext_mixer = self.opts.settings.get('extmixer')
        #get volume
        self.volume = self.GetInitVolume(self.opts.settings.get('volume'))
        if self.volume > 0:
            self.checked = False
        else:
            self.checked = True
        #add tray icon
        self.trayicon = None
        self.trayicon = StatusIcc(self)
        #apply settings to tray icon objects
        self.volume_object.set_value(self.volume)
        if self.volume:
            self.PushVolume(self.volume)

    def GetCard(self, keys):
        if keys <= (len(self.cards)-1):
            return keys
        else:
            if len(alsaaudio.cards) > 0:
                return len(alsaaudio.cards) -1
            return 0

    def GetMixers(self, card):
        if card>=0:
            return alsaaudio.mixers(card)
        else:
            return alsaaudio.mixers(0)

    def CheckMixerExists(self, mixer, mixers):
        if mixer in mixers:
            return True
        else:
            return False

    def GetMixer(self, keys):
        self.mixers = self.GetMixers(self.card)
        self.getValidMixers()
        if keys:
            #check options key for existense
            if self.CheckMixerExists(keys, self.mixers):
                mixer = keys
            else:
                mixer = str(self.mixers[0])
        else:
            mixer = str(self.mixers[0])
            #try to apply mixer from options
        try:
            result = self.SetTempMixer(mixer,  self.card)
        except:
            mixer = str(self.mixers[0])
            result = self.SetTempMixer(mixer,  self.card)
        if (mixer and result):
            return mixer, result
        else:
            print ('Error#01: Impossible to get mixer for "%s" card in pySliderFrame module' % self.cards[self.card])
            return None, None

    def GroupMixers(self, mixer_id):
        if mixer_id:
            self.switches = []
            self.valid_mixers = []
            self.multi_mixers = []
            ind = 0
            for mixer in self.mixers:
                if mixer:
                    if self.mixer_id[ind] == 0:
                        self.switches.append(mixer)
                    elif self.mixer_id[ind] > 1:
                        self.valid_mixers.append(mixer)
                        self.multi_mixers.append(mixer)
                    elif self.mixer_id[ind] == 1:
                        self.valid_mixers.append(mixer)
                    ind+= 1
        else:
            print ('Error#02: mixer_id variable is empty in GroupMixers function in pySliderFrame module')

    def SetTempMixer(self,  control,  cardindex):
        return self.alsa.Mixer(control,  cardindex)

    def getValidMixers(self):
        self.mixer_id = []
        self.mixers = self.GetMixers(self.card)
        if self.mixers:
            for mixer in self.mixers:
                if mixer:
                    if self.SetTempMixer(mixer,  self.card).switchcap() == []:
                        self.mixer_id.append(0)
                    else:
                        self.mixer_id.append(len(self.SetTempMixer(mixer,  self.card).switchcap()))
            self.GroupMixers(self.mixer_id)
        else:
            print ('Error#03: self.mixers variable is empty in getValidMixers function in pySliderFrame module')

    def GetInitVolume(self, key):
        if self.mixer:
            if key:
                result = key
            else:
                result = self.mixer.getvolume()[0]
            return result
        else:
            print ('Error#04: self.mixer variable is empty in GetInitVolume function in pySliderFrame module')

    def onVolumeChange(self, widget):
        self.volume = int(widget.get_value())
        self.SetTrayTip()
        self.PushVolume(self.volume)

    def PushVolume(self, value):
        if (self.mixer.mixer() in self.valid_mixers) and (len(self.mixer.volumecap())):
            if not self.mixer.volumecap()[0] in self.playback_cap:
                try:
                    self.mixer.setvolume(value, 0)
                    self.mixer.setvolume(value, 1)
                except:
                    sys.stderr.write("Error#05: can't set volume for mixer %s in pySliderFrame module\n" % self.mixer.mixer())
            else:
                try:
                    self.mixer.setvolume(value)
                except alsaaudio.ALSAAudioError:
                    sys.stderr.write("Error#06:can't set volume for mixer %s in pySliderFrame module\n" % self.mixer.mixer())

    def SetTrayTip(self):
        if self.trayicon:
            self.trayicon.status_tooltip = str(self.lang.icon_dic.get("icon_tooltip")) +": "+ str(self.alsa.cards()[self.card]) + "\n"+  str(self.mixer.mixer()) + " - " + str(self.volume) + r"%"
            self.trayicon.staticon.set_tooltip_text(self.trayicon.status_tooltip)
            self.trayicon.SetIcon(self.volume)

    def SetMixer(self, mixer):
        #initialize mixers for selected sound card
        mixers = self.GetMixers(self.card)
        #group mixers
        self.getValidMixers()
        #initialize mixer
        if mixer:
            self.mixer = None
        else:
            mixer = self.mixers[0]
        self.mixer = self.SetTempMixer(mixer,  self.card)
        #get mixer type
        self.mixer_type = self.getMixerType(self.mixer)
        #apply existed mixer parms
        if self.mixer_type == ([0, 0], 'mute'):
            self.volume = self.mixer.getvolume()[0]
            self.volume_object.set_value(self.volume)
            self.trayicon.muteItem.set_active(False)
        elif self.mixer_type == ([1, 1], 'rec'):
            self.volume = self.mixer.getvolume()[0]
            self.volume_object.set_value(self.volume)
            self.trayicon.muteItem.set_active(False)
        else:
            if self.mixer.mixer() in self.multi_mixers:
                self.volume = self.mixer.getvolume()[0]
                if self.volume > 0:
                    self.trayicon.muteItem.set_active(False)
                    self.volume_object.set_value(self.volume)
                else:
                    self.trayicon.muteItem.set_active(True)
                    self.volume_object.set_value(0)
            else:
                self.volume = self.mixer.getvolume()[0]
                if self.volume > 0:
                    self.trayicon.muteItem.set_active(False)
                else:
                    self.trayicon.muteItem.set_active(True)
        self.SetTrayTip()

    def getMixerType(self, mixer, mute=[], rec=[]):
        try:
            rec = mixer.getrec()
            return rec, 'rec'
        except:
            try:
                mute = mixer.getmute()
                return mute, 'mute'
            except:
                print ('Error#07: Mixer "%s" maybe has a multipy mute(rec) switches in pySliderFrame module' % mixer.mixer())
                return None

    def onMute(self, state):
        self.checked = state
        if self.trayicon:
            self.trayicon.muteItem.set_active(state)
            self.trayicon.status_tooltip = str(self.mixer_name) + " - " + str(self.lang.icon_dic.get("icon_tooltip_mute"))
            self.trayicon.staticon.set_tooltip_text(self.trayicon.status_tooltip)
            if not self.checked:
                try:
                    if self.getMixerType(self.mixer)[1] == 'mute':
                        self.mixer.setmute(0)
                    elif self.getMixerType(self.mixer)[1] == 'rec':
                        self.mixer.setrec(1)
                except:
                    self.PushVolume(self.volume)
                self.SetTrayTip()
                self.volume_slider.set_sensitive(True)
            else:
                try:
                    if self.getMixerType(self.mixer)[1] == 'mute':
                        self.mixer.setmute(1)
                    elif self.getMixerType(self.mixer)[1] == 'rec':
                        self.mixer.setrec(0)
                except:
                    self.PushVolume(0)
                self.volume_slider.set_sensitive(False)

    def hideInactive(self, widget, event):
        if event.type == EventType.FOCUS_CHANGE:
            self.window.hide()

    def SaveSettings(self):
        mixer = ''
        card = 0
        if not self.volume:
            self.volume = 100
        if self.card != None:
            card = self.card
        else:
            card = 0
        if not self.mixer.mixer():
            mixer = self.GetMixers(card)[0]
        else:
            mixer = self.mixer.mixer()
        if not self.ext_mixer:
            self.ext_mixer = 'gnome-alsamixer'
        if not self.iconpack_name:
            self.iconpack_name = 'default'
        if not self.tabpos:
            self.tabpos = 0        
        self.opts.SetSettings(card, mixer, self.volume, self.ext_mixer, self.iconpack_name,  self.tabpos)
        self.opts.WriteConfig()

    def SetIconPack(self,  name):
        if name == "default":
            self.iconpack_name = name
            self.iconpack['logo'] = self.loader.get(self._name, 'icons/volume.png')
            self.iconpack['icon0'] = self.loader.get(self._name, 'icons/tb_icon0.png')
            self.iconpack['icon20'] = self.loader.get(self._name, 'icons/tb_icon20.png')
            self.iconpack['icon40'] = self.loader.get(self._name, 'icons/tb_icon40.png')
            self.iconpack['icon60'] = self.loader.get(self._name, 'icons/tb_icon60.png')
            self.iconpack['icon80'] = self.loader.get(self._name, 'icons/tb_icon80.png')
            self.iconpack['icon100'] = self.loader.get(self._name, 'icons/tb_icon100.png')
        elif name:
            self.iconpack_name = name
            self.readPack(self.iconpath + name)
            self.iconpack['logo'] = self.tmp_dir + '/volume.png'
            self.iconpack['icon0'] = self.tmp_dir + '/tb_icon0.png'
            self.iconpack['icon20'] = self.tmp_dir + '/tb_icon20.png'
            self.iconpack['icon40'] = self.tmp_dir + '/tb_icon40.png'
            self.iconpack['icon60'] = self.tmp_dir + '/tb_icon60.png'
            self.iconpack['icon80'] = self.tmp_dir + '/tb_icon80.png'
            self.iconpack['icon100'] = self.tmp_dir +'/tb_icon100.png'
        self.anim = Animate(self.iconpack)


    def readPack(self,  pack_name):
        currdir = os.getcwd()
        if tarfile.is_tarfile(pack_name):
            tar = tarfile.open(pack_name,  "r:gz")
            if not os.path.exists(self.tmp_dir):
                os.mkdir(self.tmp_dir, 0o775)
            else:
                os.chdir(self.tmp_dir)
                os.system('rm -f *.png')
            os.chdir(currdir)
            tar.extractall(self.tmp_dir)
            tar.close()

    def onDestroy(self, widget):
        self.SaveSettings()
        Gtk.main_quit()

