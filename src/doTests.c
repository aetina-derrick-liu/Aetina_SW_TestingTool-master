#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "doTests.h"

void logINFO(char *s){
	printf("%s\n", s);
}

void logPASS(char *s){
	printf("\033[32m%s\033[0m", s);
}

void logWARN(char *s){
	printf("\033[33m%s\033[0m", s);
}

void logFAILED(char *s){
	printf("\033[31m%s\033[0m", s);
}

DeviceInfo* getDeviceInfo(DeviceInfo* devInfo_result){
	/////////////////////////////////
	//What is TOB_GetDeviceInfo
	/////////////////////////////////
	return TOB_GetDeviceInfo(devInfo_result);
}

int doTest_LAN(LAN_CFG *lan_cfg, LAN_RESULT *lan_result){
	logINFO("--------------------------LAN Test------------------------------\n");
	char szMsg[MAX_PATH] = {0};
	
	int nResult = TOB_LAN_testing(lan_cfg, lan_result);
	if(nResult == OK){
		lan_result->nResult = OK;
		sprintf(szMsg, "LAN IP:%s", lan_result->szIP);
		logINFO(szMsg);
		sprintf(szMsg, "LAN[%d]: PASS\n", lan_cfg->nLAN_ID);
		logPASS(szMsg);
	}
	else if (nResult == FAILED){
		lan_result->nResult = FAILED;
		sprintf(szMsg, "LAN[%d]: FAILED\n", lan_cfg->nLAN_ID);
		logFAILED(szMsg);
	}
	else{
		lan_result->nResult = NOT_FOUND_DEVICE;
		sprintf(szMsg, "LAN[%d]: Please Check LAN Port Connect, or MAC Address\n", lan_cfg->nLAN_ID);
		logWARN(szMsg);
	}

	logINFO("--------------------------LAN Done------------------------------\n");
	return nResult;
}

int doTest_WIFI(WIFI_RESULT *wifi_result){
  logINFO("--------------------------WIFI Test-----------------------------\n");
	char szMsg[MAX_PATH] = {0};
	int nResult = TOB_WIFI_testing(wifi_result);
	if(nResult != OK){
		wifi_result->nResult = NOT_FOUND_DEVICE;
		sprintf(szMsg, "WIFI: Please Check WIFI\n");
		logFAILED(szMsg);
	}
	else
	{
		wifi_result->nResult = OK;
		//sprintf(szMsg, "MAC : %s", wifi_result->szMACNum);
		logINFO(szMsg);
		logPASS("WIFI: PASS\n");
	}

	logINFO("--------------------------WIFI Done-----------------------------\n");
	return nResult;
}

int doTest_BT(BT_RESULT *bt_result){
	logINFO("----------------------BlueTooth Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_BT_testing(bt_result);
	if(nResult != OK){
		bt_result->nResult = NOT_FOUND_DEVICE;
		logWARN("BlueTooth: Please Check BlueTooth\n");
		return nResult;
	}
	else
	{
		bt_result->nResult = OK;
		sprintf(szMsg, "MAC : %s", bt_result->szMACNum);
		logINFO(szMsg);
		logPASS("BlueTooth: PASS\n");
	}

	logINFO("----------------------BlueTooth Done----------------------------\n");
	return nResult;
}

int doTest_SDCARD(SDCARD_CFG *sdcard_cfg, SDCARD_RESULT *sdcard_result){
  logINFO("-----------------------Micro SD Test----------------------------\n");

	int nResult = TOB_SDCARD_testing(sdcard_cfg, sdcard_result);
	if(nResult != NOT_FOUND_DEVICE){
		if(nResult == OK){
			sdcard_result = OK;
			logPASS("SDCard: PASS\n");
		}
	}
	else{
		sdcard_result = NOT_FOUND_DEVICE;
		logWARN("SDCard: Not Found SDCard\n");
	}
	
	logINFO("-----------------------Micro SD Done----------------------------\n");
	return nResult;
}

int doTest_mSATA(MSATA_CFG *msata_cfg, MSATA_RESULT *msata_result){
  logINFO("--------------------------mSATA Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};
	
	int nResult = TOB_MSATA_testing(msata_cfg,msata_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			msata_result->nResult = OK;
			sprintf(szMsg, "Result : mSATA Speed(%u MB/s), TimeCost(%llu ms)\n", msata_result->nTransferSpeed, msata_result->nTimeCost_ms);
			logINFO(szMsg);
			logPASS("m-SATA: PASS\n");
		}		
	}
	else{
		msata_result->nResult = NOT_FOUND_DEVICE;
		logWARN("m-SATA: Not Found Device\n");
	}

	logINFO("--------------------------mSATA Done----------------------------\n");
	return nResult;
}

int doTest_mPCIE(MPCIE_CFG *mpcie_cfg, MPCIE_RESULT *mpcie_result){
  logINFO("--------------------------mPCIE Test----------------------------\n");

	int nResult = TOB_MPCIE_testing(mpcie_cfg, mpcie_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			mpcie_result->nResult = OK;
			logPASS("m-PCIE: PASS\n");
		}		
	}
	else{
		mpcie_result->nResult = NOT_FOUND_DEVICE;
		//char szMsg[MAX_PATH] = {0};
		//if (mpcie_result->nResult == NOT_FOUND_DEVICE){

		//	if (mpcie_cfg->nMPCIE_type->pMpciePortPosition == 2 ){
		//		sprintf(szMsg, "Not Found m-PCIE Device On Up Port\n");
		//		logWARN(szMsg);
		//	}
		//	if (mpcie_cfg->nMPCIE_typepMpciePortPosition == 1 ){
		//		sprintf(szMsg, "Not Found m-PCIE Device On Down Port\n");
		//		logWARN(szMsg);
		//	}
		//	else{
		//		sprintf(szMsg,"m-PCIE: Not Found Device\n");
		//		logWARN(szMsg);
		//	}
		//}

		logWARN("m-PCIE: Not Found Device\n");
	}

	logINFO("--------------------------mPCIE Done----------------------------\n");
	return nResult;
}

int doTest_RS232(RS232_CFG *rs232_cfg, RS232_RESULT *rs232_result){
  logINFO("--------------------------RS232 Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_RS232_testing(rs232_cfg, rs232_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			rs232_result = OK;
			sprintf(szMsg, "DevName : %s, Data : %s", rs232_cfg->pszDeviceName, rs232_cfg->pszTestingData);
			logINFO(szMsg);
			logPASS("RS232: PASS\n");
		}
		else{
			rs232_result = FAILED;
			logFAILED("RS232: FAILED\n");
		}
	}
	else{
		rs232_result->nResult = NOT_FOUND_DEVICE;
		logWARN("RS232: Please check RS232 Device\n");
	}
	
	logINFO("--------------------------RS232 Done----------------------------\n");
	return nResult;
}

int doTest_UART(UART_CFG *uart_cfg, UART_RESULT *uart_result){
  logINFO("---------------------------UART Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_UART_Testing(uart_cfg, uart_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			uart_result->nResult = OK;
			sprintf(szMsg, "DevName : %s, Data : %s", uart_cfg->pszDeviceName, uart_cfg->pszTestingData);
			logINFO(szMsg);
			logPASS("UART: PASS\n");
		}
		else{
			uart_result->nResult = FAILED;
			logFAILED("UART: FAILED\n");
		}
	}
	else{
		uart_result->nResult = NOT_FOUND_DEVICE;
		logWARN("UART: Please check UART Device\n");
	}

	logINFO("---------------------------UART Done----------------------------\n");
	return nResult;
}

int doTest_UART2(UART2_CFG *uart2_cfg, UART2_RESULT *uart2_result){
  logINFO("---------------------------UART J518 Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_UART_Testing(uart2_cfg, uart2_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			uart2_result->nResult = OK;
			sprintf(szMsg, "DevName : %s, Data : %s", uart2_cfg->pszDeviceName, uart2_cfg->pszTestingData);
			logINFO(szMsg);
			logPASS("UART J518: PASS\n");
		}
		else{
			uart2_result->nResult = FAILED;
			logFAILED("UART J518: FAILED\n");
		}
	}
	else{
		uart2_result->nResult = NOT_FOUND_DEVICE;
		logWARN("UART: Please check UART J518 Device\n");
	}

	logINFO("---------------------------UART J518 Done----------------------------\n");
	return nResult;
}

int doTest_UART_CSI(UART_CSI_CFG *uart_CSI_cfg, UART_RESULT *uart_CSI_result){
  logINFO("---------------------------UART CSI Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_UART_CSI_Testing(uart_CSI_cfg, uart_CSI_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			uart_CSI_result->nResult = OK;
			sprintf(szMsg, "DevName : %s, Data : %s", uart_CSI_cfg->pszDeviceName, uart_CSI_cfg->pszTestingData);
			logINFO(szMsg);
			logPASS("UART CSI: PASS\n");
		}
		else{
			uart_CSI_result->nResult = FAILED;
			logFAILED("UART CSI: FAILED\n");
		}
	}
	else{
		uart_CSI_result->nResult = NOT_FOUND_DEVICE;
		logWARN("UART CSI: Please check UART CSI Device\n");
	}

	logINFO("---------------------------UART CSI Done----------------------------\n");
	return nResult;
}

int doTest_GPIO(GPIO_CFG *gpio_cfg, GPIO_RESULT *gpio_result){
  logINFO("---------------------------GPIO Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_GPIO_Testing(gpio_cfg, gpio_result);
	for(int i = 0 ; i < gpio_cfg->nGPIO_Pins_Number ; i++){
		sprintf(szMsg, "GPIO[%d]: Number %d : %s", i+1, gpio_cfg->pGPIO_Pins[i], gpio_result->nPinResults[i] == OK ? "PASS" : "FAILED");
		logINFO(szMsg);
	}

	if( nResult != OK){
		gpio_result->nResult = FAILED;
		logFAILED("GPIO: FAILED\n");
	}
	else{
		gpio_result->nResult = OK;
		logPASS("GPIO: PASS\n");
	}
	
	logINFO("---------------------------GPIO Done----------------------------\n");
	return nResult;
}

int doTest_FAN(FAN_CFG *fan_cfg, FAN_RESULT *fan_result){
  logINFO("----------------------------Fan Test----------------------------\n");

	int nResult = TOB_FAN_Testing(fan_cfg, fan_result);
	if( nResult != OK){
		fan_result = FAILED;
		logFAILED("FAN: FAILED\n");
	}
	else
	{
		fan_result->nResult = OK;
		logPASS("FAN: PASS\n");
	}

	logINFO("----------------------------Fan Done----------------------------\n");
	return nResult;
}

int doTest_eDP(EDP_CFG *eDP_cfg, EDP_RESULT *eDP_result){
  logINFO("----------------------------eDP Test----------------------------\n");

	int nResult = TOB_EDP_Testing(eDP_cfg, eDP_result);
	if( nResult != OK){
		eDP_result = FAILED;
		logFAILED("eDP: FAILED\n");
	}
	else
	{
		eDP_result->nResult = OK;
		logPASS("eDP: PASS\n");
	}

	logINFO("----------------------------eDP Done----------------------------\n");
	return nResult;
}

int doTest_CANbus(CAN_CFG *can_cfg, CAN_RESULT *can_result){
  logINFO("-------------------------CAN Bus Test---------------------------\n");
	int nResult = TOB_CANBus_Testing(can_cfg, can_result);
	if( nResult != OK){
		can_result->nResult = FAILED;
		logFAILED("CAN Bus: FAILED\n");
	}	
	else{
		can_result->nResult = OK;
		logPASS("CAN Bus: PASS\n");
	}

	logINFO("-------------------------CAN Bus Done---------------------------\n");
	return nResult;
}

int doTest_I2C(I2C_CFG *i2c_cfg, I2C_RESULT *i2c_result){
  logINFO("----------------------------I2C Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_I2C_Testing(i2c_cfg, i2c_result);
	if( nResult != OK){
		i2c_result->nResult = FAILED;
		logFAILED("I2C: FAILED\n");
	}
	else{
		i2c_result->nResult = OK;
		sprintf(szMsg, "I2C-%d, Address : 0x%2X, Data : 0x%2X", i2c_cfg->nI2C_ID, i2c_cfg->nAddress, i2c_cfg->nData);
		logPASS("I2C: PASS\n");
	}

	logINFO("----------------------------I2C Done----------------------------\n");
	return nResult;
}

int doTest_I2C_II(I2C_II_CFG *i2c_II_cfg, I2C_II_RESULT *i2c_II_result){
  logINFO("----------------------------I2C  II Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_I2C_Testing(i2c_II_cfg, i2c_II_result);
	if( nResult != OK){
		i2c_II_result->nResult = FAILED;
		logFAILED("I2C CSI: FAILED\n");
	}
	else{
		i2c_II_result->nResult = OK;
		sprintf(szMsg, "I2C-%d, Address : 0x%2X, Data : 0x%2X", i2c_II_cfg->nI2C_ID, i2c_II_cfg->nAddress, i2c_II_cfg->nData);
		logPASS("I2C II: PASS\n");
	}

	logINFO("----------------------------I2C  II Done----------------------------\n");
	return nResult;
}

int doTest_I2C_CSI(I2C_CSI_CFG *i2c_CSI_cfg, I2C_CSI_RESULT *i2c_CSI_result){
  logINFO("----------------------------I2C CSI Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_I2C_Testing(i2c_CSI_cfg, i2c_CSI_result);
	if( nResult != OK){
		i2c_CSI_result->nResult = FAILED;
		logFAILED("I2C CSI: FAILED\n");
	}
	else{
		i2c_CSI_result->nResult = OK;
		sprintf(szMsg, "I2C-%d, Address : 0x%2X, Data : 0x%2X", i2c_CSI_cfg->nI2C_ID, i2c_CSI_cfg->nAddress, i2c_CSI_cfg->nData);
		logPASS("I2C CSI: PASS\n");
	}

	logINFO("----------------------------I2C CSI Done----------------------------\n");
	return nResult;
}

int doTest_SPI(SPI_CFG *spi_cfg, SPI_RESULT *spi_result){
  logINFO("----------------------------SPI Test----------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_SPI_testing(spi_cfg, spi_result);
	if(nResult != NOT_FOUND_DEVICE){
		sprintf(szMsg, "DevName : %s", spi_cfg->pszDeviceName);
		if (nResult == OK){
			spi_result->nResult = OK;
			logINFO(szMsg);
			logPASS("SPI: PASS\n");
		}
		else{
			spi_result->nResult = FAILED;
			logFAILED("SPI: FAILED\n");
		}
	}
	else{
		spi_result->nResult = NOT_FOUND_DEVICE;
		logWARN("SPI: Not Found Device\n");
	}

	logINFO("----------------------------SPI Done----------------------------\n");
	return nResult;
}

int doTest_USB(USB_CFG *usb_cfg, USB_RESULT *usb_result){
  char szMsg[4096] = {0};
	char szUSB3_UP_Msg[4096] = {0};
	char szUSB3_DOWN_Msg[4096] = {0};
	bool bUSB3_UP_Exist = false, bUSB3_DOWN_Exist = false;

	int nResult = TOB_USB_Testing(usb_cfg, usb_result);
	
	for(int i = 0 ; i < usb_result->nResultNum ; i++){
		char szResult[MAX_CMD] = {0};
		sprintf(szResult, "Device[%d]: Type : %s, Port : %s , TimeCost : %llu(ms), Speed : %u\n", i, 
																				usb_result->pUSBType[i] == USB_3_0 ? "USB3.0" : "USB2.0", 
																				usb_result->pPortPosition == UP ? "Up" : "Down", 
																				usb_result->pTimeCost_ms[i], 
																				usb_result->pTransferSpeed[i]);

		if(usb_result->pPortPosition[i] == UP){
			strcat(szUSB3_UP_Msg, szResult);
		}
		else if(usb_result->pPortPosition[i] == DOWN){
			strcat(szUSB3_DOWN_Msg, szResult);
		}
		else{
			strcat(szMsg, szResult);
		}
	}

	switch(nResult){
		case NOT_FOUND_DEVICE:
			usb_result->nResult = NOT_FOUND_DEVICE;
			if (usb_cfg->nInterfaceType == USB_3_0_TYPE_A){
				if ((usb_result->nUsbFoundDevice & 0xff)){
					if (!(usb_result->nUsbFoundDevice & USB_3_0_TYPE_A_UP_FOUND)){
						sprintf(szMsg, "USB Type-A: Not Found USB 3.0 Device On Up Port\n");
						logWARN(szMsg);
					}
					if (!(usb_result->nUsbFoundDevice & USB_3_0_TYPE_A_DOWN_FOUND)){
						sprintf(szMsg, "USB Type-A: Not Found USB 3.0 Device On Down Port\n");
						logWARN(szMsg);
					}
					if (!(usb_result->nUsbFoundDevice & USB_2_0_TYPE_A_UP_FOUND)){
						sprintf(szMsg, "USB Type-A: Not Found USB 2.0 Device On Up Port\n");
						logWARN(szMsg);
					}
					if (!(usb_result->nUsbFoundDevice & USB_2_0_TYPE_A_DOWN_FOUND)){
						sprintf(szMsg, "USB Type-A: Not Found USB 2.0 Device On Down Port\n");
						logWARN(szMsg);
					}
				}
				else{
					sprintf(szMsg,"USB Type-A: Not Found Any USB Device On Type-A Port\n");
					logWARN(szMsg);
				}
			}
			else if (usb_cfg->nInterfaceType == USB_3_0_TYPE_C){
				sprintf(szMsg, "Not Found Any USB Device On Type-C Port\n");
				logWARN(szMsg);
			}
			else{
				sprintf(szMsg, "Not Found Any USB Device On Micro-USB Port\n");
				logWARN(szMsg);
			}
			
			break;
		case OK:
			usb_result->nResult = OK;
			switch(usb_cfg->nInterfaceType){
				case USB_3_0_TYPE_C:
					sprintf(szMsg, "USB Type-C: PASS\n");
					break;
				case MICRO_USB:
					sprintf(szMsg, "Micro-USB: PASS\n");
					break;
				case USB_3_0_TYPE_A:
					sprintf(szMsg, "USB Type-A: PASS\n");
					break;
			}
			logPASS(szMsg);
			break;
	}
  
	return nResult;
}

int doTest_USB_Micro(USB_CFG *usb_cfg, USB_RESULT *usb_micro_result){
  logINFO("------------------------Micro USB Test--------------------------\n");
	usb_cfg->nInterfaceType = MICRO_USB;
	int nResult = doTest_USB(usb_cfg, usb_micro_result);
	logINFO("------------------------Micro USB Done--------------------------\n");
	return nResult;
}

int doTest_USB_Type_A(USB_CFG *usb_cfg, USB_RESULT* usb_type_a_result){
  logINFO("------------------------USB Type-A Test-------------------------\n");
	usb_cfg->nInterfaceType = USB_3_0_TYPE_A;
	int nResult = doTest_USB(usb_cfg, usb_type_a_result);
	logINFO("------------------------USB Type-A Done-------------------------\n");
	return nResult;
}

int doTest_USB_Type_C(USB_CFG *usb_cfg, USB_RESULT *usb_type_c_result){
  logINFO("------------------------USB Type-C Test-------------------------\n");
	usb_cfg->nInterfaceType = USB_3_0_TYPE_C;
	int nResult = doTest_USB(usb_cfg, usb_type_c_result);
	logINFO("------------------------USB Type-C Done-------------------------\n");
	return nResult;
}

int doTest_m2_eKey(M2_EKEY_CFG *ekey_cfg, M2_EKEY_RESULT *ekey_result){
  logINFO("------------------------M.2 E-Key Test--------------------------\n");

	int nResult = TOB_M2_EKEY_Testing(ekey_cfg, ekey_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			ekey_result->nResult = OK;
			logPASS("M.2 E Key: PASS\n");
		}
		else{
			ekey_result->nResult = FAILED;
			logFAILED("M.2 E Key: FAILED\n");
		}
	}
	else{
		ekey_result->nResult = NOT_FOUND_DEVICE;
		logWARN("M.2 E Key: Not Found Device\n");
	}

	logINFO("------------------------M.2 E-Key Done--------------------------\n");
	return nResult;
}

int doTest_m2_bKey(M2_BKEY_CFG *bkey_cfg, M2_BKEY_RESULT *bkey_result){
  logINFO("------------------------M.2 B-Key Test--------------------------\n");

	int nResult = TOB_M2_BKEY_Testing(bkey_cfg, bkey_result);
	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			bkey_result->nResult = OK;
			logPASS("M.2 B Key: PASS\n");
		}
		else{
			bkey_result->nResult = FAILED;
			logFAILED("M.2 B Key: FAILED\n");
		}
	}
	else{
		bkey_result->nResult = NOT_FOUND_DEVICE;
		logWARN("M.2 E Key: Not Found Device\n");
	}

	logINFO("------------------------M.2 B-Key Done--------------------------\n");
	return nResult;
}

int doTest_m2_mKey(M2_MKEY_CFG *mkey_cfg, M2_MKEY_RESULT *mkey_result){
	int nResult = TOB_M2_MKEY_Testing(mkey_cfg, mkey_result);
	char szMsg[MAX_PATH] = {0};

	if(nResult != NOT_FOUND_DEVICE){
		if (nResult == OK){
			mkey_result->nResult = OK;
			sprintf(szMsg, "Speed(%u MB/s), TimeCost(%llu ms)\n", mkey_result->nTransferSpeed, mkey_result->nTimeCost_ms);
			logINFO(szMsg);
			if (mkey_cfg->nDeviceType == PCIE){
				logPASS("M.2 M Key(PCIE): PASS\n");
			}
			else{
				logPASS("M.2 M Key(SATA): PASS\n");
			}
		}	
	}
	else{
		mkey_result->nResult = NOT_FOUND_DEVICE;
		if (mkey_cfg->nDeviceType == PCIE){
			logWARN("Not Found PCIE Storage\n");
		}
		else{
			logWARN("Not Found SATA Storage\n");
		}
	}

	return nResult;
}

int doTest_m2_mKey_SATA(M2_MKEY_CFG *mkey_cfg, M2_MKEY_RESULT *mkey_result){
	logINFO("--------------------M.2 M Key SATA Test-------------------------\n");
	mkey_cfg->nDeviceType = SATA;
	int nResult = doTest_m2_mKey(mkey_cfg, mkey_result);
	logINFO("--------------------M.2 M Key SATA Done-------------------------\n");
	return nResult;
}

int doTest_m2_mKey_PCIE(M2_MKEY_CFG *mkey_cfg, M2_MKEY_RESULT *mkey_result){
	logINFO("--------------------M.2 M Key PCIE Test-------------------------\n");
	mkey_cfg->nDeviceType = PCIE;
	int nResult = doTest_m2_mKey(mkey_cfg, mkey_result);
	logINFO("--------------------M.2 M Key PCIE Done-------------------------\n");
	return nResult;
}

int doTest_EEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_RESULT *eeprom_result){
	logINFO("--------------------------EEPROM Test---------------------------\n");
	char szContent[MAX_PATH] = {0};
	int nResult = FAILED;

	if(TOB_WriteEEPROM(eeprom_cfg, eeprom_result) != OK){
		logWARN("Write EEPROM FAILED");
	}

	memset(szContent, 0xFF, strlen(eeprom_cfg->pszContent));
	if(TOB_ReadEEPROM(eeprom_cfg, eeprom_result) == OK){
		if(strcmp(eeprom_cfg->pszContent, eeprom_result->pszResult) == 0){
			eeprom_result->nResult = OK;
			nResult = OK;
			logPASS("EEPROM: PASS\n");
		}
		else{
			eeprom_result->nResult = FAILED;
			nResult = FAILED;
			logFAILED("EEPROM: FAILED\n");
		}
	}
	else
	{
		logWARN("Read EEPROM FAILED");
	}
	logINFO("--------------------------EEPROM Done---------------------------\n");
	return nResult;
}

int doTest_Camera(CAMERA_CFG *camera_cfg, CAMERA_RESULT *camera_result){
	logINFO("--------------------------Camera Test---------------------------\n");

	int nResult = TOB_Camera_Testing(camera_cfg, camera_result);
	if (nResult == OK){
		camera_result->nResult = OK;
		logPASS("Camera: PASS\n");
	}
	else{
		camera_result->nResult = FAILED;
		logFAILED("Camera: FAILED\n");
	}

	logINFO("--------------------------Camera Done---------------------------\n");
	return nResult;
}

int doTest_Temperature(TEMPERATURE_CFG *temperature_cfg, TEMPERATURE_RESULT *temperature_result){
	logINFO("---------------------Temperature Test---------------------------\n");
	char szMsg[MAX_PATH] = {0};

	int nResult = TOB_Temperature_Testing(temperature_cfg, temperature_result);
	if (nResult == OK){
		temperature_result->nResult = OK;
		sprintf(szMsg, "Temperature: %f (C)", temperature_result->nTemperature);
		logINFO(szMsg);
		logPASS("Temperature: PASS\n");
	}
	else{
		temperature_result->nResult = FAILED;
		sprintf(szMsg, "Temperature: FAILEDn");
		logPASS(szMsg);
	}

	logINFO("---------------------Temperature Done---------------------------\n");
	return nResult;
}
