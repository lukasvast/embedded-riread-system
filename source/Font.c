/**
@file Font.c
@brief Knjižnice funkcija za ispis teksta na LCD.
@author H. Reddmann
*/

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <avr/pgmspace.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "Graphic.h"
#include "Font.h"
#include "SSD1289x16.h"

/* _____LOCAL DEFINE_________________________________________________________ */
/// Header size of fonts
#define FONT_HEADER_SIZE            7     

/* _____LOCAL VARIABLES______________________________________________________ */
/// Font  Pointer
uint8_t     *FontPointer;    
/// Rot 0=0 degrees, 1=90 degrees 
uint8_t     rot = 0;            
/// Text FG color, 5-6-5 RGB
uint16_t    FgColor = WHITE;  
/// Text BK color, 5-6-5 RGB  
uint16_t    BkColor = WHITE;    
/// Text type 0=Proportional , 1=Fixed
uint8_t     FontFixed = 0;      
/// Transparent 0=No, 1=Yes
uint8_t     NonTransparence = 0;

 /// MAX width of font
uint8_t     FontWidth;    
/// MAX height of font     
uint8_t     FontHeight;    
/// X size of font     
uint8_t     FontXScale = 1;    
/// Y size of font 
uint8_t     FontYScale = 1;     
/// Space between char
uint8_t     FontSpace = 1;      

/// X position
unsigned int cursorX = 0;      
/// Y position 
unsigned int cursorY = 0;       

/* _____PRIVATE FUNCTIONS____________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
void LcdFont(uint8_t *pointer)
{
	FontPointer = pointer;
	FontWidth   = pgm_read_byte_near(&FontPointer[2]);
	FontHeight  = pgm_read_byte_near(&FontPointer[3]);
}

// Nacrtaj char
uint8_t DrawChar(unsigned char c)
{
	uint16_t    fontSize        =   pgm_read_word_near(&FontPointer[0]);
	uint8_t     firstchar       =   pgm_read_byte_near(&FontPointer[5]);
	uint8_t     lastchar        =   pgm_read_byte_near(&FontPointer[6]);
	uint8_t     charwidth       =   pgm_read_byte_near(&FontPointer[7+c-firstchar]);
	uint8_t     byte            =   0;
	uint8_t     bitoffset       =   0;
	uint8_t     maske           =   0;
	uint16_t    bcounter        =   0;
	uint16_t    bitsbischar     =   0;
	uint16_t    bytesbischar    =   0;
	uint16_t    xPos,yPos;
	uint16_t    color = GetColor(); // backup color
	
	if (c == '\n')
	{
		cursorY = cursorY + (unsigned int)FontHeight * FontYScale;
		cursorX = 0;
	}
	
	// character out of range.
	if( (c < firstchar) || (c > lastchar) || (fontSize == 0))
	return 0;
	
	// character is not in list.
	if (charwidth == 0)
	return 0;
	
	// sara thai font. line remain at last position
	if(FontFixed)
	charwidth = FontWidth;
	else
	{
		if (((c >= 0xd4) && (c <= 0xda)) ||
		((c >= 0xe7) && (c <= 0xec)) ||
		(c == 0xd1))
		{
			cursorX = cursorX - charwidth * FontXScale;
		}
	}
	
	// fixed width for digit
	if ( (c >= '0') && (c <= '9'))
	charwidth = pgm_read_byte_near(&FontPointer[7+'0'-firstchar]);
	
	// line adjust
	if (cursorX + charwidth * FontXScale > GetMaxX())
	{
		cursorY = cursorY + (unsigned int)FontHeight * FontYScale;
		cursorX = 0;
	}
	
	for(uint8_t Ccounter = 0; Ccounter < c-firstchar; Ccounter++)
	bitsbischar += (pgm_read_byte_near(&FontPointer[Ccounter+7]));
	
	
	bitsbischar *= FontHeight;
	bitsbischar += ((lastchar-firstchar)+7)*8;
	bytesbischar = bitsbischar/8;
	bitoffset = bitsbischar % 8;
	maske = bitoffset % 8;
	
	for(uint8_t xc = 0; xc < charwidth; xc++) // Eigentlcihe Ausgaberoutine
	{
		for(uint8_t yc = 0; yc < FontHeight; yc++)
		{
			if(maske > 7)
			{
				maske = 0;
				bcounter+=1;
			};
			byte = pgm_read_byte_near(&FontPointer[bytesbischar + bcounter]+1);
			
			xPos = (uint16_t)xc*FontXScale + cursorX;
			#ifdef Horizontal
			yPos = (((uint16_t)yc)*FontYScale + cursorY);
			#else
			//          yPos = GetMaxY() - (((uint16_t)yc)*FontYScale + cursorY);
			//          yPos = (((uint16_t)FontHeight-yc)*FontYScale + cursorY);
			yPos = ((yc)*FontYScale + cursorY);
			#endif
			for(uint8_t sx = 0; sx < FontXScale; sx++)
			{
				for(uint8_t sy = 0; sy < FontYScale; sy++)
				{
					if(bit_is_set(byte,maske))
					{
						SetColor(GetFgColor());
						if(rot)
						LCD_PutPixel(yPos +sy,xPos+sx);
						else
						LCD_PutPixel(xPos+sx,yPos+sy);
					}
					else
					{
						if(NonTransparence)
						{
							SetColor(GetBkColor());
							if(rot)
							LCD_PutPixel(yPos +sy,xPos+sx);
							else
							LCD_PutPixel(xPos+sx,yPos+sy);
						}
					}
				}
			}
			maske++;
		}
	}
	SetColor(color);
	
	// adjust cursor to next position
	cursorX += charwidth * FontXScale + FontSpace;
	
	return(charwidth * FontXScale + FontSpace);
}

unsigned int CalcTextWidth(char *Text)
{
	unsigned int strSize = 0;
	unsigned char firstchar =   pgm_read_byte_near(&FontPointer[5]);
	unsigned char lastchar  =   pgm_read_byte_near(&FontPointer[6]);
	unsigned char charwidth;
	unsigned char c;
	
	while(*Text)
	{
		c = *Text;
		
		if(FontFixed)
		strSize += FontWidth * FontXScale;
		else
		{
			if( (c < firstchar) || (c > lastchar))
			charwidth = FontWidth;
			else
			charwidth = pgm_read_byte_near(&FontPointer[7+c-firstchar]);
			strSize += charwidth * FontXScale;
		}
		strSize += FontSpace;
		Text++;
	}
	return strSize ;
}

unsigned int CalcTextHeight(char *Text)
{
	return FontHeight * FontYScale;
}

// Nacrtaj string
unsigned int DrawStr(char *Text)
{
	char c;
	uint8_t sizeofstring = 0;
	uint8_t counter = 0;
	
	while((c = Text[counter]))
	{
		sizeofstring += DrawChar(c);
		counter++;
	}
	return sizeofstring;
}

// Nacrtaj tekst
void DrawText(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, char *Text, uint8_t aline)
{
	uint16_t sizeofstring;
	
	sizeofstring = CalcTextWidth(Text);
	top = top + (bottom - top - FontHeight)/2;
	
	switch (aline)
	{
		case ALINE_LEFT:
		break;
		
		case ALINE_CENTER:
		left = left + (right - left - sizeofstring) / 2;
		break;
		
		case ALINE_RIGHT:
		if (right > sizeofstring)
		left = right - sizeofstring;
		break;
	}
	
	SetCursorX(left);
	SetCursorY(top);
	DrawStr(Text);
}
