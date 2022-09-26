/****************************************************************************************************************************
  AsyncPrinter.h
  
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
/*
  Asynchronous TCP library for Espressif MCUs

  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ASYNCPRINTER_H_
#define ASYNCPRINTER_H_

#include "Arduino.h"
#include "Portenta_H7_AsyncTCP.h"
#include "cbuf.h"

/////////////////////////////////////////////

class AsyncPrinter;

typedef std::function<void(void*, AsyncPrinter*, uint8_t*, size_t)> ApDataHandler;
typedef std::function<void(void*, AsyncPrinter*)> ApCloseHandler;

/////////////////////////////////////////////

class AsyncPrinter: public Print 
{
  private:
    AsyncClient *_client;
    ApDataHandler _data_cb;
    void *_data_arg;
    ApCloseHandler _close_cb;
    void *_close_arg;
    cbuf *_tx_buffer;
    size_t _tx_buffer_size;

    void _onConnect(AsyncClient *c);
    
  public:
    AsyncPrinter *next;

    AsyncPrinter();
    AsyncPrinter(AsyncClient *client, size_t txBufLen = TCP_MSS);
    virtual ~AsyncPrinter();

    int connect(IPAddress ip, uint16_t port);
    int connect(const char *host, uint16_t port);

    void onData(ApDataHandler cb, void *arg);
    void onClose(ApCloseHandler cb, void *arg);

    operator bool();
    AsyncPrinter & operator=(const AsyncPrinter &other);

    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t len);

    bool connected();
    void close();

    size_t _sendBuffer();
    void _onData(void *data, size_t len);
    void _on_close();
    void _attachCallbacks();
};

/////////////////////////////////////////////

#endif /* ASYNCPRINTER_H_ */
