/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __USB_DEVICE_HID_MOUSE_H__
#define __USB_DEVICE_HID_MOUSE_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SCREEN_WIDTH	1070
#define SCREEN_HEIGHT	605
#define CORNER_DISTANCE	200
#define HEIGHT			500
#define LEFT_DISTANCE	300
#define RIGHT_DISTANCE	300
#define BTN_BUFFER 	0
#define X_BUFFER	1
#define Y_BUFFER	2
#define DONT_MOVE 	0
#define MOVE_RIGHT	2
#define MOVE_DOWN	2
#define MOVE_LEFT	-2
#define MOVE_UP		-2
#define RESET		0
#define CLICK_DOWN	1
#define CLICK_UP	0
#define SQUARE_LENGHT	100

typedef struct _usb_device_hid_mouse_struct
{
    uint8_t *buffer;
    uint8_t idleRate;
} usb_device_hid_mouse_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

extern usb_status_t USB_DeviceHidMouseInit(usb_device_composite_struct_t *deviceComposite);
extern usb_status_t USB_DeviceHidMouseCallback(class_handle_t handle, uint32_t event, void *param);
extern usb_status_t USB_DeviceHidMouseSetConfigure(class_handle_t handle, uint8_t configure);
extern usb_status_t USB_DeviceHidMouseSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting);

#endif /* __USB_DEVICE_HID_MOUSE_H__ */
