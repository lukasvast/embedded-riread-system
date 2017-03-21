 /**
 @file SSD1289x16.c
 @brief Knjižnica implementacija funkcija za 3.2" TFT LCD.
 @author Jaruwit Supa
 */
  
/* _____STANDARD INCLUDES____________________________________________________ */   
#include <avr/io.h>       
#include <avr/pgmspace.h>       
   
/* _____PROJECT INCLUDES_____________________________________________________ */   
#include "SSD1289x16.h"

/* _____LOCAL VARIABLES______________________________________________________ */   
uint16_t _color = WHITE;   
   
/* _____PRIVATE FUNCTIONS____________________________________________________ */   
   
/**   
@brief Waits (pauses) for ms milliseconds (assumes internal clock at 8MHz)  
@param ms - wait milliseconds  
@return none  
*/    
void LCD_WaitMs(unsigned int ms)   
{   
    int i;   
   
    while (ms-- > 0)   
    {   
        /* 8192 (8k) clock cycles for 1ms; each time through loop  
           is 5 cycles (for loop control + nop) */   
        for (i = 0; i < 1638; ++i)   
            asm volatile ("nop");   
    }   
}   
                                 
/**    
@brief This function performs low level command write to LCD   
@param cmd - command written to the LCD module   
@return none  
@detail Hardware specific 
*/    
void LCD_WrCmd(unsigned char cmd)    
{    
    LCD_CLR_CS();   // clear CS pin to ENABLE LCD   
   
    LCD_CLR_RS();   
    LCD_SET_DBL = cmd;   
    LCD_SET_DBH = (cmd >> 8);   
    LCD_CLR_WR();   
    LCD_SET_WR();   
   
    LCD_SET_CS();   // set CS pin to DISABLE LCD   
}    
   
/**   
@brief This function performs low level display data write to LCD   
@param val - is the data written to the LCD module   
@return none   
@detail Hardware specific.   
*/    
void LCD_WrDat(unsigned int val)    
{    
    LCD_CLR_CS();               // clear CS pin to ENABLE LCD    
   
    LCD_SET_RS();   
    LCD_SET_DBL = val;   
    LCD_SET_DBH = (val>>8);   
    LCD_CLR_WR();   
    LCD_SET_WR();   
   
    LCD_SET_CS();   // set CS pin to DISABLE LCD    
}    
/**    
@brief Function sets register
@param index - register number  
@param val   - value to be set  
@return none  
@detail Hardware specific
*/   
void LCD_SetReg(unsigned char index, unsigned int val)   
{   
    LCD_WrCmd(index);   
    LCD_WrDat(val);   
}   
   
/**   
@brief This function performs low level read data register from LCD   
@param index - register number  
@return register data  
*/   
unsigned int LCD_GetReg(unsigned char index)   
{   
    unsigned int ret;   
   
    LCD_CLR_CS();   // clear  CS pin to ENABLE LCD    
   
    LCD_CLR_RS();   
    LCD_SET_DBL = index;   
    LCD_SET_DBH = (index >> 8);   
    LCD_CLR_WR();   
    LCD_SET_WR();   
   
    LCD_INPUT();                // input mode   
    LCD_CLR_RD();   
    asm volatile ("nop");   
    ret = LCD_GET_DBH();   
    ret <<= 8;   
    ret |= LCD_GET_DBL();   
    LCD_SET_RD();   
    LCD_SET_CS();   // set CS pin to DISABLE LCD    
   
    LCD_OUTPUT();               // output data   
    return ret;    
}   
   
/** 
@brief This function inits LCD IO   
*/   
void LCD_InitPort(void)   
{   
    LCD_SET_DBH = 0xFF;   
    LCD_SET_DBL = 0xFF;   
    LCD_OUTPUT();   
   
    // set default   
    LCD_SET_CS();   
    LCD_SET_RS();   
    LCD_SET_WR();   
    LCD_SET_RD();   
   
    LCD_CLR_RST();   
   
    // init direction   
    LCD_DIR_CS(1);   
    LCD_DIR_RS(1);   
    LCD_DIR_WR(1);   
    LCD_DIR_RD(1);   
    LCD_DIR_RST(1);   
   
    // reset module   
    LCD_WaitMs(20);   
    LCD_SET_RST();   
    LCD_WaitMs(20);   
}   
   
/* _____PUBLIC FUNCTIONS_____________________________________________________ */   
   
/**  
@brief This function resets LCD module  
*/   
void LCD_Reset(void)   
{   
    unsigned int driver_code;   
    LCD_InitPort();   
   
    driver_code = LCD_GetReg(0x00);   
    LCD_LO_DDR = 0xFF;   
    LCD_HI_DDR = 0xFF;   
   
    LCD_LO_PORT = driver_code;   
    LCD_HI_PORT = driver_code>>8;   

    LCD_WrCmd(0x0000);LCD_WrDat(0x0001); //Start Oscillation OSCEN=1   
    LCD_WaitMs(15);   
    LCD_WrCmd(0x0003);LCD_WrDat(0xAAAC); //Power Control (1)   
    LCD_WrCmd(0x000C);LCD_WrDat(0x0002); //Power Control (2)   
    LCD_WrCmd(0x000D);LCD_WrDat(0x000A); //Power Control (3)   
    LCD_WrCmd(0x000E);LCD_WrDat(0x2C00); //Power Control (4)   
    LCD_WrCmd(0x001E);LCD_WrDat(0x00B8); //Power Control (5)   
    LCD_WaitMs(15);   
    LCD_WrCmd(0x0001);LCD_WrDat(0x2B3F); //Driver Output Control RL=0, REV=1, BGR=1, TB=1    
//      LCD_WrCmd(0x0001);LCD_WrDat(0x6B3F); //Driver Output Control RL=1, REV=1, BGR=1, TB=1    
    LCD_WrCmd(0x0002);LCD_WrDat(0x0600);   
    // Restore VSYNC mode from low power state    
    LCD_WrCmd(0x0010);LCD_WrDat(0x0000); //Sleep mode cancel   
    LCD_WrCmd(0x0011);LCD_WrDat(0x6030); //Entry Mode   
                                        // DFM   0x4000 = 262L color   
                                        // DFM   0x6000 = 65K color   
                                        // AM    0x0000 = horizontal display   
                                        // AM    0x0008 = Vertical display   
                                        // ID[0] 0x0000 = horizontal decrement   
                                        // ID[0] 0x0010 = horizontal increment   
                                        // ID[1] 0x0000 = Vertical decrement   
                                        // ID[1] 0x0020 = Vertical decrement   
   
    LCD_WaitMs(20);   
    LCD_WrCmd(0x0005);LCD_WrDat(0x0000); // Compare register   
    LCD_WrCmd(0x0006);LCD_WrDat(0x0000); // Compare register   
    // Horizontal and Vertical porch are for DOTCLK mode operation   
    LCD_WrCmd(0x0016);LCD_WrDat(0xEF1C); // Horizontal Porch    
    LCD_WrCmd(0x0017);LCD_WrDat(0x0003); // Vertical Porch   
    // Display Control   
    LCD_WrCmd(0x0007);LCD_WrDat(0x0233); // Display Control   
                                         // D1 0x0000 = display off   
                                         // D1 0x0002 = display on   
                                         // D0 0x0000 = internal display halt   
                                         // D0 0x0001 = internal display operate   
   
   
    LCD_WrCmd(0x000B);LCD_WrDat(0x5312); // Frame cycle control   
    LCD_WrCmd(0x000F);LCD_WrDat(0x0000); // Gate Scan Position    
    LCD_WaitMs(20);   
    // Vertical Scroll Control   
    LCD_WrCmd(0x0041);LCD_WrDat(0x0000); // Vertical Scroll Control   
    LCD_WrCmd(0x0042);LCD_WrDat(0x0000); // Vertical Scroll Control   
   
    // 1st Screen driving position   
    LCD_WrCmd(0x0048);LCD_WrDat(0x0000); // Start position. 0   
    LCD_WrCmd(0x0049);LCD_WrDat(0x013F); // End position.   319   
   
    // Source RAM address   
    LCD_WrCmd(0x0044);LCD_WrDat(0xEF00); //Horizontal RAM address position start/end setup    
                                         //dec 239    
                                         //dec 0, i.e. horizontal ranges from 0 -> 239    
                                         //POR value is 0xEF00 anyway. This address must be set before RAM write    
   
    LCD_WrCmd(0x0045);LCD_WrDat(0x0000); //Vertical RAM address start position setting    
                                         //0x0000 = dec 0   
    LCD_WrCmd(0x0046);LCD_WrDat(0x013F); //Vertical RAM address end position setting (0x013F = dec 319)   
   
        // 2nd Screen driving position   
//      LCD_WrCmd(0x004A);LCD_WrDat(0x0000); // Start position. 0   
//      LCD_WrCmd(0x004B);LCD_WrDat(0x0000); // End position.   0   
    LCD_WaitMs(20);    
    //gamma control    
    LCD_WrCmd(0x0030);LCD_WrDat(0x0707);   
    LCD_WrCmd(0x0031);LCD_WrDat(0x0704);   
    LCD_WrCmd(0x0032);LCD_WrDat(0x0204);   
    LCD_WrCmd(0x0033);LCD_WrDat(0x0201);   
    LCD_WrCmd(0x0034);LCD_WrDat(0x0203);   
    LCD_WrCmd(0x0035);LCD_WrDat(0x0204);   
    LCD_WrCmd(0x0036);LCD_WrDat(0x0204);   
    LCD_WrCmd(0x0037);LCD_WrDat(0x0502);   
    LCD_WrCmd(0x003A);LCD_WrDat(0x0302);   
    LCD_WrCmd(0x003B);LCD_WrDat(0x0500);   
   
    LCD_WaitMs(20);   
}       
   
/**  
@brief Set start address of LCD ram  
@param x,y - pixel coordinates  
@return none  
@detail set start address change  
*/   
void LCD_SetCursor(unsigned int x, unsigned int y)   
{   
#ifdef Horizontal   
    LCD_SetReg(0x4E,x); // initial settings for the GDDRAM X address in the address counter (AC).   
    LCD_SetReg(0x4F,y); // initial settings for the GDDRAM Y address in the address counter (AC).   
#else   
    LCD_SetReg(0x4E,y); // initial settings for the GDDRAM X address in the address counter (AC).   
    LCD_SetReg(0x4F,x); // initial settings for the GDDRAM Y address in the address counter (AC).   
#endif   
    LCD_WrCmd(0x22);    
}   
   
/** 
@brief Set paint area  
@param x1,y1,x2,y2 - pixel coordinates  
@return none  
@detail Side Effects: paint area change  
*/   
void LCD_SetArea(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)   
{   
#ifdef Horizontal   
    LCD_WrCmd(0x44); LCD_WrDat((x2 << 8) | x1);    // Source RAM address window    
    LCD_WrCmd(0x45); LCD_WrDat(y1);    // Gate RAM address window    
    LCD_WrCmd(0x46); LCD_WrDat(y2);    // Gate RAM address window    
#else   
    LCD_WrCmd(0x44); LCD_WrDat((y2 << 8) | y1);    // Source RAM address window    
    LCD_WrCmd(0x45); LCD_WrDat(x1);    // Gate RAM address window    
    LCD_WrCmd(0x46); LCD_WrDat(x2);    // Gate RAM address window    
#endif   
    LCD_SetCursor(x1, y1);;   
}   
   
/**    
@brief Puts pixel  
@param x,y - pixel coordinates  
@return none  
*/   
void LCD_PutPixel(unsigned int x, unsigned int y)   
{   
    LCD_SetCursor(x,y);   
    LCD_WrDat(_color);   
}   
   
/** 
@brief Draws rectangle filled with current color  
@param left,top - top left corner coordinates,  
@param right,bottom - bottom right corner coordinates  
@return none  
@details
*  CGRAM map  \n
*  00000...000EF line0  \n
*  00100...001EF line1  \n
*  00200...002EF line2   \n
*   ....  \n
*  13F00...13FEF line320  \n
*  yyyxx...yyyxx  \n
*  address x = x  \n
*  address y = y << 8  \n
*  new line y = y + 0x100  \n
*/   
void LCD_Bar(unsigned int left, unsigned int top, unsigned int right, unsigned int bottom, unsigned int color)   
{   
    register unsigned int x,y;   
   
    LCD_SetArea(left, top, right, bottom);   
    for(y=top; y<=bottom; y++)   
    {   
        for(x=left; x<=right; x++)   
        {   
            LCD_WrDat(color);   
        }   
    }   
    LCD_SetArea(0, 0, GetMaxX(), GetMaxY());   
}   
   
/**   
@brief Fill display with color  
@param color - paint color  
@return 
*/   
void LCD_Clear(unsigned int color)   
{   
//  LCD_WrCmd(0x0007);LCD_WrDat(0x0221); // Display Control: display off   
    LCD_Bar(0, 0, GetMaxX(), GetMaxY(), color);   
//  LCD_WrCmd(0x0007);LCD_WrDat(0x0233); // Display Control: display on   
}   
   
/**
@brief Display image array to lcd  
@param    x,y - pixel coordinates  
@param     w  - width   
@param      h - height  
@param      t - compress type(0 = none(RGB565), 1 = compress(RGB5<compress bit>55)  
@param pImage - FLASH array of image  
*/   
void LCD_DrawSymbol(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char t, const unsigned char *pImage)   
{   
    unsigned char colorMsb;   
    unsigned char colorLsb;   
    unsigned char colorDup = 0;   
    unsigned int color=0;   
    unsigned int x1,y1;   
   
    // set draw area   
    LCD_SetArea(x,y,x+w-1,y+h-1);   
    for (y1 = 0; y1<h ; y1++)   
    {   
        for (x1 = 0; x1<w ; x1++)   
        {   
            if (!colorDup)   
            {   
                colorMsb = pgm_read_byte(pImage++);   
                colorLsb = pgm_read_byte(pImage++);   
                if (t == 0x01)   
                {   
                    colorDup = colorLsb & 0x20;         // get duplicate flag   
                    colorLsb = colorLsb | 0x20;         // remove duplicate flag and add more white   
                }   
                if (colorDup)   
                    colorDup = pgm_read_byte(pImage++); // get duplicate no   
                color = ((uint16_t)colorMsb << 8) | colorLsb;     // merge color   
                SetColor(color);                        // set a pixel   
            }   
            else   
            {   
                colorDup--;   
            }   
            LCD_WrDat(color);   
        }   
    }   
    LCD_SetArea(0, 0, GetMaxX(), GetMaxY());   
}   
   
/**
@brief Display image array to lcd  
@param    x,y - pixel coordinates  
@param pImage - FLASH array of image  
@details
* Note        : ** modify image format here **  \n
*   image format  \n
*   <type 1 byte><width 2 byte><height 2 btye><image array n byte>  \n
*/   
void LCD_DrawImage(unsigned int x, unsigned int y, const unsigned char *pImage)   
{   
    unsigned char t;   
    unsigned int w, h;   
   
    // get image information   
    t = pgm_read_byte(pImage++);            // image type   
    w = pgm_read_byte(pImage++);            // width   
    w = (w << 8) | pgm_read_byte(pImage++);   
    h = pgm_read_byte(pImage++);            // Height   
    h = (h << 8) | pgm_read_byte(pImage++);   
   
    LCD_DrawSymbol(x, y, w, h, t, pImage);   
}
