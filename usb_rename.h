#include "PluggableUSB.h"

class USBRename : public PluggableUSBModule {
  public:
    USBRename(
      const u8 *product_name = NULL,
      const u8 *manufacturer_name = NULL,
      const u8 *serial_num = NULL,
      const u16 vid = 0x2341,
      const u16 pid = 0x8036);
    int getInterface(uint8_t* interfaceCount);
    int getDescriptor(USBSetup& setup);
    bool setup(USBSetup& setup);

  private:
    const u8 *manufacturer_name;
    const u8 *product_name;
    const u8 *serial_num;
    const u16 vid;
    const u16 pid;
    uint8_t epType[0];
};
