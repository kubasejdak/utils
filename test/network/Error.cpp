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

#include <utils/network/Error.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <string>
#include <system_error>

TEST_CASE("4. Errors have proper human readable messages", "[unit][TcpConnection]")
{
    const std::string cUnrecognizedMsg = "(unrecognized error)";
    constexpr int cErrorsCount = 12;

    for (int i = 0; i < cErrorsCount; ++i) {
        std::error_code error = static_cast<utils::network::Error>(i);
        REQUIRE_THAT(error.category().name(), Catch::Matchers::Equals("utils::network"));
        REQUIRE(!error.message().empty());
        REQUIRE_THAT(error.message(), !Catch::Matchers::Equals(cUnrecognizedMsg));
    }

    constexpr int cInvalidError = cErrorsCount;
    std::error_code error = static_cast<utils::network::Error>(cInvalidError);
    REQUIRE_THAT(error.category().name(), Catch::Matchers::Equals("utils::network"));
    REQUIRE(!error.message().empty());
    REQUIRE_THAT(error.message(), Catch::Matchers::Equals(cUnrecognizedMsg));
}
