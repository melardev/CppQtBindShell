#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>
#include <QThread>
#include <iostream>
#include "Client.h"

QString getProgramPath()
{
#ifdef Q_OS_WIN
	return "cmd.exe";
#else
	const QFileInfo shell("/bin/sh");
	if (!shell.exists())
	{
		const QFileInfo bash("/bin/bash");
		if (!bash.exists())
		{
			qDebug() << "shell not found, please write down the shell path before running this snippet";
			return "";
		}

		return bash.absoluteFilePath();
	}

	return shell.absoluteFilePath();
#endif
}

int main(int argc, char* argv[])
{
	// To connect to this server use the below command( you must have nmap installed)
	// ncat -v localhost 3002

	const QString program = getProgramPath();
	if (program.isEmpty())
		return 1;

	QApplication a(argc, argv);
	QTcpServer* server = new QTcpServer();

	if (!server->listen(QHostAddress::LocalHost, 3002))
	{
		qDebug().noquote() << "Error on listen()";
		return 1;
	}

	QObject::connect(server, &QTcpServer::newConnection, [&]()
	{
		QTcpSocket* sock = server->nextPendingConnection();

		Client* client = new Client(program, sock);
		client->interact();
		server->close();
	});


	return a.exec();
}
