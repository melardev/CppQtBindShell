#pragma once
#include <QTcpSocket>
#include <QObject>
#include <QProcess>
#include <QApplication>
#include <utility>

class Client : public QObject
{
Q_OBJECT
public:

	Client(QString program, QTcpSocket* sock, QObject* parent = Q_NULLPTR);

	void interact();

private:
	QTcpSocket* sock;
	QProcess process;
	QString program;
};
