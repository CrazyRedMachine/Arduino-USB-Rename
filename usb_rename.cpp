#include "usb_rename.h"

extern const DeviceDescriptor PROGMEM USB_DeviceDescriptorIAD;

static
bool SendControl(u8 d)
{
  return USB_SendControl(0, &d, 1) == 1;
}

// Send a USB descriptor string. The string is stored in PROGMEM as a
// plain ASCII string but is sent out as UTF-16 with the correct 2-byte
// prefix
// taken from Arduino's USBCore.cpp
static bool USB_SendStringDescriptor(const u8*string_P, u8 string_len, uint8_t flags) {
        SendControl(2 + string_len * 2);
        SendControl(3);
        bool pgm = flags & TRANSFER_PGM;
        for(u8 i = 0; i < string_len; i++) {
                bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
                r &= SendControl(0); // high byte
                if(!r) {
                        return false;
                }
        }
        return true;
}

const DeviceDescriptor USB_DeviceDescriptorIAD;

USBRename::USBRename(
      const u8 *product_name = NULL,
      const u8 *manufacturer_name = NULL,
      const u8 *serial_num = NULL,
      const u16 vid = 0x8036,
      const u16 pid = 0x2341) :
        manufacturer_name(manufacturer_name),
        product_name(product_name),
        serial_num(serial_num),
        PluggableUSBModule(1, 1, epType) {
    vid = vid;
    pid = pid;
    USB_DeviceDescriptorIAD =
  D_DEVICE(0xEF,0x02,0x01,64,vid,pid,0x100,IMANUFACTURER,IPRODUCT,ISERIAL,1);

    PluggableUSB().plug(this);
}

int USBRename::getInterface(uint8_t* interfaceCount) {
    return 0;
}

int USBRename::getDescriptor(USBSetup& setup)
{
    if(setup.wValueH == USB_DEVICE_DESCRIPTOR_TYPE) {
        return USB_SendControl(TRANSFER_PGM, (const u8*)&USB_DeviceDescriptorIAD, sizeof(USB_DeviceDescriptorIAD));
    }
  
    if(setup.wValueH != USB_STRING_DESCRIPTOR_TYPE) {
        return 0;
    }

    if(setup.wValueL == IMANUFACTURER && manufacturer_name) {
        return USB_SendStringDescriptor(manufacturer_name, strlen(manufacturer_name), 0);
    } else if (setup.wValueL == IPRODUCT && product_name) {
        return USB_SendStringDescriptor(product_name, strlen(product_name), 0);
    } else if (setup.wValueL == ISERIAL && serial_num) {
        return USB_SendStringDescriptor(serial_num, strlen(serial_num), 0);
    }

    return 0;
}

bool USBRename::setup(USBSetup& setup)
{
    return false;
}
