

#include "serial.h"
#include "string.h"
#include "malloc.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef hlpuart1;

extern UART_HandleTypeDef huart3; // debug

Serial::Serial(){
  memset(aRxBuffer, 0, RXBUFFERSIZE);
  rx_pos = 0;
}

void Serial::begin(uint8_t n_uart,uint32_t baudrate){
  if(n_uart == 0){
	  huart = &hlpuart1;
	  huart->Init.BaudRate = baudrate;
	  // call it only once

	  HAL_UART_AbortReceive_IT(huart); // this solves overflow issues
	  huart->RxXferCount = huart->RxXferSize;

	 if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	 {
		 printf("USARTLP: error while initing  HAL_UART_Receive_IT function\n");
		 //Error_Handler(__FILE__, __LINE__);
	 }

  }else if(n_uart == 1){
    huart = &huart1;
    huart->Init.BaudRate = baudrate;
    // call it only once

    if (HAL_UART_Init(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }

    HAL_UART_AbortReceive_IT(huart); // this solves overflow issues
    huart->RxXferCount = huart->RxXferSize;


  	HAL_UART_AbortTransmit_IT(huart);
 	 if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
 	 {
       printf("USART1: error while initing  HAL_UART_Receive_IT function\n");
 	    //Error_Handler(__FILE__, __LINE__);
 	 }

	  //MX_USART1_UART_Init();
  }else if(n_uart == 2){
    huart = &huart2;
    huart->Init.BaudRate = baudrate;
    //MX_USART2_UART_Init();
  }else if(n_uart == 3){
    //MX_USART3_UART_Init();
	huart = &huart3;
	huart->Init.BaudRate = baudrate;

	HAL_UART_AbortReceive_IT(huart); // this solves overflow issues
	huart->RxXferCount = huart->RxXferSize;


	HAL_UART_AbortTransmit_IT(&huart3);

	if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	{
		printf("USART3: error while initing  HAL_UART_Receive_IT function\n");
		//Error_Handler(__FILE__, __LINE__);
	}

  }else if(n_uart == 4){
    huart = &huart4;
	  huart->Init.BaudRate = baudrate;
    // call it only once

 	 if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
 	 {
       printf("USART4: error while initing  HAL_UART_Receive_IT function\n");
 	    //Error_Handler(__FILE__, __LINE__);
 	 }

	  //MX_USART1_UART_Init();
  }

  /*
  char data[50];
  sprintf(data,"uart: %d baudrate %lu \n",n_uart,baudrate);
  HAL_UART_Transmit(&huart2,(uint8_t*)data,50,100);
  */
}

void Serial::end(uint8_t n_uart){

  if(n_uart == 0){
	  huart = &hlpuart1;
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }

  }else if(n_uart == 1){
    huart = &huart1;
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }
  }else if(n_uart == 2){
    huart = &huart2;
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }
  }else if(n_uart == 3){
    huart = &huart3;
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }

  }else if(n_uart == 4){
    huart = &huart4;
    if (HAL_UART_DeInit(huart) != HAL_OK)
    {
      Error_Handler(__FILE__, __LINE__);
    }
  }

}

void Serial::flush(){

	return;
}


void Serial::write(char* data, uint16_t size){

	write((uint8_t*)data,size);
}

void Serial::write(uint8_t* data, uint16_t size){

	if(size > 0)
		HAL_UART_Transmit(huart,data,size,3000);

	return;
}

/*
uint8_t Serial::write(std::queue<char> *buffer_tx){
	uint8_t* p_buffer = (uint8_t*)malloc((unsigned int)buffer_tx->size());
	uint8_t buffer_size = 0;

	if(p_buffer != NULL){
		while(buffer_tx->size() > 0){
			p_buffer[buffer_size] = (uint8_t)buffer_tx->front();
			buffer_tx->pop();
			buffer_size++;
		}
	}

	if(buffer_size > 0)
		HAL_UART_Transmit(huart,p_buffer,buffer_size,10);
	free(p_buffer);
	return (buffer_size-buffer_tx->size()); // I am not sure if the data is transmitted
}
*/

uint16_t Serial::available(){
  //return aRxBuffer[0];
  return (uint16_t)(huart->RxXferSize-huart->RxXferCount);
}

bool Serial::read_byte(uint8_t *byte){


  //if(rx_pos > huart->RxXferCount){
  if(rx_pos < (huart->RxXferSize-huart->RxXferCount)){
    *byte = aRxBuffer[rx_pos++];
    return true;
  }

  return false;
}

bool Serial::clear(){

  huart->RxXferCount = huart->RxXferSize;
  rx_pos = 0;

  HAL_UART_AbortReceive_IT(huart); // this solves overflow issues
  if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
     printf("error while initing  HAL_UART_Receive_IT function\n");
     //Error_Handler(__FILE__, __LINE__);
  }

  return true;
}

// this function reads from buffer and stores in a queue returning the number of bytes in that queue
uint16_t Serial::read(uint8_t* data, uint16_t buff_size){

  // check if data was received
  // an interrupt should be called when one byte is received, not when buffer is full


  __HAL_LOCK(huart);

  uint16_t size = 0;
  while(size < buff_size && rx_pos < (huart->RxXferSize-huart->RxXferCount) ){
    data[size++] = aRxBuffer[rx_pos++];
  }

  if(rx_pos == (huart->RxXferSize-huart->RxXferCount)){  // all buffer was read
      huart->RxXferCount = huart->RxXferSize;
      rx_pos = 0;
  }


  __HAL_UNLOCK(huart);

  HAL_UART_AbortReceive_IT(huart); // this solves overflow issues
  rx_pos = 0;


  if(HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK){
     printf("error while initing  HAL_UART_Receive_IT function\n");
     //Error_Handler(__FILE__, __LINE__);
  }


  return size;
}
