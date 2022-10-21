/*
 * Copyright (c) 2021 Florian Becker <fb@vxapps.com> (VX APPS).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* cppunit header */
#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Weverything"
#endif
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Weffc++"
#endif
#include <gtest/gtest.h>
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

/* stl header */
#include <filesystem>

/* magic enum */
#include <magic_enum.hpp>

/* modern.cpp.logger */
#include <LoggerFactory.h>

/* local header */
#include "shared/TestHelper.h"

using ::testing::InitGoogleTest;
using ::testing::Test;

/**
 * @brief Filename of temporary log file.
 */
constexpr std::string_view logFilename = "test.log";

/**
 * @brief Count of log messages per thread.
 */
constexpr std::size_t logMessageCount = 10000;

/**
 * @brief Log message itself.
 */
constexpr std::string_view logMessage = "This is a log message";

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wglobal-constructors"
#endif
namespace vx {

  TEST( XmlT, Thread ) {

    std::error_code errorCode {};
    std::filesystem::path tmpPath = std::filesystem::temp_directory_path( errorCode );
    if ( errorCode ) {

      GTEST_FAIL() << "Error getting temp_directory_path: " + errorCode.message() + " Code: " + std::to_string( errorCode.value() );
    }
    tmpPath /= logFilename;
    std::string tmpFile = tmpPath.string();
    std::cout << tmpFile << std::endl;

    /* configure logging, if you dont do, it defaults to standard out logging with colors */
    ConfigureLogger( { { "type", "xml" }, { "filename", tmpFile }, { "reopen_interval", "1" } } );

    unsigned int hardwareThreadCount = std::max<unsigned int>( 1, std::thread::hardware_concurrency() );

#if defined __GNUC__ && __GNUC__ >= 10 || defined _MSC_VER && _MSC_VER >= 1928
    std::vector<std::jthread> threads {};
    threads.reserve( hardwareThreadCount );
    for ( unsigned int n = 0; n < hardwareThreadCount; ++n ) {

      threads.emplace_back( std::jthread( [ &hardwareThreadCount ] {
        std::ostringstream s;
        s << logMessage;

        std::string message = s.str();
        for ( std::size_t i = 0; i < logMessageCount / hardwareThreadCount; ++i ) {

          LogFatal( message );
          LogError( message );
          LogWarning( message );
          LogInfo( message );
          LogDebug( message );
          LogVerbose( message );
        }
      } ) );
    }
#else
    std::vector<std::thread> threads {};
    threads.reserve( hardwareThreadCount );
    for ( unsigned int n = 0; n < hardwareThreadCount; ++n ) {

      threads.emplace_back( std::thread( [ &hardwareThreadCount ] {
        std::ostringstream s;
        s << logMessage;

        std::string message = s.str();
        for ( std::size_t i = 0; i < logMessageCount / hardwareThreadCount; ++i ) {

          LogFatal( message );
          LogError( message );
          LogWarning( message );
          LogInfo( message );
          LogDebug( message );
          LogVerbose( message );
        }
      } ) );
    }
#endif
    for ( auto &thread : threads ) {

      thread.join();
    }
    threads.clear();

    std::size_t count = TestHelper::countNewLines( tmpFile );

    if ( !std::filesystem::remove( tmpFile ) ) {

      GTEST_FAIL() << "Tmp file cannot be removed: " + tmpFile;
    }

    /* Count Severity enum and remove entries we are avoid to log */
    std::size_t differentLogTypes = magic_enum::enum_count<Severity>() - magic_enum::enum_integer( avoidLogBelow );
    EXPECT_EQ( logMessageCount * differentLogTypes, count );
  }
}
#ifdef __clang__
  #pragma clang diagnostic pop
#endif

int main( int argc, char **argv ) {

  InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
