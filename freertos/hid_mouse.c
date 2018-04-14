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

#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_hid.h"

#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"

#include "composite.h"

#include "hid_mouse.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceHidMouseAction(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DATA_ALIGNMENT static uint8_t s_MouseBuffer[USB_HID_MOUSE_REPORT_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_device_hid_mouse_struct_t s_UsbDeviceHidMouse;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Update mouse pointer location. Draw a rectangular rotation*/
static usb_status_t USB_DeviceHidMouseAction(void)
{
	enum{ /* Aqui definimos lode diferentes estados y acciones del teclado */
		IDLE,
		WINDOWS_1,
		PAINT_P,
		PAINT_A,
		PAINT_I,
		PAINT_N,
		PAINT_T,
		ENTER_1,
		DELAY_1,
		DELAY_2,
		WINDOWS_2,
		DELAY_3,
		NOTEPAD_N,
		NOTEPAD_O,
		NOTEPAD_T,
		NOTEPAD_E,
		NOTEPAD_P,
		NOTEPAD_A,
		NOTEPAD_D,
		DELAY_4,
		DELAY_5,
		DELAY_5_2,
		ENTER_2,
		ALT_1,
		SPACE_1,
		KEY_X_1,
		HOLA_H,
		HOLA_O,
		HOLA_L,
		HOLA_A,
		SPACE_2,
		PROFE_P,
		PROFE_R,
		PROFE_O,
		PROFE_F,
		PROFE_E,
		DOT,
		SEL_ALL,
		COPY,

		WINDOWS_3,
		DELAY_6,
		NOTEPAD2_N,
		NOTEPAD2_O,
		NOTEPAD2_T,
		NOTEPAD2_E,
		NOTEPAD2_P,
		NOTEPAD2_A,
		NOTEPAD2_D,
		DELAY_7,
		DELAY_8,
		ENTER_3,
		ALT_2,
		SPACE_3,
		KEY_X_2,
		PASTE
	};

	enum{  /* Aqui definimos lode diferentes estados y acciones del mouse */
		UNKNOWN,
		CORNER,
		CENTER_X,
		CENTER_Y,
		WAITING,
		CLICK_DOWN_SQUARE,
		SQUARE_DOWN,
		SQUARE_LEFT,
		SQUARE_UP,
		SQUARE_RIGHT,
		TOP_1,
		CLICK_DOWN_TOP_1,
		LEFT_1,
		CLICK_DOWN_1,
		CLICK_UP_1,
		TOP_2,
		CLICK_DOWN_TOP_2,
		RIGHT_1,
		CLICK_DOWN_2,
		CLICK_UP_2
	};

    static int16_t pixels = RESET;

	switch (getMouseState()) {
	case UNKNOWN:/* Es un estado idle para limpiar los buffers y no se manden otra accion*/
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = (uint8_t) (MOVE_LEFT);
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = (uint8_t) (MOVE_UP);
		pixels++;
		if (pixels > CORNER_DISTANCE) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			setMouseState(CORNER);
			pixels = RESET;
		}
		break;
	case CORNER: /* Es un estado que permite direccionar la felcha del mouse a la esquina*/
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_RIGHT;
		pixels++;
		if (pixels > (SCREEN_WIDTH/2)) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			setMouseState(CENTER_X);
			pixels = RESET;
		}
		break;
	case CENTER_X:/* Es un estado que permite direccionar la felcha del mouse al centro en el eje x*/
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_DOWN;
		pixels++;
		if (pixels > (SCREEN_HEIGHT/2)) {
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			setMouseState(CENTER_Y);
			pixels = RESET;
		}
		break;
	case CENTER_Y: /* Es un estado que permite direccionar la felcha del mouse al centro en el eje y*/
		setKBState(WINDOWS_1);
		setMouseState(WAITING);
		break;
////////////////////////////////////////////////////////////////////////////////////////////
		/* Estos estados ejecutan la tarea para crear el cuadrado en paint*/
	case CLICK_DOWN_SQUARE:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_DOWN;
		setMouseState(SQUARE_DOWN);
		break;
	case SQUARE_DOWN:
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_DOWN;
		pixels++;
		if (pixels > SQUARE_LENGHT) {
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			setMouseState(SQUARE_LEFT);
			pixels = RESET;
		}
		break;
	case SQUARE_LEFT:
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_LEFT;
		pixels++;
		if (pixels > SQUARE_LENGHT) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			setMouseState(SQUARE_UP);
			pixels = RESET;
		}
		break;
	case SQUARE_UP:
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_UP;
		pixels++;
		if (pixels > SQUARE_LENGHT) {
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			setMouseState(SQUARE_RIGHT);
			pixels = RESET;
		}
		break;
	case SQUARE_RIGHT:
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_RIGHT;
		pixels++;
		if (pixels > SQUARE_LENGHT) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_UP;
			setMouseState(WAITING);
			setKBState(WINDOWS_2);
			pixels = RESET;
		}
		break;
	case TOP_1:
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_UP;
		pixels++;
		if (pixels > HEIGHT) {
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			setMouseState(CLICK_DOWN_TOP_1);
			pixels = RESET;
		}
		break;
	case CLICK_DOWN_TOP_1:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_DOWN;
		setMouseState(LEFT_1);
		break;
	case LEFT_1:
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_LEFT*2;
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_DOWN;
		pixels++;
		if (pixels > LEFT_DISTANCE) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_UP;
			setMouseState(CLICK_DOWN_1);
			pixels = RESET;
		}
		break;
	case CLICK_DOWN_1:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_DOWN;
		setMouseState(CLICK_UP_1);
		break;
	case CLICK_UP_1:
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_UP;
		setMouseState(WAITING);
		setKBState(DELAY_5_2);
		break;

////////////////////////////////////////////////////////////////////////////////////////////

	case TOP_2:
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_UP;
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_RIGHT;
		pixels++;
		if (pixels > HEIGHT) {
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			setMouseState(CLICK_DOWN_TOP_2);
			pixels = RESET;
		}
		break;
	case CLICK_DOWN_TOP_2:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_DOWN;
		setMouseState(RIGHT_1);
		break;
	case RIGHT_1:
		s_UsbDeviceHidMouse.buffer[X_BUFFER] = MOVE_RIGHT*2;
		s_UsbDeviceHidMouse.buffer[Y_BUFFER] = MOVE_DOWN;
		pixels++;
		if (pixels > RIGHT_DISTANCE) {
			s_UsbDeviceHidMouse.buffer[X_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[Y_BUFFER] = DONT_MOVE;
			s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_UP;
			setMouseState(CLICK_DOWN_2);
			pixels = RESET;
		}
		break;
	case CLICK_DOWN_2:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_DOWN;
		setMouseState(CLICK_UP_2);
		break;
	case CLICK_UP_2:
		s_UsbDeviceHidMouse.buffer[BTN_BUFFER] = CLICK_UP;
		setMouseState(WAITING);
		setKBState(PASTE);
		break;



	case WAITING:

		break;
	default:
		break;
	}


    return USB_DeviceHidSend(s_UsbDeviceComposite->hidMouseHandle, USB_HID_MOUSE_ENDPOINT_IN,
                             s_UsbDeviceHidMouse.buffer, USB_HID_MOUSE_REPORT_LENGTH);
}

/* The device HID class callback */
usb_status_t USB_DeviceHidMouseCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            if (s_UsbDeviceComposite->attach)
            {
                return USB_DeviceHidMouseAction();
            }
            break;
        case kUSB_DeviceHidEventGetReport:
        case kUSB_DeviceHidEventSetReport:
        case kUSB_DeviceHidEventRequestReportBuffer:
            error = kStatus_USB_InvalidRequest;
            break;
        case kUSB_DeviceHidEventGetIdle:
        case kUSB_DeviceHidEventGetProtocol:
        case kUSB_DeviceHidEventSetIdle:
        case kUSB_DeviceHidEventSetProtocol:
            break;
        default:
            break;
    }

    return error;
}

/* The device callback */
usb_status_t USB_DeviceHidMouseSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        return USB_DeviceHidMouseAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

/* Set interface */
usb_status_t USB_DeviceHidMouseSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_KEYBOARD_INTERFACE_INDEX == interface)
    {
        return USB_DeviceHidMouseAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

/* Initialize the HID mouse */
usb_status_t USB_DeviceHidMouseInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbDeviceHidMouse.buffer = s_MouseBuffer;
    return kStatus_USB_Success;
}
