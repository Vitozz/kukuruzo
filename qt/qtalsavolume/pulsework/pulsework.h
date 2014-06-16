#ifndef PULSEWORK_H
#define PULSEWORK_H

#include "pulsecore.h"
#include <QStringList>
#include <QList>

class PulseWork
{
public:
	PulseWork();
	PulseWork(PulseWork const &);
	~PulseWork();
	QStringList getCardList() const;
	QString getCardName(const QString &description) const;
	QString getCardDescription(const QString &name) const;
	QString getDefaultCard() const;
	void setCurrentCard(const QString &description);
	int getVolume(const QString &description) const;
	bool getMute(const QString &description) const;
	void setMute(bool mute);
	void setVolume(int value);
private:
	PulseCore *pulse_;
	QStringList sinks_;
	QStringList sources_;
	QString currentCard_;
};

#endif // PULSEWORK_H
