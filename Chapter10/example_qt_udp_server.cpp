#include <iostream>
#include <thread>
#include <QUdpSocket>


class UDPServer: public QObject
{
public:
    UDPServer();
    void start();
    bool alive();

public slots:
    void onNewConnection();
    void onReadyRead();

private:
    QUdpSocket mServer;
    QList<QUdpSocket*> mSockets;
    bool mAlive;
};


UDPServer::UDPServer(): mAlive(true)
{
}

void UDPServer::start()
{
    mServer.bind(QHostAddress("127.0.0.1"), 4242);
    std::cout << "Waiting connection..." << std::endl;
}

bool UDPServer::alive()
{
    return mAlive;
}

void UDPServer::onNewConnection()
{
    QUdpSocket *clientSocket = mServer.nextPendingConnection();
    connect(clientSocket, &QIODevice::readyRead, this, &UDPServer::onReadyRead);
    std::cout << "Connected" << std::endl;

    mSockets.push_back(clientSocket);

    // Send message
    for (QUdpSocket* socket : mSockets) 
    {
        std::cout << "Sending message to client..." << std::endl;
        socket->write(QByteArray("Hello client!\n"));
    }

    // After this, expecting response
}

void UDPServer::onReadyRead()
{
    // Getting response
    QUdpSocket* sender = static_cast<QUdpSocket*>(QObject::sender());
    QByteArray data = sender->readAll();
    std::cout << "Response received: " << data.toStdString() << std::endl;

    // End of connection
    mAlive = false;
}


int main(int argc, char *argv[])
{
    UDPServer server;
    server.start();

    while (server.alive())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "End of udp connection" << std::endl;
    return 0;
}

