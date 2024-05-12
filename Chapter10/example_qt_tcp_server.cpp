#include <iostream>
#include <thread>
#include <QTcpServer>
#include <QTcpSocket>


class TCPServer: public QObject
{
public:
    TCPServer();
    void start();
    bool alive();

public slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer mServer;
    QList<QTcpSocket*> mSockets;
    bool mAlive;
};


TCPServer::TCPServer(): mAlive(true)
{
}

void TCPServer::start()
{
    mServer.listen(QHostAddress("127.0.0.1"), 4242);
    connect(&mServer, &QTcpServer::newConnection, this, &TCPServer::onNewConnection);
    std::cout << "Waiting connection..." << std::endl;
}

bool TCPServer::alive()
{
    return mAlive;
}

void TCPServer::onNewConnection()
{
    QTcpSocket *clientSocket = mServer.nextPendingConnection();
    connect(clientSocket, &QIODevice::readyRead, this, &TCPServer::onReadyRead);
    std::cout << "Connected" << std::endl;

    mSockets.push_back(clientSocket);

    // Send message
    for (QTcpSocket* socket : mSockets) 
    {
        std::cout << "Sending message to client..." << std::endl;
        socket->write(QByteArray("Hello client!\n"));
    }

    // After this, expecting response
}

void TCPServer::onReadyRead()
{
    // Getting response
    QTcpSocket* sender = static_cast<QTcpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    std::cout << "Response received: " << data.toStdString() << std::endl;

    // End of connection
    mAlive = false;
}


int main(int argc, char *argv[])
{
    TCPServer server;
    server.start();

    while (server.alive())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "End of tcp connection" << std::endl;
    return 0;
}

