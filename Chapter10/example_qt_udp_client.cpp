#include <iostream>
#include <thread>
#include <QUdpSocket>
#include <QHostAddress>


class UDPClient: public QObject
{
public:
    UDPClient();
    void start();
    bool alive();

public slots:
    void onReadyRead();

private:
    QUdpSocket mSocket;
    bool mAlive;
};


UDPClient::UDPClient(): mAlive(true)
{
}

void UDPClient::start()
{
    std::cout << "Connecting..." << std::endl;
    mSocket.connectToHost(QHostAddress("127.0.0.1"), 4242);
    connect(&mSocket, &QIODevice::readyRead, this, &UDPClient::onReadyRead);
}

bool UDPClient::alive()
{
    return mAlive;
}

void UDPClient::onReadyRead()
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
    UDPClient client;
    client.start();

    while (client.alive())
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "End of udp connection" << std::endl;
    return 0;
}

