/*
 *--------------------------------------
 * Program Name: Terminal CE
 * Author: SuperbeeeLavaman
 * License: Apache License 2.0
 * Description: Dumb Terminal application for TI-84 Plus CE
 *--------------------------------------
*/

#include <srldrvce.h>
#include <sys/lcd.h>
#include <debug.h>
#include <keypadc.h>
#include <stdbool.h>
#include <string.h>
#include <tice.h>
#include <fileioc.h>
#include <debug.h>

srl_device_t srl;

bool has_srl_device = false;

uint8_t srl_buf[512];

long* vram = (long*)lcd_Ram;

static usb_error_t handle_usb_event(usb_event_t event, void *event_data,
                                    usb_callback_data_t *callback_data __attribute__((unused))) {
    usb_error_t err;
    /* Delegate to srl USB callback */
    if ((err = srl_UsbEventCallback(event, event_data, callback_data)) != USB_SUCCESS)
        return err;
    /* Enable newly connected devices */
    if(event == USB_DEVICE_CONNECTED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE)) {
        usb_device_t device = event_data;
        dbg_printf("device connected\n");
        usb_ResetDevice(device);
    }

    /* Call srl_Open on newly enabled device, if there is not currently a serial device in use */
    if(event == USB_HOST_CONFIGURE_EVENT || (event == USB_DEVICE_ENABLED_EVENT && !(usb_GetRole() & USB_ROLE_DEVICE))) {

        /* If we already have a serial device, ignore the new one */
        if(has_srl_device) return USB_SUCCESS;

        usb_device_t device;
        if(event == USB_HOST_CONFIGURE_EVENT) {
            /* Use the device representing the USB host. */
            device = usb_FindDevice(NULL, NULL, USB_SKIP_HUBS);
            if(device == NULL) return USB_SUCCESS;
        } else {
            /* Use the newly enabled device */
            device = event_data;
        }

        /* Initialize the serial library with the newly attached device */
        srl_error_t error = srl_Open(&srl, device, srl_buf, sizeof srl_buf, SRL_INTERFACE_ANY, 9600);
        if(error) {
            /* Print the error code to the homescreen */
            dbg_printf("Error %d initting serial\n", error);
            return USB_SUCCESS;
        }

        dbg_printf("serial initialized\n");

        has_srl_device = true;
    }

    if(event == USB_DEVICE_DISCONNECTED_EVENT) {
        usb_device_t device = event_data;
        if(device == srl.dev) {
            dbg_printf("device disconnected\n");
            srl_Close(&srl);
            has_srl_device = false;
        }
    }
    return USB_SUCCESS;
}

uint8_t fontData;
uint8_t fontBytes[2048];
uint16_t renderGoal[1200];
uint16_t renderCurrent[1200];
int exit_Code = 0;
uint24_t idx = 0;
uint8_t cursorX = 0;
uint8_t cursorY = 0;
bool canUseFont = false;
uint8_t idx8 = 0;

uint8_t colour_red   = 0b0001;
uint8_t colour_green = 0b0010;
uint8_t colour_blue  = 0b0100;
uint8_t colour_gray  = 0b1000;

uint8_t keyboardState = 0x00;

bool newTiles = true;

const char* keyactions[1024];

long lcdregisters;

int begin() {
    kb_EnableOnLatch();
    kb_ClearOnLatch();
    fontData = ti_Open("TermFont", "r");
    if (fontData == 0) {
        dbg_printf("dang, no font :(\n");
        canUseFont = false;
    } else {
        canUseFont = true;
        ti_Read(fontBytes, 1, 2048, fontData);
    }
    lcdregisters = lcd_Control; // Save LCD Register
    lcd_Control = ((lcdregisters & 0xFFF1) | 0b0100); //Set 4bpp Mode

    lcd_Palette[ 0] = 0b0000000000000000; //Set LCD Palette
    lcd_Palette[ 1] = 0b0101010000000000;
    lcd_Palette[ 2] = 0b0000001010100000;
    lcd_Palette[ 3] = 0b0101011010100000;
    lcd_Palette[ 4] = 0b0000000000010101;
    lcd_Palette[ 5] = 0b0101010000010101;
    lcd_Palette[ 6] = 0b0000001010110101;
    lcd_Palette[ 7] = 0b0101011010110101;
    lcd_Palette[ 8] = 0b1010100101001010;
    lcd_Palette[ 9] = 0b1111110101001010;
    lcd_Palette[10] = 0b1010101111101010;
    lcd_Palette[11] = 0b1111111111101010;
    lcd_Palette[12] = 0b1010100101011111;
    lcd_Palette[13] = 0b1111110101011111;
    lcd_Palette[14] = 0b1010101111111111;
    lcd_Palette[15] = 0b1111111111111111; //(end of LCD Palette)

    memset(lcd_Ram, 0, LCD_SIZE / 4);


    const usb_standard_descriptors_t *desc = srl_GetCDCStandardDescriptors();
    /* Initialize the USB driver with our event handler and the serial device descriptors */
    usb_error_t usb_error = usb_Init(handle_usb_event, NULL, desc, USB_DEFAULT_INIT_FLAGS);
    if(usb_error) {
       usb_Cleanup();
       printf("usb init error %u\n", usb_error);
    }

    idx = 0;
    while (idx < 80) {
        renderGoal[idx] = 0x200F;
        renderGoal[idx+1120] = 0x200F;
        idx += 1;
    }
    renderGoal[39] = 0xFC0F;
    return 0;
}

void scroll() {
    memcpy(renderGoal+80, renderGoal+120, 2000);
    memset(renderGoal+1080, 0, 80);
    newTiles = true;
}

int end() {
    usb_Cleanup();
    if (canUseFont) {
        if (ti_Close(fontData) == 0) {
            exit_Code = 1;
        }
    }
    lcd_Control = lcdregisters; // Restore LCD Register (unfuck display)
    return exit_Code;
}

bool step() {
    usb_HandleEvents();
    if(has_srl_device) {
        char in_buf[64];
        /* Read up to 64 bytes from the serial buffer */
        size_t bytes_read = srl_Read(&srl, in_buf, sizeof in_buf);

        /* Check for an error (e.g. device disconneced) */
        if(bytes_read < 0) {
            dbg_printf("error %d on srl_Read\n", bytes_read);
            has_srl_device = false;
        } else if(bytes_read > 0) {
            idx = 0;
            char symbol;
            char escape_code[16];
            bool bs = false;
            while (idx < bytes_read) {
                symbol = in_buf[idx];
                int increment_X = 1;
                if (symbol == 0x27)
                if (symbol == 0x7F) {
                    symbol = 0x20;
                    cursorX -= (cursorX>0);
                    increment_X = 0;
                }
                if (symbol == 0x0D) {
                    cursorX = 0;
                    cursorY += 1;
                    symbol = 0x20;
                    increment_X = 0;
                }
                if (symbol == 0x05) {
                    srl_Write(&srl, "TerminalCE", 10);
                    increment_X = 0;
                    symbol = 0x20;
                }
                if (symbol == 0x08) {
                    if (cursorX != 0) {
                        cursorX -= 1;
                    }
                    symbol = 0x20;
                    bs = true;
                }
                if (cursorX >= 40) {
                    cursorX = 0;
                    cursorY += 1;
                }
                if (cursorY > 25) {
                    scroll();
                    cursorY = 25;
                }
                renderGoal[((40 * (cursorY+2)) + cursorX)] = (256*symbol+0xF0);
                if (!bs) {
                    cursorX += increment_X;
                }
                idx += 1;
            }
            newTiles = true;
        }
        kb_Scan();
        idx8 = 0;
        idx  = 0;
        char send[1] = { 0 };
        uint8_t keys = 0;
        while (idx < 8) {
            keys = kb_Data[idx];
            if (keys) {
                while (idx8 < 8) {
                    if (keys & 0x01) {
                        if (keyboardState > 3) { keyboardState = 0 ; }
                        send[0] = keyactions[(256*keyboardState)+(8*idx)+idx8];
                    }
                    keys = (keys >> 1);
                    idx8 += 1;
                }
            }
            idx += 1;
        }
	if (send[0] != 0) {
        	srl_Write(&srl, send, 1);
	}
    }
    return !kb_On;
}

int renderTile(int indx) {
    unsigned int tileY = indx / 40;
    unsigned int tileX = indx - ( 40 * tileY );
    uint16_t tileData = renderGoal[indx];
    uint8_t symbol = (uint8_t)((tileData & 0xFF00) >> 8);
    uint8_t colour = (uint8_t)(tileData & 0x00FF);
    uint8_t fgColour = 0x00;
    uint8_t bgColour = 0x00;
    bgColour = colour & 0x0F;
    fgColour = (colour & 0xF0) >> 4;
    uint8_t pixelX = 0;
    uint8_t pixelY = 0;
    uint32_t tile[8];
    unsigned long row;
    unsigned char rowData;
    uint32_t pixel;
    int bit;
    while (pixelY < 8) {
        if (canUseFont) {
            rowData = fontBytes[(8*symbol)+pixelY];
        } else {
            rowData = symbol;
        }
        row = 0;

        pixelX = 0;
        pixel = 0;
        while (pixelX < 8) {
            bit = ((rowData >> (7-pixelX)) & 1);
            pixel = bit ? fgColour : bgColour;
            row |= (pixel << (4 * pixelX));
            pixelX++;
        }




        tile[pixelY] = row;
        pixelY += 1;
    }
    pixelY = 0;
    
    while (pixelY < 8) {
        vram[tileX + 40 * (pixelY + (8 * tileY))] = tile[pixelY];
        pixelY++;
    }
    return 0;
}

int draw() {
    int drawIdx = 0;
    int renderedNow = 0;
    if (newTiles == false) {
        return 0;
    }
    while (renderedNow < 160) {
        if (renderCurrent[drawIdx] != renderGoal[drawIdx]) {
            exit_Code = renderTile(drawIdx);
            renderCurrent[drawIdx] = renderGoal[drawIdx];
            if (exit_Code != 0 ) {
                return exit_Code;
            }
            renderedNow += 1;
        }
        if (drawIdx > 1200) {
            newTiles = false;
            return 0;
        }
        drawIdx += 1;
    }
    return 0;
}

int main() {
    exit_Code = begin(); // No rendering allowed!
    if (exit_Code != 0 ) {
        return exit_Code;
    }

    while (step()) { // No rendering allowed in step!
        exit_Code = draw(); // As little non-rendering logic as possible
        if (exit_Code != 0 ) {
            end();
            return exit_Code;
        }
    }
    return end();
}
