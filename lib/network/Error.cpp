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

#include "utils/network/Error.hpp"

#include <string>

namespace utils::network {

struct ErrorCategory : std::error_category {
    [[nodiscard]] const char* name() const noexcept override;
    [[nodiscard]] std::string message(int value) const override;
};

const char* ErrorCategory::name() const noexcept
{
    return "utils::network";
}

std::string ErrorCategory::message(int value) const
{
    switch (static_cast<Error>(value)) {
        case Error::eOk: return "no error";
        case Error::eInvalidArgument: return "invalid argument";
        case Error::eSocketError: return "socket error";
        case Error::eBindError: return "bind error";
        case Error::eConnectError: return "connect error";
        case Error::eTimeout: return "timeout";
        case Error::eClientRunning: return "client running";
        case Error::eClientDisconnected: return "client disconnected";
        case Error::eServerRunning: return "server running";
        case Error::eConnectionNotActive: return "connection not active";
        case Error::eRemoteEndpointDisconnected: return "remote endpoint disconnected";
        case Error::eWriteError: return "write error";
        default: return "(unrecognized error)";
    }
}

// NOLINTNEXTLINE(readability-identifier-naming)
std::error_code make_error_code(Error error)
{
    static const ErrorCategory cErrorCategory{};
    return {static_cast<int>(error), cErrorCategory};
}

} // namespace utils::network
