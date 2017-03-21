/**
@file Usart.h
@brief Header knjižnice funkcija za USART.
*/

#ifndef USART_H_
#define USART_H_

void UsartInit(uint16_t ubrr_value);
char USARTReadChar();

#endif //USART_H_
