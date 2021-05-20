/*
 * Copyright (C) 2014-2021 Vitaly Tonkacheyev <thetvg@gmail.com>
 *
 * Big thanks to Clément Démoulins <clement@archivel.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulse/pulseaudio.h"
#include "pulsedevice.h"
#include <QList>
#include <QPair>
#include <QSharedPointer>
#include <QStringList>

struct ServerInfo {
  QString defaultSourceName;
  QString defaultSinkName;
};
enum state { CONNECTING, CONNECTED, ERROR };

typedef QList<PulseDevice::Ptr> PulseDevicePtrList;

class PulseCore {
public:
  explicit PulseCore(const char *clientName);
  ~PulseCore();
  typedef QSharedPointer<PulseCore> Ptr;
  state pState;
  QString defaultSink();
  const QStringList &getCardList() const;
  QString getDeviceDescription(const QString &name);
  QString getDeviceNameByIndex(int index);
  int getCurrentDeviceIndex() const;
  int getVolume();
  int getCardIndex();
  bool getMute();
  void setVolume(int value);
  void setMute(bool mute);
  void setCurrentDevice(const QString &name);
  bool available() const;

private:
  void getSinks();
  void getSources();
  PulseDevice::Ptr getSink(int index);
  PulseDevice::Ptr getSink(const QString &name);
  PulseDevice::Ptr getSource(int index);
  PulseDevice::Ptr getSource(const QString &name);
  PulseDevice::Ptr getDefaultSink();
  PulseDevice::Ptr getDefaultSource();
  PulseDevice::Ptr getDeviceByName(const QString &name);
  PulseDevice::Ptr getDeviceByIndex(int index);
  void setVolume_(const PulseDevice::Ptr &device, int value);
  void setMute_(const PulseDevice::Ptr &device, bool mute);
  void iterate(pa_operation *op);
  static void onError(const QString &message);
  void updateDevices();
  void clearLists();
  static void pa_context_notify_cb(pa_context *context, void *raw);
  static void pa_sink_info_cb(pa_context *c, const pa_sink_info *i, int eol,
                              void *raw);
  static void pa_source_info_cb(pa_context *c, const pa_source_info *i, int eol,
                                void *raw);
  static void server_info_cb(pa_context *c, const pa_server_info *i, void *raw);
  static void success_cb(pa_context *c, int success, void *raw) {
    Q_UNUSED(c);
    Q_UNUSED(success);
    Q_UNUSED(raw);
  }

private:
  pa_mainloop *mainLoop_;
  pa_mainloop_api *mainLoopApi_;
  pa_context *context_;
  int retval_;
  QString currentDeviceName_;
  int currentDeviceIndex_;
  QStringList sinksDescriptions_;
  QStringList sourcesDescriptions_;
  QStringList deviceNames_;
  QStringList deviceDescriptions_;
  PulseDevicePtrList sinks_;
  PulseDevicePtrList sources_;
  bool isAvailable_;
};

#endif // PULSECORE_H
