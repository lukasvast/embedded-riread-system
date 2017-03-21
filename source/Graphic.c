/**
@file Graphic.c
@brief Knjižnica funkcija za crtanje geometrijskih primitiva na LCD-u.
*/
   
/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "Graphic.h"

/* _____LOCAL DEFINE_________________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____PUBLIC FUNCTIONS_____________________________________________________ */

/**
@brief Draw a line on a graphic LCD using Bresenham's
@details
PreCondition : SetColor()\n
@param x1,y1 - starting coordinates
@param x2,y2 - ending coordinates
@return none

*/
void Line(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	signed int  x, y, addx, addy, dx, dy;
	signed long P;
	int i;

	dx = abs((signed int)(x2 - x1));
	dy = abs((signed int)(y2 - y1));
	x = x1;
	y = y1;
	
	addx = addy = 1;
	if(x1 > x2)
	addx = -1;
	if(y1 > y2)
	addy = -1;
	
	if (dx >= dy)
	{
		P = 2L*dy - dx;
		for (i=0; i<=dx; ++i)
		{
			PutPixel(x, y);
			if(P < 0)
			{
				P += 2*dy;
				x += addx;
			}
			else
			{
				P += 2*dy - 2*dx;
				x += addx;
				y += addy;
			}
		}
	}
	else
	{
		P = 2L*dx - dy;
		for(i=0; i<=dy; ++i)
		{
			PutPixel(x, y);
			if(P < 0)
			{
				P += 2*dx;
				y += addy;
			}
			else
			{
				P += 2*dx - 2*dy;
				x += addx;
				y += addy;
			}
		}
	}
}

/**
@brief Draw a rectangle on a graphic LCD
@details
PreCondition : SetColor()\n
@param x1,y1 - starting coordinates
@param x2,y2 - ending coordinates
@return none
*/
void Rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	FillRectangle(x1, y1, x2, y1);    // top
	FillRectangle(x1, y2, x2, y2);    // bottom
	FillRectangle(x1, y1, x1, y2);    // left
	FillRectangle(x2, y1, x2, y2);    // right
}

/**
@brief Draws a beveled figure on the screen.
@details
PreCondition: None
@param x1, y1 - coordinate position of the upper left center
@param x2, y2 - coordinate position of the lower right center
@param rad - defines the radius of the circle
@param fill - fill yes or no
@return none
*/
void RoundRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int radius, unsigned char fill)
{
	signed int a, b, P;
	signed int bx, ay;
	
	if (fill)
	{
		a = 0;       // increment by 1
		b = radius;  // decrement by 1 using P
		P = 1 - radius;
		bx = b;
		ay = a;
		
		FillRectangle(x1, y1-radius+1, x2, y2+radius-1);
		do
		{
			if (bx != b)
			{
				FillRectangle(x1-bx, y1-ay, x1-bx, y2+ay); // left in
				FillRectangle(x2+bx, y1-ay, x2+bx, y2+ay); // right in
				bx = b;
			}
			ay = a;
			if (a != b)
			{
				FillRectangle(x1-a, y1-b, x1, y2+b); // left out
				FillRectangle(x2  , y1-b, x2+a, y2+b); // rigth out
			}
			if(P < 0)
			P+= 3 + 2*a++;
			else
			P+= 5 + 2*(a++ - b--);
		} while(a <= b);
		
		if (fill && (bx != b))
		{
			FillRectangle(x1-bx, y1-ay, x1-bx, y2+ay); // left in
			FillRectangle(x2+bx, y1-ay, x2+bx, y2+ay); // right in
		}
	}
	
	// draw broder
	{
		a = 0;       // increment by 1
		b = radius;  // decrement by 1 using P
		P = 1 - radius;
		bx = b;
		ay = a;
		
		do
		{
			if (a && (a != b))
			{
				PutPixel(x1-b, y1-a);
				PutPixel(x1-b, y2+a);
				PutPixel(x2+b, y1-a);
				PutPixel(x2+b, y2+a);
			}
			PutPixel(x1-a, y1-b);
			PutPixel(x2+a, y1-b);
			PutPixel(x1-a, y2+b);
			PutPixel(x2+a, y2+b);
			
			if(P < 0)
			P+= 3 + 2*a++;
			else
			P+= 5 + 2*(a++ - b--);
		} while(a <= b);
		
		FillRectangle(x1       , y1-radius, x2       , y1-radius);  // top
		FillRectangle(x1       , y2+radius, x2       , y2+radius);  // bottom
		FillRectangle(x1-radius, y1       , x1-radius, y2       );  // left
		FillRectangle(x2+radius, y1       , x2+radius, y2       );  // right
	}
}
/**
@brief Draw a circle on the screen.
@details
PreCondition: None
@param x,y - the center of the circle
@param rad - defines the redius of the circle,
@param fill - fill yes or no
@return none
*/
void Circle(unsigned int x, unsigned int y, unsigned int radius, unsigned char fill)
{
	signed int a, b, P;
	a = 0;
	b = radius;
	P = 1 - radius;
	
	do
	{
		if(fill)
		{
			FillRectangle(x-a, y+b, x+a, y+b);
			FillRectangle(x-a, y-b, x+a, y-b);
			FillRectangle(x-b, y+a, x+b, y+a);
			FillRectangle(x-b, y-a, x+b, y-a);
		}
		else
		{
			PutPixel(a+x, b+y);
			PutPixel(b+x, a+y);
			PutPixel(x-a, b+y);
			PutPixel(x-b, a+y);
			PutPixel(b+x, y-a);
			PutPixel(a+x, y-b);
			PutPixel(x-a, y-b);
			PutPixel(x-b, y-a);
		}
		
		if(P < 0)
		P+= 3 + 2*a++;
		else
		P+= 5 + 2*(a++ - b--);
	} while(a <= b);
}
