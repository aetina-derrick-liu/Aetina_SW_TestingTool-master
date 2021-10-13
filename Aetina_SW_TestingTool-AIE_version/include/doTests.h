
#include "utils/libTestOfBoard.h"

void logINFO(char*);
void logPASS(char*);
void logWARN(char*);
void logFAILED(char*);

DeviceInfo* getDeviceInfo(DeviceInfo*);
int  doTest_LAN(LAN_CFG*, LAN_RESULT*);
int  doTest_WIFI(WIFI_RESULT*);
int  doTest_BT(BT_RESULT*);
int  doTest_SDCARD(SDCARD_CFG*, SDCARD_RESULT*);
int  doTest_mSATA(MSATA_CFG*, MSATA_RESULT*);
int  doTest_mPCIE(MPCIE_CFG*, MPCIE_RESULT*);
int  doTest_RS232(RS232_CFG*, RS232_RESULT*);
int  doTest_UART(UART_CFG*, UART_RESULT*);
int  doTest_UART2(UART2_CFG*, UART2_RESULT*);
int  doTest_GPIO(GPIO_CFG*, GPIO_RESULT*);
int  doTest_FAN(FAN_CFG*, FAN_RESULT*);
int  doTest_eDP(EDP_CFG*, EDP_RESULT*);
int  doTest_CANbus(CAN_CFG*, CAN_RESULT*);
int  doTest_I2C(I2C_CFG*, I2C_RESULT*);
int  doTest_I2C_II(I2C_II_CFG*, I2C_II_RESULT*);
int  doTest_I2C_CSI(I2C_CSI_CFG*, I2C_CSI_RESULT*);
int  doTest_SPI(SPI_CFG*, SPI_RESULT*);
int  doTest_USB(USB_CFG*, USB_RESULT*);
int  doTest_USB_Micro(USB_CFG*, USB_RESULT*);
int  doTest_USB_Type_A(USB_CFG*, USB_RESULT*);
int  doTest_USB_Type_C(USB_CFG*, USB_RESULT*);
int  doTest_m2_eKey(M2_EKEY_CFG*, M2_EKEY_RESULT*);
int  doTest_m2_bKey(M2_BKEY_CFG*, M2_BKEY_RESULT*);
int  doTest_m2_mKey(M2_MKEY_CFG*, M2_MKEY_RESULT*);
int  doTest_m2_mKey_SATA(M2_MKEY_CFG*, M2_MKEY_RESULT*);
int  doTest_m2_mKey_PCIE(M2_MKEY_CFG*, M2_MKEY_RESULT*);
int  doTest_EEPROM(EEPROM_CFG*, EEPROM_RESULT*);
int  doTest_Camera(CAMERA_CFG*, CAMERA_RESULT*);
int  doTest_Temperature(TEMPERATURE_CFG*, TEMPERATURE_RESULT*);