/**
@file Usart.c
@brief Knji�nica funkcija za USART.
*/

#include <avr/io.h>

/**
@brief Inicijalizacija USART-a (USART se mora inicijalizirati prije bilo kakve komunikacije)
@param ubrrValue - UBRR vrijednost (fosc = 8.000MHz, Baud rate 9600bps --> UBRR = 51)
@details USART se mora inicijalizirati prije bilo kakve komunikacije.
UCSRC registar:
				- URSEL bit postavljen u 1 odre�uje pristupanje UCSRC registru
				- UCSZ1:0 bitovi postavljaju broj kori�tenih podatkovnih bitova (Character Size) u okvirima koje koriste receiver i transmitter 

UCSRB registar: - RXEN - postavljanje ovog bita u jedinicu omogu�ava USART receiver
				- TXEN - postavljanje ovog bita u jedinicu omogu�ava USART transmitter
*/
void UsartInit(uint16_t ubrrValue) {

	UBRRL = ubrrValue; 
	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	UCSRB = (1<<RXEN)|(1<<TXEN);

}

/**
Funkcija za �itanje karaktera.
@brief Funkcija "�eka podatke" u receive bufferu tako da provjerava RXC zastavicu, prije �itanja buffera i vra�anja povratne vrijednosti.
*/
char USARTReadChar() {
	// Wait for data to be received
	while(!(UCSRA & (1<<RXC)))
	{
		//Do nothing

	}
	// Get and return received data from buffer
	return UDR;

}
