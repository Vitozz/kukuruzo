#-*- coding: utf-8 -*-
#
# (c) Vitaly Tonkacheyev, 2010
# Tray icon animation class
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

class Animate:
    def __init__(self, icons):
        self.value = 0
        if icons:
            self.icons = icons

    def GetIcon(self, index):
        if index != None:
            if index == 0:
                return self.icons.get('icon0')
            if index > 0 and index <= 40:
                return self.icons.get('icon20')
            if index > 40 and index <=60:
                return self.icons.get('icon40')
            if index > 60 and index <=80:
                return self.icons.get('icon60')
            if index >80 and index < 100:
                return self.icons.get('icon80')
            if index == 100:
                return self.icons.get('icon100')
        else:
            return self.icons.get('icon100')
