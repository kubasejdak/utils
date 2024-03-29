/////////////////////////////////////////////////////////////////////////////////////
///
/// @file
/// @author Kuba Sejdak
/// @copyright BSD 2-Clause License
///
/// Copyright (c) 2021-2023, Kuba Sejdak <kuba.sejdak@gmail.com>
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// 1. Redistributions of source code must retain the above copyright notice, this
///    list of conditions and the following disclaimer.
///
/// 2. Redistributions in binary form must reproduce the above copyright notice,
///    this list of conditions and the following disclaimer in the documentation
///    and/or other materials provided with the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
/// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
/// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
/// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
/// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
/// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
/// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
/////////////////////////////////////////////////////////////////////////////////////

#include "utils/network/TcpClient.hpp"

#include "utils/network/Error.hpp"
#include "utils/network/logger.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cerrno>
#include <cstring>
#include <utility>

namespace utils::network {

TcpClient::TcpClient()
    : TcpClient({}, m_cUninitialized)
{}

TcpClient::TcpClient(std::string address, int port)
    : m_address(std::move(address))
    , m_port(port)
{
    TcpClientLogger::info("Created TCP/IP client with the following parameters:");
    TcpClientLogger::info("  server address : {}", m_address);
    TcpClientLogger::info("  server port    : {}", m_port);
}

TcpClient::~TcpClient()
{
    disconnect();
}

TcpClient::TcpClient(TcpClient&& other) noexcept
    : m_running(std::exchange(other.m_running, false))
    , m_address(std::move(other.m_address))
    , m_port(std::exchange(other.m_port, m_cUninitialized))
    , m_connection(std::move(other.m_connection))
{}

std::error_code TcpClient::connect()
{
    return connect(m_address, m_port);
}

std::error_code TcpClient::connect(std::string_view address, int port)
{
    if (m_running) {
        TcpClientLogger::error("Failed to connect: client is already running");
        return Error::eClientRunning;
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        TcpClientLogger::error("Failed to create AF_INET socket: {}", strerror(errno));
        return Error::eSocketError;
    }

    auto ip = addressToIp(address);
    if (ip.empty()) {
        TcpClientLogger::error("Failed to convert address to IP: address={}", address);
        return Error::eInvalidArgument;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    serverAddr.sin_port = htons(port);

    if (::connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != 0) { // NOLINT
        TcpClientLogger::error("Failed to connect to server: ip={}, port={}, err={}", ip, port, strerror(errno));
        close(clientSocket);
        return Error::eConnectError;
    }

    TcpClientLogger::info("Connected to TCP/IP server with the following parameters:");
    TcpClientLogger::info("  IP   : {}", ip);
    TcpClientLogger::info("  port : {}", port);

    auto localEndpoint = getLocalEndpoint(clientSocket);
    auto remoteEndpoint = getRemoteEndpoint(serverAddr);
    m_connection = std::make_unique<TcpConnection>(clientSocket, localEndpoint, remoteEndpoint);
    m_running = true;

    return Error::eOk;
}

void TcpClient::disconnect()
{
    if (m_running) {
        m_running = false;
        m_connection->close();
    }
}

Endpoint TcpClient::localEndpoint() const
{
    if (!m_running) {
        TcpClientLogger::error("Cannot get local endpoint: client is not connected");
        return {};
    }

    return m_connection->localEndpoint();
}

Endpoint TcpClient::remoteEndpoint() const
{
    if (!m_running) {
        TcpClientLogger::error("Cannot get remote endpoint: client is not connected");
        return {};
    }

    return m_connection->remoteEndpoint();
}

std::error_code TcpClient::read(BytesVector& bytes, std::size_t size, osal::Timeout timeout)
{
    if (!m_running) {
        TcpClientLogger::error("Failed to read: client is not connected");
        return Error::eClientDisconnected;
    }

    auto error = m_connection->read(bytes, size, timeout);
    if (error == Error::eRemoteEndpointDisconnected)
        disconnect();

    return error;
}

std::error_code
TcpClient::read(std::uint8_t* bytes, std::size_t size, std::size_t& actualReadSize, osal::Timeout timeout)
{
    if (!m_running) {
        TcpClientLogger::error("Failed to read: client is not connected");
        return Error::eClientDisconnected;
    }

    auto error = m_connection->read(bytes, size, actualReadSize, timeout);
    if (error == Error::eRemoteEndpointDisconnected)
        disconnect();

    return error;
}

std::error_code TcpClient::write(std::string_view text)
{
    if (!m_running) {
        TcpClientLogger::error("Failed to write: client is not connected");
        return Error::eClientDisconnected;
    }

    auto error = m_connection->write(text);
    if ((error == Error::eWriteError) && !m_connection->isActive())
        disconnect();

    return error;
}

std::error_code TcpClient::write(const BytesVector& bytes)
{
    if (!m_running) {
        TcpClientLogger::error("Failed to write: client is not connected");
        return Error::eClientDisconnected;
    }

    auto error = m_connection->write(bytes);
    if ((error == Error::eWriteError) && !m_connection->isActive())
        disconnect();

    return error;
}

std::error_code TcpClient::write(const std::uint8_t* bytes, std::size_t size)
{
    if (!m_running) {
        TcpClientLogger::error("Failed to write: client is not connected");
        return Error::eClientDisconnected;
    }

    auto error = m_connection->write(bytes, size);
    if ((error == Error::eWriteError) && !m_connection->isActive())
        disconnect();

    return error;
}

} // namespace utils::network
