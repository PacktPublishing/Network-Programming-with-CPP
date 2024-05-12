#include <iostream>
#include <thread>
#include <QTcpSocket>
#include <QHostAddress>


class TCPClient: public QObject
{
public:
    TCPClient();
    void start();
    bool alive();

public slots:
    void onReadyRead();

private:
    QTcpSocket mSocket;
    bool mAlive;
};


TCPClient::TCPClient(): mAlive(true)
{
}

void TCPClient::start()
{
    std::cout << "Connecting..." << std::endl;
    mSocket.connectToHost(QHostAddress("127.0.0.1"), 4242);
    connect(&mSocket, &QIODevice::readyRead, this, &TCPClient::onReadyRead);
}

bool TCPClient::alive()
{
    return mAlive;
}

void TCPClient::onReadyRead()
{
    // Getting message
    QByteArray data = mSocket.readAll();
    std::cout << "Message received: " << data.toStdString() << std::endl;

    // Send response
    std::cout << "Sending message back to server..." << std::endl;
    mSocket.write(QByteArray("Hello server!\n"));

    // End of connection
    mAlive = false;
}


int main(int argc, char *argv[])
{
    TCPClient client;
    client.start();

    while (client.alive())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "End of tcp connection" << std::endl;
    return 0;
}

