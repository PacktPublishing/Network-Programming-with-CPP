#include <QCoreApplication>

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class SocketTest : public QObject
{
public:
    explicit SocketTest(QObject *parent = 0) : QObject(parent) {}

    void Connect()
    {
        socket = new QTcpSocket(this);
        socket->connectToHost("bogotobogo.com", 80);

        if(socket->waitForConnected(3000))
        {
            qDebug() << "Connected!";

            // send
            socket->write("hello server\r\n\r\n\r\n\r\n");
            socket->waitForBytesWritten(1000);
            socket->waitForReadyRead(3000);
            qDebug() << "Reading: " << socket->bytesAvailable();

            qDebug() << socket->readAll();

            socket->close();
        }
        else
        {
            qDebug() << "Not connected!";
        }

        // sent

        // got

        // closed
    }

signals:

public slots:

private:
    QTcpSocket *socket;

};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SocketTest cTest;
    cTest.Connect();

    return a.exec();
}
