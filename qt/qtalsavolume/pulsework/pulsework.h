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
	QStringList getCardList();
	QString getCardName(const QString &description);
	QString getCardDescription(const QString &name);
	QString getDefaultCard();
	void setCurrentCard(const QString &description);
	int getVolume(const QString &description);
	bool getMute(const QString &description);
	void setMute(bool mute);
	void setVolume(int value);
private:
	PulseCore *pulse_;
	QStringList sinks_;
	QStringList sources_;
	QString currentCard_;
};

#endif // PULSEWORK_H
