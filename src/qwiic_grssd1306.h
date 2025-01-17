// qwiic_gr1306.h
// 
// This is a library written for SparkFun Qwiic OLED boards that use the SSD1306.
//
// SparkFun sells these at its website: www.sparkfun.com
//
// Do you like this library? Help support SparkFun. Buy a board!
//
//   Micro OLED             https://www.sparkfun.com/products/14532
//   Transparent OLED       https://www.sparkfun.com/products/15173
//   "Narrow" OLED          https://www.sparkfun.com/products/17153
// 
// 
// Written by Kirk Benell @ SparkFun Electronics, March 2022
//
// This library configures and draws graphics to OLED boards that use the 
// SSD1306 display hardware. The library only supports I2C.
// 
// Repository:
//     https://github.com/sparkfun/SparkFun_Qwiic_OLED_Arduino_Library
//
// Documentation:
//     https://sparkfun.github.io/SparkFun_Qwiic_OLED_Arduino_Library/
//
//
// SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
//
// SPDX-License-Identifier: MIT
//
//    The MIT License (MIT)
//
//    Copyright (c) 2022 SparkFun Electronics
//    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
//    associated documentation files (the "Software"), to deal in the Software without restriction,
//    including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//    and/or sell copies of the Software, and to permit persons to whom the Software is furnished to
//    do so, subject to the following conditions:
//    The above copyright notice and this permission notice shall be included in all copies or substantial
//    portions of the Software.
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//    NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/*
 * Header file for the SSD1306 bitmap graphics driver device.
 */

#pragma once

#include "qwiic_grbuffer.h"
#include "qwiic_i2c.h"
#include "res/qwiic_resdef.h"


/////////////////////////////////////////////////////////////////////////////
// Device Config
/////////////////////////////////////////////////////////////////////////////
//
// Defaults
// Each device can have a different Hardware pin configuration, which must
// be set in the device. These are the pins that connect the display to
// the SSD1306.
//
#define kDefaultPinConfig       0x12
#define kDefaultPreCharge       0xF1
#define kDefaultVCOMDeselect    0x40
#define kDefaultContrast        0x8F

/////////////////////////////////////////////////////////////////////////////
// The graphics Raster Operator functions (ROPS)
/////////////////////////////////////////////////////////////////////////////
//      - Copy      - copy the pixel value in to the buffer (default)
//      - Not Copy  - copy the not of the pixel value to buffer
//      - Not       - Set the buffer value to not it's current value
//      - XOR       - XOR of color and current pixel value
//      - Black     - Set value to always be black
//      - White     - set value to always be white

typedef enum gr_op_funcs_{
    grROPCopy       = 0,
    grROPNotCopy    = 1,
    grROPNot        = 2,
    grROPXOR        = 3,
    grROPBlack      = 4,
    grROPWhite      = 5
}grRasterOp_t;

/////////////////////////////////////////////////////////////////////////////
// Flags for scrolling
/////////////////////////////////////////////////////////////////////////////

#define SCROLL_VERTICAL     0x01
#define SCROLL_RIGHT        0x02
#define SCROLL_LEFT         0x04
#define SCROLL_VERT_RIGHT   SCROLL_VERTICAL|SCROLL_RIGHT
#define SCROLL_VERT_LEFT    SCROLL_VERTICAL|SCROLL_LEFT

#define SCROLL_INTERVAL_5_FRAMES    0x00
#define SCROLL_INTERVAL_64_FRAMES   0x01
#define SCROLL_INTERVAL_128_FRAMES  0x02
#define SCROLL_INTERVAL_256_FRAMES  0x03
#define SCROLL_INTERVAL_3_FRAMES    0x04
#define SCROLL_INTERVAL_4_FRAMES    0x05
#define SCROLL_INTERVAL_25_FRAMES   0x06
#define SCROLL_INTERVAL_2_FRAMES    0x07

/////////////////////////////////////////////////////////////////////////////
// Buffer Management
/////////////////////////////////////////////////////////////////////////////
//
// The memory/back buffer of the SSD1306 is based on the concept of pages -
// each page is a stream of bytes, and defined as follows:
//
//      - X pixel position is an offset in a byte array
//      - Y pixel position is a bit in a byte, so a page can have 8 Y locations
//
// A pixel value of 1, turn on the corresponding pixel, 0 turns it off. 
//
// The device has different data transfer modes - see the data sheet - mostly
// outline how received a recieved byte is placed in the device framebuffer and the 
// next update locaton set. 
//
// This implementation uses the Page mode for buffer transfer. This is defined by:
//     - A start position is set - a page number and column in that page.
//     - As data is transferred, it is written to the screenbuffer, based on this start 
//       position
//     - If the end of the page is reached, the next entry location is the start of that page
//
// >> Implementation <<
//
// This implementation uses the concept of "dirty rects" at the page level to minimize data
// transfers to the device. The min and max x locations set for each page is recorded as 
// graphics are draw to the graphics buffer. When the transfering the display buffer to
// the devices screen buffer, the following takes place:
//
//      For each page:
//          - if page is dirty
//              - Set the screen buffer current location to this page, xmin dirty value
//              - Write buffer bytes to the device - starting at xmin for the page, ending at xmax
//              - Mark the buffer as "clean"
//
//
// Define variables to manage page state

#define  kMaxPageNumber             8

typedef struct {
    int16_t     xmin;
    int16_t     xmax;
} pageState_t; 


/////////////////////////////////////////////////////////////////////////////
// QwGrSSD1306
// A buffer graphics device to support the SSD1306 graphics hardware

class QwGrSSD1306 :  public QwGrBufferDevice {

public:

    QwGrSSD1306();    // default constructor - always called 
    using QwGrBufferDevice::QwGrBufferDevice; // inherit constructors 

    // Public draw methods
    void display(void);         // send graphics buffer to the devices screen buffer
    void erase(void);
    
    // Device setup
    virtual bool init(void );
    bool is_initialized(void){ return _isInit;};
    
    // method to set the communication bus this object should use
    void set_comm_bus(QwI2C &theBus, uint8_t id_bus);

    // Set the current color/pixel write operation
    void set_color(uint8_t color);

    // Settings/operational methods
    void set_contrast(uint8_t);

    // default address of the device - expect the sub to fill in.
    uint8_t default_address;

    void set_raster_op(grRasterOp_t rop){
        _rop = rop;
    }
    grRasterOp_t get_raster_op(void){
        return _rop;
    }
    // screen control
    void invert(bool);
    void flip_vert(bool);
    void flip_horz(bool);   

    // screen scrolling
    void scroll_stop(void);
    void scroll(uint16_t scroll_type, uint8_t start, uint8_t stop, uint8_t interval=SCROLL_INTERVAL_2_FRAMES);

protected:

    // Subclasses of this class define the specifics of the device, including size.
    // Subclass needs to define the graphics buffer array - stack based - and pass in 
    void set_buffer(uint8_t *pBuffer);      

    ///////////////////////////////////////////////////////////////////////////
    // Internal, fast draw routines - this are used in the overall 
    // draw interface (_QwIDraw) for this object/device/system. 
    //
    // >> Pixels <<
    void  draw_pixel(uint8_t x, uint8_t y, uint8_t clr);

    // >> Fast Lines <<
    void draw_line_horz(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t clr);
    void draw_line_vert(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t clr);   

    // fast rect fill 
    void draw_rect_filled(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, uint8_t clr);
    
    // >> Fast Bitmap <<
    void draw_bitmap(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
                     uint8_t *pBitmap, uint8_t bmp_width, uint8_t bmp_height );
    
    ///////////////////////////////////////////////////////////////////////////
    // configuration methods for sub-classes. Settings unique to a device 
    void set_comm_pins(uint8_t);
    void set_pre_charge(uint8_t);
    void set_vcom_deselect(uint8_t);

private:

    // Internal buffer management methods
    bool set_screenbuffer_address(uint8_t page, uint8_t column);
    void init_buffers(void); // clear graphics and screen buffer
    void clear_screen_buffer(void);
    void resend_graphics(void);
    
    // device communication methods
    void send_dev_command(uint8_t command);
    void send_dev_command(uint8_t command, uint8_t value);  
    void send_dev_command(uint8_t *commands, uint8_t n);        
    void send_dev_data(uint8_t *pData, uint8_t nData);

    /////////////////////////////////////////////////////////////////////////////
    // instance vars

    // Buffer variables
    uint8_t           * _pBuffer;           // Pointer to the graphics buffer
    uint8_t             _nPages;            // number of pages for current device
    pageState_t         _pageState[kMaxPageNumber];   // page state descriptors 
    pageState_t         _pageErase[kMaxPageNumber];   // keep track of erase boundaries 
    bool                _pendingErase; 

    // display variables
    uint8_t             _color;     // current color (really 0 or 1)
    grRasterOp_t        _rop;       // current raster operation code

    // I2C  things
    QwI2C             * _i2cBus;       // pointer to our i2c bus object
    uint8_t             _i2c_address;  // address of the device

    // Stash values for settings that are unique to each device.
    uint8_t             _initHWComPins;
    uint8_t             _initPreCharge;
    uint8_t             _initVCOMDeselect;
    uint8_t             _initContrast;

    bool                _isInit; // general init flag
};
