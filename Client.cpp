#include "Client.h"
#include <utility>

Client::Client(QString program, QTcpSocket* sock, QObject* parent): QObject(parent), sock(sock),
                                                                    program(std::move(program))
{
}

void Client::interact()
{
	// Merge StdErr with StdOut
	process.setProcessChannelMode(QProcess::MergedChannels);

	// Start Process
	process.start(program);
	// process.waitForStarted();


	// When we have something from the server, pipe it to process
	connect(sock, &QTcpSocket::readyRead, this, [&]()
	{
		const QByteArray command = sock->readAll();

		if (command.endsWith('\n'))
			process.write(command);
		else
			process.write((command + "\r\n"));

		process.waitForBytesWritten();

		/*
		if (command.trimmed() == "exit")
		{
			sock->close();
			a.exit(0);
		}
		*/
	});

	// When we have something from process pipe it to socket
	connect(&process, &QProcess::readyRead, this, [&]()
	{
		const QByteArray processOut = process.readAll();
		sock->write(processOut);
		sock->waitForBytesWritten();
	});

	// Exit app and close socket
	connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
	        this, [&](int exitCode, QProcess::ExitStatus /*exitStatus*/)
	        {
		        sock->close();
		        QApplication::exit(exitCode);
		        return exitCode;
	        });
}
