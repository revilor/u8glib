/*

  u8g_dev_ssd1306_64x48.c

  Universal 8bit Graphics Library
  
  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  

*/


#include "u8g.h"

#define WIDTH 96
#define HEIGHT 16
#define PAGE_HEIGHT 8



/* init sequence buydisplay.com 0.69" 96x16 OLED */
/* http://www.buydisplay.com/download/manual/ER-OLED0.69-1_Series_Datasheet.pdf */
static const uint8_t u8g_dev_ssd1306_96x16_init_seq[] PROGMEM = {
  U8G_ESC_CS(0),        /* disable chip */
  U8G_ESC_ADR(0),       /* instruction mode */
  U8G_ESC_RST(1),   /* do reset low pulse with (1*16)+2 milliseconds */
  U8G_ESC_CS(1),        /* enable chip */

  0x0ae,        /* display off, sleep mode */
  0x0d5, 0x080,     /* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  0x0a8, 0x00f,     /* Feb 23, 2013: 128x32 OLED: 0x01f,  128x32 OLED 0x03f */

  0x0d3, 0x000,     /*  */

  0x040,        /* start line */
  
  0x08d, 0x014,     /* [2] charge pump setting (p62): 0x014 enable, 0x010 disable */ 

  0x020, 0x002,     /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5), Feb 23, 2013: 128x32 OLED: 0x002,  128x32 OLED 0x012 */
  0x0a1,        /* segment remap a0/a1*/
  0x0c8,        /* c0: scan dir normal, c8: reverse */
  0x0da, 0x002,     /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
  0x081, 0x0af,     /* [2] set contrast control */
  0x0d9, 0x0f1,     /* [2] pre-charge period 0x022/f1*/
  0x0db, 0x020,     /* vcomh deselect level */
  
  0x02e,        /* 2012-05-27: Deactivate scroll */ 
  0x0a4,        /* output ram to display */
  0x0a6,        /* none inverted normal display mode */
  0x0af,        /* display on */

  U8G_ESC_CS(0),        /* disable chip */
  U8G_ESC_END           /* end of sequence */
};




static const uint8_t u8g_dev_ssd1306_96x16_data_start[] PROGMEM = {
  U8G_ESC_ADR(0),       /* instruction mode */
  U8G_ESC_CS(1),        /* enable chip */
  0x010,				/* set upper 4 bit of the col adr. to 0, 0.66" OLED starts with offset 32 */
  0x000,				/* set lower 4 bit of the col adr. to 4  */
  U8G_ESC_END           /* end of sequence */
};

static const uint8_t u8g_dev_ssd13xx_sleep_on[] PROGMEM = {
  U8G_ESC_ADR(0),           /* instruction mode */
  U8G_ESC_CS(1),             /* enable chip */
  0x0ae,		/* display off */      
  U8G_ESC_CS(0),             /* disable chip, bugfix 12 nov 2014 */
  U8G_ESC_END                /* end of sequence */
};

static const uint8_t u8g_dev_ssd13xx_sleep_off[] PROGMEM = {
  U8G_ESC_ADR(0),           /* instruction mode */
  U8G_ESC_CS(1),             /* enable chip */
  0x0af,		/* display on */      
  U8G_ESC_DLY(50),       /* delay 50 ms */
  U8G_ESC_CS(0),             /* disable chip, bugfix 12 nov 2014 */
  U8G_ESC_END                /* end of sequence */
};

uint8_t u8g_dev_ssd1306_96x16_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{
  switch(msg)
  {
    case U8G_DEV_MSG_INIT:
      u8g_InitCom(u8g, dev, U8G_SPI_CLK_CYCLE_300NS);
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_ssd1306_96x16_init_seq);
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_NEXT:
      {
        u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
        u8g_WriteEscSeqP(u8g, dev, u8g_dev_ssd1306_96x16_data_start);    
        u8g_WriteByte(u8g, dev, 0x0b0 | pb->p.page);	/* select current page (SSD1306) */
        u8g_SetAddress(u8g, dev, 1);					/* data mode */
        if ( u8g_pb_WriteBuffer(pb, u8g, dev) == 0 )
          return 0;
        u8g_SetChipSelect(u8g, dev, 0);
      }
      break;
    case U8G_DEV_MSG_SLEEP_ON:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_ssd13xx_sleep_on);    
      return 1;
    case U8G_DEV_MSG_SLEEP_OFF:
      u8g_WriteEscSeqP(u8g, dev, u8g_dev_ssd13xx_sleep_off);    
      return 1;
}
  
  return u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
}


U8G_PB_DEV(u8g_dev_ssd1306_96x16_i2c, WIDTH, HEIGHT, PAGE_HEIGHT, u8g_dev_ssd1306_96x16_fn, U8G_COM_SSD_I2C);
