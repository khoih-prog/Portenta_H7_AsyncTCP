/****************************************************************************************************************************
  Portenta_H7_AsyncTCP.h
  
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

#ifndef PORTENTA_H7_ASYNCTCP_H_
#define PORTENTA_H7_ASYNCTCP_H_

/////////////////////////////////////////////

#if ( defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4) )

  #if defined(BOARD_NAME)
    #undef BOARD_NAME
  #endif

  #if defined(CORE_CM7)
    //#warning Using Portenta H7 M7 core
    #define BOARD_NAME              "PORTENTA_H7_M7"
  #else
    //#warning Using Portenta H7 M4 core
    #define BOARD_NAME              "PORTENTA_H7_M4"
  #endif

#else

  #error For Portenta_H7 only
    
#endif

/////////////////////////////////////////////

#include "Arduino.h"

/////////////////////////////////////////////

#if (ETHERNET_USE_PORTENTA_H7 || USE_ETHERNET_PORTENTA_H7)
  #define SHIELD_TYPE           "Ethernet using Portenta_Ethernet Library"
  #include <EthernetClient.h>
#elif (WIFI_USE_PORTENTA_H7 || USE_WIFI_PORTENTA_H7)
  #define SHIELD_TYPE           "Portenta_H7 WiFi"
  #include <WiFiClient.h>
#else
  // Default WiFi if not specified
  #define SHIELD_TYPE           "Portenta_H7 WiFi"
  #include <WiFiClient.h>
#endif

/////////////////////////////////////////////

#define PORTENTA_H7_ASYNC_TCP_VERSION            "Portenta_H7_AsyncTCP v1.4.0"

#define PORTENTA_H7_ASYNC_TCP_VERSION_MAJOR      1
#define PORTENTA_H7_ASYNC_TCP_VERSION_MINOR      4
#define PORTENTA_H7_ASYNC_TCP_VERSION_PATCH      0

#define PORTENTA_H7_ASYNC_TCP_VERSION_INT        1004000

/////////////////////////////////////////////

#if ASYNC_TCP_SSL_ENABLED
  #undef ASYNC_TCP_SSL_ENABLED
  #define ASYNC_TCP_SSL_ENABLED			false
  
  #warning ASYNC_TCP_SSL_ENABLED is not ready yet. Disable it
#endif

#define DEBUG_ESP_ASYNC_TCP       true

/////////////////////////////////////////////

#include "Portenta_H7_AsyncTCP_Debug.h"

#define MBED_CONF_LWIP_IPV4_ENABLED     1

#include <async_config.h>
#include "IPAddress.h"
#include <functional>
#include <memory>

extern "C"
{
  #include "lwip/ip_addr.h"
  #include "lwip/err.h"
  #include "lwip/tcp.h"
  #include "lwip/pbuf.h"
};

#ifndef PORTENTA_H7_ATCP_UNUSED
  #define PORTENTA_H7_ATCP_UNUSED(x)       (void)(x)
#endif

/////////////////////////////////////////////

class AsyncClient;
class AsyncServer;
class ACErrorTracker;

#define ASYNC_MAX_ACK_TIME      5000
#define ASYNC_WRITE_FLAG_COPY   0x01    //to allocate new buffer to hold the data while sending
#define ASYNC_WRITE_FLAG_MORE   0x02    //not send PSH flag. More data to be sent before the application should react.

/////////////////////////////////////////////

struct tcp_pcb;

#if ASYNC_TCP_SSL_ENABLED
  struct SSL_;
  typedef struct SSL_ SSL;
  struct SSL_CTX_;
  typedef struct SSL_CTX_ SSL_CTX;
#endif

typedef std::function<void(void*, AsyncClient*)> AcConnectHandler;
typedef std::function<void(void*, AsyncClient*, size_t len, uint32_t time)> AcAckHandler;
typedef std::function<void(void*, AsyncClient*, err_t error)> AcErrorHandler;
typedef std::function<void(void*, AsyncClient*, void *data, size_t len)> AcDataHandler;
typedef std::function<void(void*, AsyncClient*, struct pbuf *pb)> AcPacketHandler;
typedef std::function<void(void*, AsyncClient*, uint32_t time)> AcTimeoutHandler;
typedef std::function<void(void*, size_t event)> AsNotifyHandler;

enum error_events 
{
  EE_OK = 0,
  EE_ABORTED,       // Callback or foreground aborted connections
  EE_ERROR_CB,      // Stack initiated aborts via error Callbacks.
  EE_CONNECTED_CB,
  EE_RECV_CB,
  EE_ACCEPT_CB,
  EE_MAX
};

/////////////////////////////////////////////

// DEBUG_MORE is for gathering more information on which CBs close events are
// occuring and count.
// #define DEBUG_MORE 1
class ACErrorTracker 
{
  private:
    AsyncClient *_client;
    err_t _close_error;
    int _errored;
    
#if DEBUG_ESP_ASYNC_TCP
    size_t _connectionId;
#endif

#ifdef DEBUG_MORE
    AsNotifyHandler _error_event_cb;
    void* _error_event_cb_arg;
#endif

  protected:
    friend class AsyncClient;
    friend class AsyncServer;
    
#ifdef DEBUG_MORE
    void onErrorEvent(AsNotifyHandler cb, void *arg);
#endif

#if DEBUG_ESP_ASYNC_TCP
    void setConnectionId(size_t id) 
    {
      _connectionId = id;
    }
    
    size_t getConnectionId() 
    {
      return _connectionId;
    }
#endif

    void setCloseError(err_t e);
    void setErrored(size_t errorEvent);
    err_t getCallbackCloseError();
    
    void clearClient() 
    {
      if (_client) _client = NULL;
    }

  public:
    err_t getCloseError() const 
    {
      return _close_error;
    }
    
    bool hasClient() const 
    {
      return (_client != NULL);
    }
    
    ACErrorTracker(AsyncClient *c);
    ~ACErrorTracker() {}
};

/////////////////////////////////////////////

class AsyncClient 
{
  protected:
    friend class AsyncTCPbuffer;
    friend class AsyncServer;
    tcp_pcb* _pcb;
    AcConnectHandler _connect_cb;
    void* _connect_cb_arg;
    AcConnectHandler _discard_cb;
    void* _discard_cb_arg;
    AcAckHandler _sent_cb;
    void* _sent_cb_arg;
    AcErrorHandler _error_cb;
    void* _error_cb_arg;
    AcDataHandler _recv_cb;
    void* _recv_cb_arg;
    AcPacketHandler _pb_cb;
    void* _pb_cb_arg;
    AcTimeoutHandler _timeout_cb;
    void* _timeout_cb_arg;
    AcConnectHandler _poll_cb;
    void* _poll_cb_arg;
    bool _pcb_busy;
    
#if ASYNC_TCP_SSL_ENABLED
    bool _pcb_secure;
    bool _handshake_done;
#endif

    uint32_t  _pcb_sent_at;
    bool      _close_pcb;
    bool      _ack_pcb;
    uint32_t  _tx_unacked_len;
    uint32_t  _tx_acked_len;
    uint32_t  _tx_unsent_len;
    uint32_t  _rx_ack_len;
    uint32_t  _rx_last_packet;
    uint32_t  _rx_since_timeout;
    uint32_t  _ack_timeout;
    uint16_t  _connect_port;
    u8_t      _recv_pbuf_flags;
    std::shared_ptr<ACErrorTracker> _errorTracker;

    void _close();
    void _connected(std::shared_ptr<ACErrorTracker>& closeAbort, void* pcb, err_t err);
    void _error(err_t err);
    
#if ASYNC_TCP_SSL_ENABLED
    void _ssl_error(int8_t err);
#endif

    void _poll(std::shared_ptr<ACErrorTracker>& closeAbort, tcp_pcb* pcb);
    void _sent(std::shared_ptr<ACErrorTracker>& closeAbort, tcp_pcb* pcb, uint16_t len);
    
#if LWIP_VERSION_MAJOR == 1
    void _dns_found(struct ip_addr *ipaddr);
#else
    void _dns_found(ip_addr_t *p);
#endif

    static err_t _s_poll(void *arg, struct tcp_pcb *tpcb);
    static err_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err);
    static void _s_error(void *arg, err_t err);
    static err_t _s_sent(void *arg, struct tcp_pcb *tpcb, uint16_t len);
    static err_t _s_connected(void* arg, void* tpcb, err_t err);
    
#if LWIP_VERSION_MAJOR == 1
    static void _s_dns_found(const char *name, struct ip_addr *ipaddr, void *arg);
#else
    static void _s_dns_found(const char *name, ip_addr_t *p, void *arg);
#endif

#if ASYNC_TCP_SSL_ENABLED
    static void _s_data(void *arg, struct tcp_pcb *tcp, uint8_t * data, size_t len);
    static void _s_handshake(void *arg, struct tcp_pcb *tcp, SSL *ssl);
    static void _s_ssl_error(void *arg, struct tcp_pcb *tcp, int8_t err);
#endif

    std::shared_ptr<ACErrorTracker> getACErrorTracker() const 
    {
      return _errorTracker;
    };
    
    void setCloseError(err_t e) const 
    {
      _errorTracker->setCloseError(e);
    }

  public:
    AsyncClient* prev;
    AsyncClient* next;

#if ASYNC_TCP_SSL_ENABLED
    AsyncClient(tcp_pcb* pcb = 0, SSL_CTX * ssl_ctx = NULL);
#else
    AsyncClient(tcp_pcb* pcb = 0);
#endif

    ~AsyncClient();

    AsyncClient & operator=(const AsyncClient &other);
    AsyncClient & operator+=(const AsyncClient &other);

    bool operator==(const AsyncClient &other);

    bool operator!=(const AsyncClient &other) 
    {
      return !(*this == other);
    }
    
#if ASYNC_TCP_SSL_ENABLED
    bool connect(IPAddress ip, uint16_t port, bool secure = false);
    bool connect(const char* host, uint16_t port, bool secure = false);
#else
    bool connect(IPAddress ip, uint16_t port);
    bool connect(const char* host, uint16_t port);
#endif

    void close(bool now = false);
    void stop();
    void abort();
    bool free();

    bool canSend();//ack is not pending
    size_t space();
    size_t add(const char* data, size_t size, uint8_t apiflags = 0); //add for sending
    bool send();//send all data added with the method above
    size_t ack(size_t len); //ack data that you have not acked using the method below
    
    void ackLater() 
    {
      _ack_pcb = false;  //will not ack the current packet. Call from onData
    }
    
    bool isRecvPush() 
    {
      return !!(_recv_pbuf_flags & PBUF_FLAG_PUSH);
    }
    
#if DEBUG_ESP_ASYNC_TCP
    size_t getConnectionId() const 
    {
      return _errorTracker->getConnectionId();
    }
#endif

#if ASYNC_TCP_SSL_ENABLED
    SSL *getSSL();
#endif

    size_t write(const char* data);
    size_t write(const char* data, size_t size, uint8_t apiflags = 0); //only when canSend() == true

    uint8_t state();
    bool    connecting();
    bool    connected();
    bool    disconnecting();
    bool    disconnected();
    bool    freeable();//disconnected or disconnecting

    uint16_t  getMss();
    uint32_t  getRxTimeout();
    void      setRxTimeout(uint32_t timeout);//no RX data timeout for the connection in seconds
    uint32_t  getAckTimeout();
    void      setAckTimeout(uint32_t timeout);//no ACK timeout for the last sent packet in milliseconds
    void      setNoDelay(bool nodelay);
    bool      getNoDelay();
    uint32_t  getRemoteAddress();
    uint16_t  getRemotePort();
    uint32_t  getLocalAddress();
    uint16_t  getLocalPort();

    IPAddress remoteIP();
    uint16_t  remotePort();
    IPAddress localIP();
    uint16_t  localPort();

    void onConnect(AcConnectHandler cb, void* arg = 0);     //on successful connect
    void onDisconnect(AcConnectHandler cb, void* arg = 0);  //disconnected
    void onAck(AcAckHandler cb, void* arg = 0);             //ack received
    void onError(AcErrorHandler cb, void* arg = 0);         //unsuccessful connect or error
    void onData(AcDataHandler cb, void* arg = 0);           //data received (called if onPacket is not used)
    void onPacket(AcPacketHandler cb, void* arg = 0);       //data received
    void onTimeout(AcTimeoutHandler cb, void* arg = 0);     //ack timeout
    void onPoll(AcConnectHandler cb, void* arg = 0);        //every 125ms when connected
    void ackPacket(struct pbuf * pb);

    const char * errorToString(err_t error);
    const char * stateToString();

    void _recv(std::shared_ptr<ACErrorTracker>& closeAbort, tcp_pcb* pcb, pbuf* pb, err_t err);
    
    err_t getCloseError() const 
    {
      return _errorTracker->getCloseError();
    }
};

/////////////////////////////////////////////

#if ASYNC_TCP_SSL_ENABLED
  typedef std::function<int(void* arg, const char *filename, uint8_t **buf)> AcSSlFileHandler;
  struct pending_pcb;
#endif

/////////////////////////////////////////////

class AsyncServer 
{
  protected:
    uint16_t _port;
    IPAddress _addr;
    bool _noDelay;
    tcp_pcb* _pcb;
    AcConnectHandler _connect_cb;
    void* _connect_cb_arg;
    
#if ASYNC_TCP_SSL_ENABLED
    struct pending_pcb * _pending;
    SSL_CTX * _ssl_ctx;
    AcSSlFileHandler _file_cb;
    void* _file_cb_arg;
#endif

#ifdef DEBUG_MORE
    int _event_count[EE_MAX];
#endif

  public:

    AsyncServer(IPAddress addr, uint16_t port);
    AsyncServer(uint16_t port);
    ~AsyncServer();
    void onClient(AcConnectHandler cb, void* arg);
    
#if ASYNC_TCP_SSL_ENABLED
    void onSslFileRequest(AcSSlFileHandler cb, void* arg);
    void beginSecure(const char *cert, const char *private_key_file, const char *password);
#endif

    void begin();
    void end();
    void setNoDelay(bool nodelay);
    bool getNoDelay();
    uint8_t status();
    
#ifdef DEBUG_MORE
    int getEventCount(size_t ee) const 
    {
      return _event_count[ee];
    }
#endif

  protected:
    err_t _accept(tcp_pcb* newpcb, err_t err);
    static err_t _s_accept(void *arg, tcp_pcb* newpcb, err_t err);
    
#ifdef DEBUG_MORE
    int incEventCount(size_t ee) 
    {
      return ++_event_count[ee];
    }
#endif

#if ASYNC_TCP_SSL_ENABLED
    int _cert(const char *filename, uint8_t **buf);
    err_t _poll(tcp_pcb* pcb);
    err_t _recv(tcp_pcb *pcb, struct pbuf *pb, err_t err);
    static int _s_cert(void *arg, const char *filename, uint8_t **buf);
    static err_t _s_poll(void *arg, struct tcp_pcb *tpcb);
    static err_t _s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pb, err_t err);
#endif
};

/////////////////////////////////////////////

#endif /* PORTENTA_H7_ASYNCTCP_H_ */
