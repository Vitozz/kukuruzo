#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulsedevice.h"
#include "pulse/pulseaudio.h"
#include <QStringList>
#include <QList>

struct ServerInfo {
	QString defaultSourceName;
	QString defaultSinkName;
};
enum state {
	CONNECTING,
	CONNECTED,
	ERROR
};

class PulseCore
{
public:
	PulseCore(const char *clientName);
	~PulseCore();
	state pState;
	QString defaultSink();
	QString defaultSource();
	QStringList getSinksDescriptions();
	QStringList getSourcesDescriptions();
	QString getDeviceDescription(const QString &name);
	QString getDeviceName(const QString &description);
	int getVolume(const QString &description);
	bool getMute(const QString &description);
	void setVolume(const QString &description, int value);
	void setMute(const QString &description, bool mute);
private:
	QList<PulseDevice> getSinks();
	QList<PulseDevice> getSources();
	PulseDevice getSink(u_int32_t);
	PulseDevice getSink(const QString &name);
	PulseDevice getSource(u_int32_t);
	PulseDevice getSource(const QString &name);
	PulseDevice getDefaultSink();
	PulseDevice getDefaultSource();
	void setVolume_(PulseDevice &device, int value);
	void setMute_(PulseDevice &device, bool mute);
	void iterate(pa_operation* op);
	void onError(const QString &message);
private:
	pa_mainloop* mainLoop_;
	pa_mainloop_api* mainLoopApi_;
	pa_context* context_;
	int retval_;
	QList<PulseDevice> sinks_;
	QList<PulseDevice> sources_;
};

#endif // PULSECORE_H
