/****************************************************************************************************************************
  DebugPrintMacros.h
  
  For Portenta_H7 (STM32H7) with Vision-Shield Ethernet
  
  Portenta_H7_AsyncWebServer is a library for the Portenta_H7 with with Vision-Shield Ethernet
  
  Based on and modified from AsyncTCP (https://github.com/me-no-dev/ESPAsyncTCP)
  Built by Khoi Hoang https://github.com/khoih-prog/Portenta_H7_AsyncTCP
  
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.
 
  Version: 1.4.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      06/10/2021 Initial coding for Portenta_H7 (STM32H7) with Vision-Shield Ethernet
  1.1.0   K Hoang      08/10/2021 Add support to Portenta_H7 (STM32H7) using Murata WiFi
  1.2.0   K Hoang      15/10/2021 Workaround for issue with dns_gethostbyname not-working issue in mbed_portenta v2.5.2
  1.3.0   K Hoang      06/12/2021 Fix compile error issue in mbed_portenta v2.6.1+
  1.3.1   K Hoang      23/05/2022 Fix typo in `library.json`
  1.3.2   K Hoang      21/06/2022 Fix PIO platform in `library.json`
  1.4.0   K Hoang      26/09/2022 Fix issue with slow browsers or network. Clean up. Remove hard-code if possible
 *****************************************************************************************************************************/

#ifndef _DEBUG_PRINT_MACROS_H
#define _DEBUG_PRINT_MACROS_H

/////////////////////////////////////////////

// Some customizable print macros to suite the debug needs de jour.

// Debug macros
// #include <pgmspace.h>
// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
// This value is resolved at compile time.
#define _FILENAME_ strrchr("/" __FILE__, '/')

/////////////////////////////////////////////

#if defined(DEBUG_ESP_PORT) && !defined(DEBUG_TIME_STAMP_FMT)
  #define DEBUG_TIME_STAMP_FMT    "%06u.%03u "
  
  struct _DEBUG_TIME_STAMP 
  {
    unsigned dec;
    unsigned whole;
  };
  
  inline struct _DEBUG_TIME_STAMP debugTimeStamp() 
  {
    struct _DEBUG_TIME_STAMP st;
    unsigned now = millis() % 1000000000;
    st.dec = now % 1000;
    st.whole = now / 1000;
    return st;
  }
#endif

/////////////////////////////////////////////

#if defined(DEBUG_ESP_PORT) && !defined(DEBUG_GENERIC)
  #define DEBUG_GENERIC( module, format, ... ) \
    do { \
      struct _DEBUG_TIME_STAMP st = debugTimeStamp(); \
      DEBUG_ESP_PORT.printf( DEBUG_TIME_STAMP_FMT module " " format, st.whole, st.dec, ##__VA_ARGS__ ); \
    } while(false)
#endif

/////////////////////////////////////////////

#if defined(DEBUG_ESP_PORT) && !defined(DEBUG_GENERIC_P)
  #define DEBUG_GENERIC_P( module, format, ... ) \
    do { \
      struct _DEBUG_TIME_STAMP st = debugTimeStamp(); \
      DEBUG_ESP_PORT.printf_P(PSTR( DEBUG_TIME_STAMP_FMT module " " format ), st.whole, st.dec, ##__VA_ARGS__ ); \
    } while(false)
#endif

/////////////////////////////////////////////

#if defined(DEBUG_GENERIC) && !defined(ASSERT_GENERIC)
  #define ASSERT_GENERIC( a, module ) \
    do { \
      if ( !(a) ) { \
        DEBUG_GENERIC( module, "%s:%s:%u: ASSERT("#a") failed!\n", __FILE__, __func__, __LINE__); \
        DEBUG_ESP_PORT.flush(); \
      } \
    } while(false)
#endif

/////////////////////////////////////////////

#if defined(DEBUG_GENERIC_P) && !defined(ASSERT_GENERIC_P)
  #define ASSERT_GENERIC_P( a, module ) \
    do { \
      if ( !(a) ) { \
        DEBUG_GENERIC_P( module, "%s:%s:%u: ASSERT("#a") failed!\n", __FILE__, __func__, __LINE__); \
        DEBUG_ESP_PORT.flush(); \
      } \
    } while(false)
#endif

/////////////////////////////////////////////

#ifndef DEBUG_GENERIC
  #define DEBUG_GENERIC(...) do { (void)0;} while(false)
#endif

/////////////////////////////////////////////

#ifndef DEBUG_GENERIC_P
  #define DEBUG_GENERIC_P(...) do { (void)0;} while(false)
#endif

/////////////////////////////////////////////

#ifndef ASSERT_GENERIC
  #define ASSERT_GENERIC(...) do { (void)0;} while(false)
#endif

/////////////////////////////////////////////

#ifndef ASSERT_GENERIC_P
  #define ASSERT_GENERIC_P(...) do { (void)0;} while(false)
#endif

/////////////////////////////////////////////

#ifndef DEBUG_ESP_PRINTF
  #define DEBUG_ESP_PRINTF( format, ...) DEBUG_GENERIC_P("[%s]", format, &_FILENAME_[1], ##__VA_ARGS__)
#endif

/////////////////////////////////////////////

#if defined(DEBUG_ESP_ASYNC_TCP) && !defined(ASYNC_TCP_DEBUG)
  #define ASYNC_TCP_DEBUG( format, ...) DEBUG_GENERIC_P("[ASYNC_TCP]", format, ##__VA_ARGS__)
#endif

/////////////////////////////////////////////

#ifndef ASYNC_TCP_ASSERT
  #define ASYNC_TCP_ASSERT( a ) ASSERT_GENERIC_P( (a), "[ASYNC_TCP]")
#endif

/////////////////////////////////////////////

#if defined(DEBUG_ESP_TCP_SSL) && !defined(TCP_SSL_DEBUG)
  #define TCP_SSL_DEBUG( format, ...) DEBUG_GENERIC_P("[TCP_SSL]", format, ##__VA_ARGS__)
#endif

/////////////////////////////////////////////

#endif //_DEBUG_PRINT_MACROS_H
