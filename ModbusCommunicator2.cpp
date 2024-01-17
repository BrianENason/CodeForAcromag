// ModbusCommunicator.cpp
#include "ModbusCommunicator.h"
#include <QDebug>

ModbusCommunicator::ModbusCommunicator(QObject *parent)
    : QObject(parent)
{
    // Initialize Modbus TCP client
    modbusClient = new QModbusTcpClient(this);
}

ModbusCommunicator::~ModbusCommunicator()
{
    // Disconnect from BusWorks when the class is destroyed
    disconnectFromBusWorks();
}

bool ModbusCommunicator::connectToBusWorks()
{
    // Connect to BusWorks using the specified IP address
    modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, busWorksIpAddress);
    modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, 502); // Modbus TCP port

    if (!modbusClient->connectDevice())
    {
        qDebug() << "Failed to connect to BusWorks:" << modbusClient->errorString();
        return false;
    }

    qDebug() << "Connected to BusWorks.";
    return true;
}

bool ModbusCommunicator::isConnected() const
{
    // Check if the Modbus client is connected
    return modbusClient->state() == QModbusDevice::ConnectedState;
}

void ModbusCommunicator::queryChannel11Status()
{
    // Check if the client is connected before sending a request
    if (!isConnected())
    {
        qDebug() << "Not connected to BusWorks. Cannot send request.";
        return;
    }

    // Create a Modbus data unit for reading discrete inputs
    QModbusDataUnit readInputs(QModbusDataUnit::InputRegisters, channel11Address, 1);

    // Send the Modbus request
    QModbusReply *reply = modbusClient->sendReadRequest(readInputs, slaveAddress);
    if (!reply)
    {
        qDebug() << "Failed to send Modbus request:" << modbusClient->errorString();
        return;
    }

    // Connect the slot to process the response
    connect(reply, &QModbusReply::finished, this, [this, reply]() {
        if (reply->error() == QModbusDevice::NoError)
        {
            processResponse(reply->result());
        }
        else if (reply->error() == QModbusDevice::ProtocolError)
        {
            qDebug() << "Modbus protocol error:" << reply->rawResult().exceptionCode();
        }
        else
        {
            qDebug() << "Modbus request error:" << reply->errorString();
        }

        // Clean up the reply
        reply->deleteLater();
    });
}

void ModbusCommunicator::processResponse(const QModbusDataUnit &response)
{
    // Extract and interpret the response from BusWorks
    // For simplicity, assuming the response is a single bit representing the status

    // Check if the response contains the expected data
    if (response.valueCount() < 1)
    {
        qDebug() << "Invalid Modbus response. Expected data not available.";
        return;
    }

    // Example: Check if the status of channel 11 is high or low
    bool channel11Status = response.value(0) != 0;

    // Output the response to the user
    qDebug() << "Channel 11 Status:" << (channel11Status ? "High" : "Low");
}

void ModbusCommunicator::disconnectFromBusWorks()
{
    // Disconnect from BusWorks
    if (modbusClient->state() == QModbusDevice::ConnectedState)
    {
        modbusClient->disconnectDevice();
        qDebug() << "Disconnected from BusWorks.";
    }
}
