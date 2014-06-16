#ifndef PULSEDEVICE_H
#define PULSEDEVICE_H

#include "pulse/pulseaudio.h"
#include <QString>

enum device_type {
	SOURCE,
	SINK
};

class PulseDevice
{
public:
	PulseDevice(const pa_source_info* i_);
	PulseDevice(const pa_sink_info* i);
	uint32_t index() const;
	device_type type() const;
	QString &name();
	QString &description();
	pa_cvolume volume;
	int volume_percent() const;
	bool mute() const;
	double round(double value) const;
private:
	int percent(pa_cvolume& volume) const;
private:
	uint32_t index_;
	device_type type_;
	QString name_;
	QString description_;
	int volume_percent_;
	bool mute_;
};

#endif // PULSEDEVICE_H
