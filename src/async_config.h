/****************************************************************************************************************************
  async_config.h
  
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

#ifndef _PORTENTA_H7_ASYNC_CONFIG_H_
#define _PORTENTA_H7_ASYNC_CONFIG_H_

#ifndef TCP_MSS
  // May have been definded as a -DTCP_MSS option on the compile line or not.
  // Arduino core 2.3.0 or earlier does not do the -DTCP_MSS option.
  // Later versions may set this option with info from board.txt.
  // However, Core 2.4.0 and up board.txt does not define TCP_MSS for lwIP v1.4
  #define TCP_MSS       MBED_CONF_LWIP_TCP_MSS        //(1460)
#endif

#endif // _PORTENTA_H7_ASYNC_CONFIG_H_
