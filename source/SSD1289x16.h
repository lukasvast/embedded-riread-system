 /**
 @file SSD1289x16.h
 @brief Header knjižnice funkcija, definicija kontrolnih i podatkovnih pinova za 3.2" TFT LCD.
 @author Jaruwit Supa
 */
 
#ifndef __SSD1289X16_H__
#define __SSD1289X16_H__
#include <avr/io.h>


// Makroi za I/O registre
#define Orb(port,bitnum)		port |= _BV(bitnum)
#define Setb(port,bitnum)		port |= _BV(bitnum)
#define Clrb(port,bitnum)		port &= ~(_BV(bitnum))
#define Rdb(pinp,bitnum)		(pinp & _BV(bitnum))

/* _____HARDWARE DEFINES_____________________________________________________ */

// LCD kontrolni pinovi
#define LCD_RST_DPRT  DDRD
#define LCD_RST_PRTS PORTD
#define LCD_RST_PRTC PORTD
#define LCD_RST_PIN  PD7

#define LCD_RS_DPRT  DDRD
#define LCD_RS_PRTS PORTD
#define LCD_RS_PRTC	PORTD
#define LCD_RS_PIN  PD6

#define LCD_CS_DPRT  DDRD
#define LCD_CS_PRTS PORTD
#define LCD_CS_PRTC	PORTD
#define LCD_CS_PIN  PD3

#define LCD_RD_DPRT  DDRD
#define LCD_RD_PRTS PORTD
#define LCD_RD_PRTC	PORTD
#define LCD_RD_PIN  PD4

#define LCD_WR_DPRT  DDRD
#define LCD_WR_PRTS PORTD
#define LCD_WR_PRTC	PORTD
#define LCD_WR_PIN  PD5

// Podatkovni pinovi
#define LCD_LO_DDR  DDRC
#define LCD_LO_PORT PORTC
#define LCD_LO_PIN  PINC

#define LCD_HI_DDR  DDRA
#define LCD_HI_PORT PORTA
#define LCD_HI_PIN  PINA

// Definicije za touch screen
/*
#define TC_PEN_DPRT		DDRB  
#define TC_PEN_PRTS		PORTB
#define TC_PEN_PRTC		PORTB
#define TC_PEN_PINP		PINB
#define TC_PEN_PIN		PB2

#define TC_CS_DPRT		DDRB
#define TC_CS_PRTS		PORTB
#define TC_CS_PRTC		PORTB
#define TC_CS_PIN		PB4

#define MOSI_DPRT		DDRB
#define MOSI_PRTS		PORTB
#define MOSI_PRTC		PORTB
#define MOSI_PIN		PB5

#define SCK_DPRT		DDRB
#define SCK_PRTS		PORTB
#define SCK_PRTC		PORTB
#define SCK_PIN			PB7
*/

#define LCD_INPUT()		{LCD_LO_DDR = 0x00; LCD_HI_DDR = 0x00;}
#define LCD_OUTPUT()	{LCD_LO_DDR = 0xFF; LCD_HI_DDR = 0xFF;}

#define LCD_SET_DBH		LCD_HI_PORT
#define LCD_SET_DBL		LCD_LO_PORT

#define LCD_GET_DBH()	LCD_HI_PIN
#define LCD_GET_DBL()	LCD_LO_PIN

#define LCD_SET_CS()  LCD_CS_PRTS  |=   1<<LCD_CS_PIN
#define LCD_CLR_CS()  LCD_CS_PRTS  &= ~(1<<LCD_CS_PIN)
#define LCD_DIR_CS(x) LCD_CS_DPRT   |=   1<<LCD_CS_PIN

#define LCD_SET_RS()  LCD_RS_PRTS  |=   1<<LCD_RS_PIN
#define LCD_CLR_RS()  LCD_RS_PRTS  &= ~(1<<LCD_RS_PIN)
#define LCD_DIR_RS(x) LCD_RS_DPRT   |=   1<<LCD_RS_PIN

#define LCD_SET_WR()  LCD_WR_PRTS  |=   1<<LCD_WR_PIN
#define LCD_CLR_WR()  LCD_WR_PRTS  &= ~(1<<LCD_WR_PIN)
#define LCD_DIR_WR(x) LCD_WR_DPRT   |=   1<<LCD_WR_PIN

#define LCD_SET_RD()  LCD_RD_PRTS  |=   1<<LCD_RD_PIN
#define LCD_CLR_RD()  LCD_RD_PRTS  &= ~(1<<LCD_RD_PIN)
#define LCD_DIR_RD(x) LCD_RD_DPRT   |=   1<<LCD_RD_PIN

#define LCD_SET_RST()  LCD_RST_PRTS |=  1<<LCD_RST_PIN
#define LCD_CLR_RST()  LCD_RST_PRTS &= ~(1<<LCD_RST_PIN)
#define LCD_DIR_RST(x) LCD_RST_DPRT  |=  1<<LCD_RST_PIN

/* _____PUBLIC DEFINE_____________________________________________________ */
#define Horizontal

#ifdef Horizontal
// Horizontalna i vertikalna velièina ekrana
#define SCREEN_HOR_SIZE    240UL
#define SCREEN_VER_SIZE    320UL

#else
// Horizontalna i vertikalna velièina ekrana
#define SCREEN_HOR_SIZE    320UL
#define SCREEN_VER_SIZE    240UL

#endif

// Paleta definiranih boja
#define BLACK                       RGB(0x00, 0x00, 0x00)
#define WHITE                       RGB(0xFF, 0xFF, 0xFF)
#define RED                         RGB(0xFF, 0x00, 0x00)
#define GREEN                       RGB(0x33, 0xFF, 0x33)
#define BLUE                        RGB(0x00, 0x00, 0xFF)
#define YELLOW                      RGB(0xFF, 0xFF, 0x00)
#define MAGENTA                     RGB(0xFF, 0x00, 0xFF)
#define CYAN                        RGB(0x00, 0xFF, 0xFF)
#define GRAY                        RGB(0x80, 0x80, 0x40)
#define SILVER                      RGB(0xA0, 0xA0, 0x80)
#define GOLD                        RGB(0xA0, 0xA0, 0x40)
#define LIGHTBLUE					RGB(0xAC, 0xD1, 0xE9)
#define ORANGE						RGB(0xE0, 0x76, 0x28)
#define MAROON						RGB(0x51, 0x00, 0x00)
#define DARKBLUE					RGB(0x00, 0x00, 0x22)
#define LIGHTORANGE					RGB(0xFE, 0xC1, 0xA4)
#define DARKMAGENTA					RGB(0x73, 0x32, 0x73)

/* _____PUBLIC VARIABLE_____________________________________________________ */
extern unsigned int _color;

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
extern void LCD_Reset(void);
extern void LCD_SetCursor(unsigned int x, unsigned int y);
extern void LCD_SetArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
extern void LCD_PutPixel(unsigned int x, unsigned int y);
extern void LCD_Bar(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int color);
extern void LCD_Clear(unsigned int color);
extern void LCD_DrawSymbol(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char t, const unsigned char *pImage);
extern void LCD_DrawImage(unsigned int x, unsigned int y, const unsigned char *pImage);

/* _____DEFINE MACRO_________________________________________________________ */
#define GetMaxX() 		((unsigned int)SCREEN_HOR_SIZE-1)
#define GetMaxY() 		((unsigned int)SCREEN_VER_SIZE-1)

#define RGB(red, green, blue)	((unsigned int)( (( red >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 )))

#define SetColor(color) _color = color
#define GetColor()      _color

#endif

