// qwiic_oledtransp.h
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

// Implementation for the Transparent OLED device

#pragma once


#include "qwiic_grssd1306.h"

//////////////////////////////////////////////////////////////////
// Set the defaults for the SparkFun Qwiic MicroOLED

#define kOLEDTransWidth     128
#define kOLEDTransHeight    64

// The viewport x is off by 2 on this device - starts at value 2
#define kOLEDTransXOffset    0   
#define kOLEDTransYOffset    0   

// Parameters for this device
#define kOLEDTransPinConfig  0x12
#define kOLEDTransPreCharge  0x25
#define kOLEDTransVCOM       0x40

#define kOLEDTransDefaultAddress 0x3C

class QwOLEDTransparent : public QwGrSSD1306 {


public:

    // Constructor - setup the viewport and default address for this device. 
    QwOLEDTransparent() : 
        QwGrSSD1306(kOLEDTransXOffset, kOLEDTransYOffset, kOLEDTransWidth, kOLEDTransHeight)
        {
            default_address =kOLEDTransDefaultAddress;
        };

    // set up the specific device settings
    bool init(void){

        set_buffer(_graphicsBuffer); // The buffer to use 

        set_comm_pins(kOLEDTransPinConfig);
        set_pre_charge(kOLEDTransPreCharge);
        set_vcom_deselect(kOLEDTransVCOM);

        // Call the super class to do all the work
        return this->QwGrSSD1306::init();
    };

private:
    // Graphics buffer for this device. 
    uint8_t _graphicsBuffer[kOLEDTransWidth * kOLEDTransHeight/8];

};