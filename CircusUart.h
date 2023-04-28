//***************************************************************************************
// PROJECT: CircusUart.h
// VERSION: V1.0 (Build:20220902)
// AUTHOR : Zack Huang
//***************************************************************************************
// Version History:
//
// V1.0: First Test Release
//
//
//***************************************************************************************

#include "Arduino.h"
#ifndef _CIRCUS_UART_H
#define _CIRCUS_UART_H

#define READ_DEBUG false
#define CIRCUS_HANDLER_MAX 16
#define CIRCUS_RX_BUFF 32

typedef void (*Func)(const char *data);

typedef enum
{
    RX_HEADER,
    RX_CONTENT,
    RX_DATA,
    RX_FOOTER,
    RX_ERROR,
    RX_MAX
} RX_STATE_E;

typedef enum
{
    ERROR_UART_NONE,
    ERROR_UART_NO_DATA,
} ERROR_CODE_E;

typedef struct {
    char *cmd;
    char  separator;
    Func  func;
} handler_t;

class CircusUart : public Print {
  private:
    Stream   &stream;
    uint8_t   receive(void);
    handler_t handlers[CIRCUS_HANDLER_MAX];
    uint8_t   handler_idx;
    char      rec_buff[CIRCUS_RX_BUFF];

  public:
    CircusUart(Stream &stream);
    ~CircusUart();
    void     loop();
    uint16_t send(const char *cmd, const char separator = '\0', const char *param = NULL);
    void     on(const char *cmd, const char separator, Func func);
    size_t   write(uint8_t);
    size_t   write(const char *buffer, size_t size);
};

#endif