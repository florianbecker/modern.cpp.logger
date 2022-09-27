/*
 * Copyright (c) 2020 Florian Becker <fb@vxapps.com> (VX APPS).
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

#pragma once

/* stl header */
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

/* local headewr */
#include "Logger.h"

/**
 * @brief vx (VX APPS) namespace.
 */
namespace vx {

  /**
   * @brief The StdLogger class for writing messages to stdout.
   * @author Florian Becker <fb\@vxapps.com> (VX APPS)
   */
  class StdLogger : public Logger {

  public:
    /**
     * @brief Deletet default constructor for StdLogger.
     */
    StdLogger() = delete;

    /**
     * @brief Default constructor for StdLogger.
     * @param _configuration   Logger configuration.
     */
    explicit StdLogger( const std::unordered_map<std::string, std::string> &_configuration ) noexcept;

    /**
     * @brief Build the log message.
     * @param _message   Message to log.
     * @param _severity   Severity level of the message.
     * @param _location   Source location information.
     */
    void log( std::string_view _message,
              Severity _severity,
              const std::source_location &_location = std::source_location::current() ) noexcept override;

    /**
     * @brief Output the log message.
     * @param _message   Message to log.
     */
    void log( std::string_view _message ) noexcept override;

    inline StdLogger &operator<<( int _input ) {

      log( std::to_string( _input ) );
      return *this;
    }

    inline StdLogger &operator<<( double _input ) {

      log( std::to_string( _input ) );
      return *this;
    }

    inline StdLogger &operator<<( std::string_view _input ) {

      log( _input );
      return *this;
    }

    inline StdLogger &operator<<( const std::vector<int> &_input ) {

      std::stringstream result {};
      std::copy( std::begin( _input ), std::end( _input ), std::ostream_iterator<double>( result, " " ) );
      log( result.str() );
      return *this;
    }

  private:
    /**
     * @brief Use colored message output.
     */
    bool m_useColor = false;

    /**
     * @brief Use stderr for severity >= Error.
     */
    bool m_useStdErr = false;
  };
}
