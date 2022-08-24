#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"
#include "stm32l4xx_hal.h"
#include "queue"
using namespace std;

//#ifndef RXBUFFERSIZE
#define AT_OFFSET 50
#define RXBUFFERSIZE 1024+AT_OFFSET // 50 extra bytes for AT commands
//#define RXBUFFERSIZE 255+AT_OFFSET // 50 extra bytes for AT commands
/* Those extra bytes are important for BC660,
in which it sends always a fixed payload with 1024 bytes
for http requests in case of overload
*/
//#endif

#ifdef __cplusplus
 extern "C" {
#endif

class Serial{
public:

  Serial();
  void begin(uint8_t n_uart,uint32_t baudrate);
  void end(uint8_t n_uart);
  bool read_byte(uint8_t* byte); // read last_byte
  bool clear();  // discard all data in buffer
  uint16_t read(uint8_t* data, uint16_t size); // read all available data and returns the data read, size and size available on buffer
  uint16_t available();  // return available data
  void write(char* data, uint16_t size);
  void write(uint8_t* data, uint16_t size);
  //uint8_t write(std::queue<char> *buffer_tx);
  void flush();


private:

	uint8_t aRxBuffer[RXBUFFERSIZE];
  uint16_t rx_pos;
  UART_HandleTypeDef* huart = NULL; // debug

  void log(const char* text);
};



#ifdef __cplusplus
}
#endif

#endif
