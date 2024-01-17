// ModbusCommunicator.h
#ifndef MODBUSCOMMUNICATOR_H
#define MODBUSCOMMUNICATOR_H

#include <QObject>
#include <QModbusTcpClient>
#include <QModbusDataUnit>

class ModbusCommunicator : public QObject
{
    Q_OBJECT

public:
    explicit ModbusCommunicator(QObject *parent = nullptr);
    ~ModbusCommunicator();

    // Function to establish the connection to BusWorks
    bool connectToBusWorks();

    // Function to check the connection status
    bool isConnected() const;

    // Function to query the status of channel 11
    void queryChannel11Status();

public slots:
    // Slot to handle the response from BusWorks
    void processResponse(const QModbusDataUnit &response);

    // Slot to handle disconnection
    void disconnectFromBusWorks();

private:
    // Modbus TCP client
    QModbusTcpClient *modbusClient;

    // IP Address of BusWorks
    const QString busWorksIpAddress = "192.168.1.1";

    // Modbus Slave address
    const int slaveAddress = 1; // Assuming BusWorks is the slave

    // Function code for reading discrete inputs
    const int readDiscreteInputsFunctionCode = 2;

    // Address of channel 11
    const quint16 channel11Address = 11001; // Assuming 1X reference address

    // Error checking field
    const int errorCheckingField = 0; // Placeholder for error checking (modify as needed)
};

#endif // MODBUSCOMMUNICATOR_H
