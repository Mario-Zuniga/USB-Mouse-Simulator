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

#include "hid_keyboard.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static usb_status_t USB_DeviceHidKeyboardAction(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_DATA_ALIGNMENT static uint8_t s_KeyboardBuffer[USB_HID_KEYBOARD_REPORT_LENGTH];
static usb_device_composite_struct_t *s_UsbDeviceComposite;
static usb_device_hid_keyboard_struct_t s_UsbDeviceHidKeyboard;

/*******************************************************************************
 * Code
 ******************************************************************************/

static usb_status_t USB_DeviceHidKeyboardAction(void)
{
	enum{ /* Aqui definimos los diferentes estados teclado */
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

	enum{ /* Aqui definimos los diferentes estados mouse */
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
		TOP_2
	};

	switch(getKBState()){
	case IDLE:  /* Es un estado idle para limpiar los buffers y no se manden otra accion*/
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		break;
	case WINDOWS_1: /* Es un estado que activa el comando de windows */
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ESCAPE;
		setKBState(DELAY_1);
		break;
	case DELAY_1:/* Es un estado que permite limpiar los buffers y dar una espera para ejecutar los comandos*/
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(PAINT_P);
		break;
	case PAINT_P: /* Es un estado para imprimir las letras del programa que se va a ejecutar */
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_P;
		setKBState(PAINT_A);
		break;
	case PAINT_A:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_A;
		setKBState(PAINT_I);
		break;
	case PAINT_I:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_I;
		setKBState(PAINT_N);
		break;
	case PAINT_N:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_N;
		setKBState(PAINT_T);
		break;
	case PAINT_T:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_T;
		setKBState(DELAY_2);
		break;
	case DELAY_2:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(ENTER_1);
		break;
	case ENTER_1: /* Es un estado para enviar la tecla enter*/
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ENTER;
		setKBState(IDLE);
		setMouseState(CLICK_DOWN_SQUARE);
		break;
/////////////////////////////////////////////////////////////////////////////////////////////////
		/* Despues de aqui empezamos con la segunda tarea que debemos implementar, de abrir el bloc de notas */
	case WINDOWS_2:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ESCAPE;
		setKBState(DELAY_3);
		break;
	case DELAY_3:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(NOTEPAD_N);
		break;
	case NOTEPAD_N:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_N;
		setKBState(NOTEPAD_O);
		break;
	case NOTEPAD_O:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_O;
		setKBState(NOTEPAD_T);
		break;
	case NOTEPAD_T:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_T;
		setKBState(NOTEPAD_E);
		break;
	case NOTEPAD_E:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_E;
		setKBState(NOTEPAD_P);
		break;
	case NOTEPAD_P:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_P;
		setKBState(NOTEPAD_A);
		break;
	case NOTEPAD_A:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_A;
		setKBState(NOTEPAD_D);
		break;
	case NOTEPAD_D:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_D;
		setKBState(DELAY_4);
		break;
	case DELAY_4:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(ENTER_2);
		break;
	case ENTER_2:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ENTER;
		setKBState(DELAY_5);
		break;
	case DELAY_5:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(ALT_1);
		break;
	case ALT_1:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_ALT;
		setKBState(SPACE_1);
		break;
	case SPACE_1:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_SPACEBAR;
		setKBState(KEY_X_1);
		break;
	case KEY_X_1:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	KEY_X;
		setKBState(IDLE);
		setMouseState(TOP_1);
		break;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/* Son los estados para escribir la frase en el bloc de notas y copie la frase */
	case DELAY_5_2:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(HOLA_H);
		break;
	case HOLA_H:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_SHIFT;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_H;
		setKBState(HOLA_O);
		break;
	case HOLA_O:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_O;
		setKBState(HOLA_L);
		break;
	case HOLA_L:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_L;
		setKBState(HOLA_A);
		break;
	case HOLA_A:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_A;
		setKBState(SPACE_2);
		break;
	case SPACE_2:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_SPACEBAR;
		setKBState(PROFE_P);
		break;
	case PROFE_P:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_P;
		setKBState(PROFE_R);
		break;
	case PROFE_R:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_R;
		setKBState(PROFE_O);
		break;
	case PROFE_O:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_O;
		setKBState(PROFE_F);
		break;
	case PROFE_F:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_F;
		setKBState(PROFE_E);
		break;
	case PROFE_E:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_E;
		setKBState(DOT);
		break;
	case DOT:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_DOT_GREATER;
		setKBState(SEL_ALL);
		break;
	case SEL_ALL:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_E;
		setKBState(COPY);
		break;
	case COPY:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_C;
		setKBState(WINDOWS_3);
		break;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				/* Son los estados para abrir y escribir en la segundo bloc de notas la frase  */
	case WINDOWS_3:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ESCAPE;
		setKBState(DELAY_6);
		break;
	case DELAY_6:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(NOTEPAD2_N);
		break;
	case NOTEPAD2_N:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_N;
		setKBState(NOTEPAD2_O);
		break;
	case NOTEPAD2_O:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_O;
		setKBState(NOTEPAD2_T);
		break;
	case NOTEPAD2_T:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_T;
		setKBState(NOTEPAD2_E);
		break;
	case NOTEPAD2_E:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_E;
		setKBState(NOTEPAD2_P);
		break;
	case NOTEPAD2_P:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_P;
		setKBState(NOTEPAD2_A);
		break;
	case NOTEPAD2_A:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_A;
		setKBState(NOTEPAD2_D);
		break;
	case NOTEPAD2_D:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_D;
		setKBState(DELAY_7);
		break;
	case DELAY_7:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(ENTER_3);
		break;
	case ENTER_3:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_ENTER;
		setKBState(DELAY_8);
		break;
	case DELAY_8:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		setKBState(ALT_2);
		break;
	case ALT_2:
		delay_msOrus(1000,120000000,0);
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_ALT;
		setKBState(SPACE_3);
		break;
	case SPACE_3:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_SPACEBAR;
		setKBState(KEY_X_2);
		break;
	case KEY_X_2:
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	KEY_X;
		setKBState(IDLE);
		setMouseState(TOP_2);
		break;
/* Estado para poder pegar la frase en el segundo bloc de notas*/
	case PASTE:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = MODIFERKEYS_LEFT_CTRL;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = KEY_V;
		setKBState(IDLE);
		break;


	default:
		s_UsbDeviceHidKeyboard.buffer[MODIFERKEYS_BUFFER] = NO_KEYS;
		s_UsbDeviceHidKeyboard.buffer[NORMALKEYS_BUFFER] = 	NO_KEYS;
		break;
	}

    return USB_DeviceHidSend(s_UsbDeviceComposite->hidKeyboardHandle, USB_HID_KEYBOARD_ENDPOINT_IN,
                             s_UsbDeviceHidKeyboard.buffer, USB_HID_KEYBOARD_REPORT_LENGTH);
}

usb_status_t USB_DeviceHidKeyboardCallback(class_handle_t handle, uint32_t event, void *param)
{
    usb_status_t error = kStatus_USB_Error;

    switch (event)
    {
        case kUSB_DeviceHidEventSendResponse:
            if (s_UsbDeviceComposite->attach)
            {
                return USB_DeviceHidKeyboardAction();
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

usb_status_t USB_DeviceHidKeyboardSetConfigure(class_handle_t handle, uint8_t configure)
{
    if (USB_COMPOSITE_CONFIGURE_INDEX == configure)
    {
        return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardSetInterface(class_handle_t handle, uint8_t interface, uint8_t alternateSetting)
{
    if (USB_HID_KEYBOARD_INTERFACE_INDEX == interface)
    {
        return USB_DeviceHidKeyboardAction(); /* run the cursor movement code */
    }
    return kStatus_USB_Error;
}

usb_status_t USB_DeviceHidKeyboardInit(usb_device_composite_struct_t *deviceComposite)
{
    s_UsbDeviceComposite = deviceComposite;
    s_UsbDeviceHidKeyboard.buffer = s_KeyboardBuffer;
    return kStatus_USB_Success;
}
