#include "reader.h"

Reader::Reader(QString host, int port)
{
    this->host = host;
    this->port = port;

    socket = new QTcpSocket;
    reading = false;

    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketConnectionClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

Reader::~Reader()
{
    delete socket;
}

void Reader::connectHost(const QString &host, quint16 port)
{

    if (reading)
        return;

    socket->connectToHost(host, port);

}

bool Reader::isConnected() const
{
    return reading;
}

void Reader::disconnectHost()
{
    socket->disconnectFromHost();
}

void Reader::socketConnected()
{
    reading = true;
    emit connected();
}

void Reader::socketConnectionClosed()
{
    reading = false;
    emit disconnected();
}

void Reader::socketReadyRead()
{
    QString reply("");

    reply.append(socket->readAll());

    readLine(reply);
}

void Reader::socketError(QAbstractSocket::SocketError e)
{
    emit error(e);
}

void Reader::sendData(const QString &data)
{
    if (!reading)
        return;

    QTextStream stream(socket);

    QString test(data);
    int k;

    QChar c,next;
    QStringList parts;
    QString buffer="";
    bool open=false;
    for(k=0;k<test.length();k++)
    {
        c=test.at(k);
        if(open)
        {
            next=k<test.length()-1?test
                       .at(k+1):' ';
            if(c=='\\'&&next=='"')
            {
                buffer+='"';
                k++;
            }
            else if(c=='"')
                open=false;
            else
                buffer+=c;
        }
        else
        {
            if(!c.isSpace())
            {
                if(c=='"')
                    open=true;
                else
                    buffer+=c;
            }
            else if(!buffer.isEmpty())
            {
                parts<<buffer;
                buffer="";
            }
        }
    }

    if(!buffer.isEmpty())
    {
        parts<<buffer;
    }

    QString bin="";
    bin.append(QString("*%1\r\n")
       .arg(parts.length()));
    int i;
    for(i=0;i<parts.length();i++)
    {
        bin.append(QString("$%1\r\n")
           .arg(parts.at(i)
           .length()));

        bin.append(QString("%1\r\n")
           .arg(parts.at(i)));
    }

    stream<<bin;
    stream.flush();
}

void Reader::readLine(const QString &reply)
{
	QStringList result;

	QString reply_bak = reply;
	if (reply_bak.endsWith("\r\n"))
	{
		reply_bak = reply_bak.left(reply_bak.length() - 2);
	}
	QStringList splited = reply_bak.split("\r\n");
	QString value;
	while (!splited.empty())
	{
		QString section = splited.first();
		splited.removeFirst();

		if (section.isEmpty()){
			result << "nil";
			continue;
		}

		QChar symbol = section[0];
		value = section.mid(1);
		if (symbol == '+')
		{
			result << "string" << value;
		}
		else if (symbol == '-')
		{
			result << "error" << value;
		}
		else if (symbol == ':')
		{
			result << "integer" << value;
		}
		else if (symbol == '$')
		{
			result << "bulk";
			if (value.left(2) == "-1")
			{
				result << "nil";
			}
			continue;//value represents the string's length, no use
		}
		else if (symbol == "*")
		{
			result << "list" << value;
		}
		else
		{
			result << section;
		}
	}


    emit response(result);
}
