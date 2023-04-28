//***************************************************************************************
// PROJECT: CircusUart.cpp
// VERSION: V1.0 (Build:20220902)
// AUTHOR : Zack Huang
//***************************************************************************************
// Version History:
//
// V1.0: First Test Release
//
//
//***************************************************************************************

#include "CircusUart.h"

CircusUart::CircusUart(Stream &stream) : stream(stream), handler_idx(0)
{
}

CircusUart::~CircusUart()
{
}

uint8_t CircusUart::receive(void)
{
    static char       buffer[CIRCUS_RX_BUFF];
    static handler_t *handler = NULL;
    static int        len = 0, cr = false, state = RX_HEADER, ii = 0;
    static char       type = '\0';
    char              c;

    if (this->stream.available()) {
        switch (state) {
        default:
        case RX_HEADER:
            /* get header CR*/
            c = this->stream.read();
#if READ_DEBUG
            Serial.print(c);
#endif
            if (c == '\r') {
                cr = true;
            } else if (c == '\n') {
                if (cr) {
                    cr    = false;
                    state = RX_CONTENT;
                } else {
                    /* error */
                    state = RX_ERROR;
                }
            } else {
                state = RX_ERROR;
            }
            break;
        case RX_CONTENT:
            /* get content */
            buffer[len] = this->stream.read();
#if READ_DEBUG
            Serial.print(buffer[len]);
#endif
            if (buffer[len] == '\r') {
                type        = '\0';
                cr          = true;
                buffer[len] = '\0';
                state       = RX_FOOTER;
            } else {
                for (ii = 0; ii < this->handler_idx; ii++) {
                    if (buffer[len] == this->handlers[ii].separator) {
                        type        = this->handlers[ii].separator;
                        buffer[len] = '\0';
                        if (strcmp(this->handlers[ii].cmd, buffer) == 0) {
                            /* got it */
                            handler = (this->handlers + ii);
                            state   = RX_DATA;
                            break;
                        }
                    }
                }
                if (type != '\0' && handler == NULL) {
                    // Found separator but no find cmd
                    state = RX_ERROR;
                } else if (type == '\0' && handler == NULL) {
                    // no find separator and cmd
                    // normal character
                    len++;
                } else if (type != '\0' && handler != NULL) {
                    // found separator and cmd
                    len = 0;
                }
            }
            break;
        case RX_DATA:
            this->rec_buff[len] = this->stream.read();
#if READ_DEBUG
            Serial.print(this->rec_buff[len]);
#endif
            if (this->rec_buff[len] == '\r') {
                cr                  = true;
                this->rec_buff[len] = '\0';
                state               = RX_FOOTER;
            } else if (this->rec_buff[len] == '\n') {
                this->rec_buff[len] = '\0';
                state               = RX_ERROR;
            } else {
                // normal character
                len++;
            }
            break;
        case RX_FOOTER:
            /* get footer CR/LF */
            c = this->stream.read();
#if READ_DEBUG
            Serial.print(c);
#endif
            if (c == '\n') {
                if (cr) {
                    cr    = false;
                    len   = 0;
                    state = RX_HEADER;
                    // call callback
                    for (ii = 0; ii < this->handler_idx; ii++) {
                        if (type == this->handlers[ii].separator) {
                            if (strcmp(this->handlers[ii].cmd, buffer) == 0) {
                                /* got it */
                                (this->handlers + ii)->func(this->rec_buff);
                                this->rec_buff[0] = '\0';
                                break;
                            }
                        }
                    }
                    if (type != '\0' && handler == NULL) {
                        // Found separator but no find cmd
                        state = RX_ERROR;
                    }
                    type    = '\0';
                    handler = NULL;
                }
            } else {
                /* error */
                state = RX_ERROR;
            }
            break;
        case RX_ERROR:
            /* error state, flush all data */
            c = this->stream.read();     // dummy read
#if READ_DEBUG
            Serial.print(c);
#endif
            if (c == '\n') {
                cr                = false;
                len               = 0;
                state             = RX_HEADER;
                handler           = NULL;
                type              = '\0';
                this->rec_buff[0] = '\0';
            }
            break;
        }
#if READ_DEBUG
        Serial.print(state);
#endif
    }

    if (len) {
        return len;
    } else {
        return ERROR_UART_NO_DATA;
    }
}

void CircusUart::loop()
{
    this->receive();
}

uint16_t CircusUart::send(const char *cmd, const char separator, const char *param)
{
    uint16_t writeLen = 0;
    writeLen += this->stream.println();
    writeLen += this->stream.write(cmd, strlen(cmd));
    if (separator != '\0' && param != NULL) {
        writeLen += this->stream.write(separator);
        writeLen += this->stream.println(param);
    } else {
        writeLen += this->stream.println();
    }
    return writeLen;
}

void CircusUart::on(const char *cmd, const char separator, Func func)
{
    if (this->handler_idx < CIRCUS_HANDLER_MAX) {
        handler_t newHandler;
        newHandler.cmd                      = (char *)cmd;
        newHandler.separator                = separator;
        newHandler.func                     = func;
        this->handlers[this->handler_idx++] = newHandler;
    } else {
        // exceeds the maximum value
    }
}

size_t CircusUart::write(uint8_t data)
{
    return this->stream.write(data);
}

size_t CircusUart::write(const char *buffer, size_t size)
{
    return this->stream.write(buffer, size);
}
