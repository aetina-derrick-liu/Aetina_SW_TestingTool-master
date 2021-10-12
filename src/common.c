#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "common.h"

char CONFIG_LOCATION[] = "/opt/aetina/bin/config.ini";

static const char items_name[29][15] = {
	"TIMES",
	"LAN",
	"WIFI",
	"BT",
	"SDCARD",
	"MSATA",
	"MPCIE",
	"RS232",
	"UART",
	"UART2",
	"UART_CSI",
	"GPIO",
	"FAN",
	"eDP",
	"CANbus",
	"I2C",
	"I2C_II",
	"I2C_CSI",
	"SPI",
	"USB_MICRO",
	"USB_A",
	"USB_C",
	"M2EKEY",
	"M2BKEY",
	"M2MKEY_SATA",
	"M2MKEY_PCIE",
	"EEPROM",
	"CAMERA",
	"TEMPERATURE"
};

int checkConfigItem(char *tag, int *value){
	char szCMD[MAX_CMD] = {0};
	dictionary  *ini = NULL;
	int	nResult		= 0;

	if(tag == NULL || strlen(tag) == 0)
		return INVALID_ARG;

	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL){
		sprintf(szCMD, "%s:%s", "ITEMS", tag);
		nResult = iniparser_getint(ini, szCMD, -1);

		*value = nResult;
		iniparser_freedict(ini);
		return OK;	
	}	

	return FAILED;
}

int setConfigItem(char *tag, int *nvalue){
	char szCMD[MAX_CMD] = {0};
	char szMsg[MAX_PATH] = {0};
	char szValue[10] ={0};
	dictionary *ini = NULL;
	int nResult = 0;

	if(tag == NULL || strlen(tag) == 0)
		return INVALID_ARG;

	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL) {
		sprintf(szCMD, "%s:%s", "ITEMS", tag);
		sprintf(szValue, "%d", nvalue);
		if(iniparser_find_entry(ini, szCMD) != 0){
			if(iniparser_set(ini, szCMD, szValue) == 0){
				nResult = iniparser_save(ini, CONFIG_LOCATION);
				iniparser_freedict(ini);
				return nResult;
			}
		}
	}

	logWARN("Cannot parse config file\n");
	iniparser_freedict(ini);
	return FAILED;
}

void resetConfigItem(void){
	char szMsg[MAX_PATH] = {0};
	char szCMD[MAX_PATH] = {0};
	char szValue[10] = {0};
	int nResult = OK;

	dictionary *ini = NULL;
	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL){
		sprintf(szCMD, "%s:%s", "ITEMS", items_name[0]);
		sprintf(szValue, "%d", 0);
		if (iniparser_find_entry(ini, szCMD) != 0){
			iniparser_set(ini, szCMD, szValue);
		}
		
		size_t size = sizeof(items_name)/sizeof(items_name[0]);
		for (int i=1;i<size;i++){
			sprintf(szCMD, "%s:%s", "ITEMS", items_name[i]);
			sprintf(szValue, "%d", FAILED);
			if (iniparser_find_entry(ini, szCMD) != 0){
				iniparser_set(ini, szCMD, szValue);
			}
		}
		nResult = iniparser_save(ini, CONFIG_LOCATION);
		iniparser_freedict(ini);
		return nResult;
	}
	else{
		logWARN("Cannot parse config file");
	}

	iniparser_freedict(ini);
	return;
}

char* checkConfigInfo(char* tag, char *value){
	char szCMD[MAX_CMD] = {0};
	dictionary  *ini = NULL;
	char *buf = NULL;

	if(tag == NULL || strlen(tag) == 0)
		return NULL;

	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL){
		sprintf(szCMD, "%s:%s", "INFOS", tag);
		buf = iniparser_getstring(ini, szCMD, "NULL");
		if (!strstr(buf, "NULL")){
			value = malloc(strlen(buf) * sizeof(char));
			strcpy(value, buf);
			iniparser_freedict(ini);
			return value;
		}
	}	
	iniparser_freedict(ini);

	return NULL;
}

int setConfigInfo(char* tag, char *value){
	char szCMD[MAX_CMD] = {0};
	char szValue[40] = {0};
	dictionary  *ini = NULL;
	int nResult = 0;

	if(tag == NULL || strlen(tag) == 0)
		return INVALID_ARG;

	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL){
		sprintf(szCMD, "%s:%s", "INFOS", tag);
		sprintf(szValue, "%s", value);
		if (iniparser_find_entry(ini, szCMD) != 0){
			if (iniparser_set(ini, szCMD, szValue) == 0){
				nResult = iniparser_save(ini, CONFIG_LOCATION);
				iniparser_freedict(ini);
				return nResult;
			}
		}
	}	
	iniparser_freedict(ini);

	return FAILED;
}

void resetConfigInfo(void){
	char szMsg[MAX_PATH] = {0};
	char szCMD[MAX_PATH] = {0};
	char szValue[10] = {0};
	int nResult = OK;

	dictionary *ini = NULL;
	ini = iniparser_load(CONFIG_LOCATION);
	if (ini != NULL){
		sprintf(szCMD, "%s:%s", "INFOS", "gondannumber");
		sprintf(szValue, "%s", "NULL");
		if (iniparser_find_entry(ini, szCMD) != 0){
			if (iniparser_set(ini, szCMD, szValue) != 0){
			#ifdef _DEBUG_
				logWARN("Cannot set config info 工單號碼");
			#endif
			}
		}

		memset(szCMD, 0, sizeof(szCMD));
		sprintf(szCMD, "%s:%s", "INFOS", "partnumber");
		if (iniparser_find_entry(ini, szCMD) != 0){
			if (iniparser_set(ini, szCMD, szValue) != 0){
			#ifdef _DEBUG_
				logWARN("Cannot set config info part number");
			#endif
			}
		}

		memset(szCMD, 0, sizeof(szCMD));
		sprintf(szCMD, "%s:%s", "INFOS", "boardnumber");
		if (iniparser_find_entry(ini, szCMD) != 0){
			if (iniparser_set(ini, szCMD, szValue) != 0){
			#ifdef _DEBUG_
				logWARN("Cannot set config info board number");
			#endif
			}
		}
		
		nResult = iniparser_save(ini, CONFIG_LOCATION);
	}
	else{
	#ifdef _DEBUG_
		logWARN("Cannot parse config file");
	#endif
	}

	iniparser_freedict(ini);
	return;
}

Items* ItemsInit(void){
	Items *items = malloc(sizeof(Items));
	items->item_kind = 0;
	items->next = NULL;
	return items;
}

Items* addItem(Items *items, ItemKind e){
	Items *item_end = NULL;
	if (items == NULL){
		items = (Items*)malloc(sizeof(Items));
		items->item_kind = e;
		items->next = NULL;
	}
	else
	{
		items->next = addItem(items->next, e);
	}

	return items;
}

Items* findItem(Items *items, ItemKind e){
	Items *pItem = NULL;
	if (items->item_kind == e){
		return items;
	}
	else{
		pItem = items->next;
		while(pItem != NULL){
			if (pItem->item_kind == e){
				return pItem;
			}
			pItem = pItem->next;
		}
	}
	
	return NULL;
}

void showAddedItems(Items *test_items){
	Items *pItem = test_items;
	char szMsg[256] = {0};
	int count = 0;

	while (pItem != NULL){
		sprintf(szMsg, "Item[%d]:%s", count, items_name[pItem->item_kind]);
		logINFO(szMsg);
		pItem = pItem->next;
		count++;
	}

	return;
}

void showNonFinishItems(Items *failed_items){
	char szMsg[256] = {0};
	Items *pItem = failed_items;
	sprintf(szMsg, "Not Finish Items:\n");
	logFAILED(szMsg);
	while (pItem != NULL)
	{
		sprintf(szMsg, "%s, ", items_name[pItem->item_kind]);
		logWARN(szMsg);
		pItem = pItem->next;
	}
	logINFO("\nPlease [Enter] to shutdown system");
	
	return;
}

TestConfigs* TestConfigsInit(void){
	TestConfigs *pConfigs = malloc(sizeof(TestConfigs));
	memset(pConfigs->mTmp_Path, 0, MAX_PATH * sizeof(char));
	memset(pConfigs->mTmp_File, 0, MAX_PATH * sizeof(char));
	pConfigs->mLAN_Count = 0;
	pConfigs->mCamera_Count = 0;
	pConfigs->pCfg_LAN     = NULL;
	pConfigs->pCfg_SDCard = NULL;
	pConfigs->pCfg_mSATA = NULL;
	pConfigs->pCfg_mPCIE = NULL;
	pConfigs->pCfg_RS232 = NULL;
	pConfigs->pCfg_UART = NULL;
	pConfigs->pCfg_UART2 = NULL;
	pConfigs->pCfg_UART_CSI = NULL;
	pConfigs->pCfg_GPIO = NULL;
	pConfigs->pCfg_FAN = NULL;
	pConfigs->pCfg_eDP = NULL;
	pConfigs->pCfg_CAN = NULL;
	pConfigs->pCfg_I2C = NULL;
	pConfigs->pCfg_I2C_II = NULL;
	pConfigs->pCfg_I2C_CSI = NULL;
	pConfigs->pCfg_SPI = NULL;
	pConfigs->pCfg_USB = NULL;
	pConfigs->pCfg_m2_eKey = NULL;
	pConfigs->pCfg_m2_mKey_SATA = NULL;
	pConfigs->pCfg_m2_mKey_PCIE = NULL;
	pConfigs->pCfg_EEPROM = NULL;
	pConfigs->pCfg_Camera = NULL;
	pConfigs->pCfg_Temperature = NULL;

	return pConfigs;
}

TestResults* TestResultsInit(void){
	TestResults *pResults = malloc(sizeof(TestResults));
	pResults->pResult_LAN = NULL;
	pResults->pResult_WIFI = NULL;
	pResults->pResult_BT = NULL;
	pResults->pResult_SDCard = NULL;
	pResults->pResult_mSATA = NULL;
	pResults->pResult_mPCIE = NULL;
	pResults->pResult_RS232 = NULL;
	pResults->pResult_UART = NULL;
	pResults->pResult_UART_CSI = NULL;
	pResults->pResult_GPIO = NULL;
	pResults->pResult_FAN = NULL;
	pResults->pResult_eDP = NULL;
	pResults->pResult_CAN = NULL;
	pResults->pResult_I2C = NULL;
	pResults->pResult_I2C_II = NULL;
	pResults->pResult_I2C_CSI = NULL;
	pResults->pResult_SPI = NULL;
	pResults->pResult_USB_Micro = NULL;
	pResults->pResult_USB_Type_A = NULL;
	pResults->pResult_USB_Type_C = NULL;
	pResults->pResult_m2_eKey = NULL;
	pResults->pResult_m2_mKey_SATA = NULL;
	pResults->pResult_m2_mKey_PCIE = NULL;
	pResults->pResult_EEPROM = NULL;
	pResults->pResult_Camera = NULL;
	pResults->pResult_Temerature = NULL;
	return pResults;
}

void setFailedResults(TestResults* pResults, Items* failed_items){
	Items *pItem = failed_items;
	while (pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			pResults->pResult_LAN->nResult = FAILED;
			break;
		case WIFI:
			pResults->pResult_WIFI->nResult = FAILED;
			break;
		case BT:
			pResults->pResult_BT->nResult = FAILED;
			break;
		case SDCARD:
			pResults->pResult_SDCard->nResult = FAILED;
			break;
		case mSATA:
			pResults->pResult_mSATA->nResult = FAILED;
			break;
		case mPCIE:
			pResults->pResult_mPCIE->nResult = FAILED;
			break;
		case RS232:
			pResults->pResult_RS232->nResult = FAILED;
			break;
		case UART:
			pResults->pResult_UART->nResult = FAILED;
			break;
		case UART2:
			pResults->pResult_UART2->nResult = FAILED;
			break;
		case UART_CSI:
			pResults->pResult_UART_CSI->nResult = FAILED;
			break;
		case GPIO:
			pResults->pResult_GPIO->nResult = FAILED;
			break;
		case FAN:
			pResults->pResult_FAN->nResult = FAILED;
			break;
		case eDP:
			pResults->pResult_eDP->nResult = FAILED;
			break;			
		case CANbus:
			pResults->pResult_CAN->nResult = FAILED;
			break;
		case I2C:
			pResults->pResult_I2C->nResult = FAILED;
			break;
		case I2C_II:
			pResults->pResult_I2C_II->nResult = FAILED;
			break;
		case I2C_CSI:
			pResults->pResult_I2C_CSI->nResult = FAILED;
			break;			
		case SPI:
			pResults->pResult_SPI->nResult = FAILED;
			break;
		case USB_Micro:
			pResults->pResult_USB_Micro->nResult = FAILED;
			break;
		case USB_Type_A:
			pResults->pResult_USB_Type_A->nResult = FAILED;
			break;
		case USB_Type_C:
			pResults->pResult_USB_Type_C->nResult = FAILED;
			break;
		case m2_eKey:
			pResults->pResult_m2_eKey->nResult = FAILED;
			break;
		case m2_mKey_SATA:
			pResults->pResult_m2_mKey_SATA->nResult = FAILED;
			break;
		case m2_mKey_PCIE:
			pResults->pResult_m2_mKey_PCIE->nResult = FAILED;
			break;
		case EEPROM:
			pResults->pResult_EEPROM->nResult = FAILED;
			break;
		case Camera:
			pResults->pResult_Camera->nResult = FAILED;
			break;
		case Temperature:
			pResults->pResult_Temerature->nResult = FAILED;
			break;
		default:
			break;
		}
		pItem = pItem->next;
	}
}

TestReport* TestReportInit(void){
	TestReport *pTestReport = malloc(sizeof(TestReport));
	pTestReport->pGondanNumber = NULL;
	pTestReport->pBoardNumber = NULL;
	pTestReport->isOnlyBoard = false;
	return pTestReport;
}

char* appendContent(char *s, char *w){
	int slen=0, wlen=0;
	if (s != NULL)
		slen = strlen(s);

	wlen = strlen(w);
	s = realloc(s, (slen+wlen)*sizeof(char));
	for(int i=0;i<wlen;i++){
		s[i+slen] = w[i];
		s[i+slen+1] = '\0';
	}
	
#ifdef _DEBUG_
	printf("S:%s\n",s);
#endif
	return s;
}

void writeInfoSparateLine(FILE *pFile){
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	strcat(buf, "+");
	size_t minus_length = REPORT_CONTENT_LENGTH - strlen(buf) -1;
	for (int i=0;i<minus_length;i++){
		strcat(buf, "-");
	}
	strcat(buf, "+\n");
	fwrite(buf, sizeof(char), strlen(buf), pFile);
}

void writeItemSparateLine(FILE *pFile){
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	strcat(buf, "+");
	size_t minus_length = REPORT_CONTENT_LENGTH - strlen(buf) -1;
	for (int i=0;i<minus_length;i++){
		strcat(buf, "-");
	}
	strcat(buf, "+\n");
	fwrite(buf, sizeof(char), strlen(buf), pFile);
}

void writeReportHead(FILE *pFile){
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	writeInfoSparateLine(pFile);
	strcat(buf, "|                                  Test Report                                 |\n");
	fwrite(buf, sizeof(char), strlen(buf), pFile);
	writeInfoSparateLine(pFile);
	return;
}

void writeInfoContent(FILE *pFile, int content_num, ...){
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	char *pContent = NULL;
	va_list vl;
	va_start(vl, content_num);
	strcat(buf, "| ");
	if (content_num == 2){
		for(int i=0;i<content_num;i++){
			pContent = va_arg(vl, char*);
			strcat(buf, pContent);
			if (i == content_num-1){
				size_t space_length = REPORT_CONTENT_LENGTH - strlen(buf) -1;
				for (int i=0;i<space_length;i++){
					strcat(buf, " ");
				}
				strcat(buf, "|\n");
			}	
			else
				strcat(buf, " | ");
		}
	}
	else if (content_num == 4){
		for(int i=0;i<content_num;i++){
			pContent = va_arg(vl, char*);
			if (i == 1){
				strcat(buf, pContent);
				continue;
			}
			if (i == 2){
				size_t space_length = REPORT_CONTENT_LENGTH/2 - strlen(buf) -1;
				for (int i=0;i<space_length;i++){
					strcat(buf, " ");
				}
				strcat(buf, " | ");
			}

			strcat(buf, pContent);
			if (i == content_num-1){
				size_t space_length = REPORT_CONTENT_LENGTH - strlen(buf) -1;
				for (int i=0;i<space_length;i++){
					strcat(buf, " ");
				}
				strcat(buf, "|\n");
			}	
			else
				strcat(buf, " | ");
		}
	}
	fwrite(buf, sizeof(char), strlen(buf), pFile);
	writeInfoSparateLine(pFile);
	va_end(vl);
}

void writeItemContent(FILE *pFile, int content_num, ...){
	char buf[MAX_PATH];
	memset(buf, 0, sizeof(buf));
	char *pContent = NULL;
	va_list vl;
	va_start(vl, content_num);
	strcat(buf, "| ");
	if (content_num == 4){
		for(int i=0;i<content_num;i++){
			pContent = va_arg(vl, char*);
			if (i == 1){
				strcat(buf, pContent);
				continue;
			}
			if (i == 2){
				size_t space_length = REPORT_CONTENT_LENGTH/2 - strlen(buf) -15;
				for (int i=0;i<space_length;i++){
					strcat(buf, " ");
				}
				strcat(buf, " | ");
			}

			strcat(buf, pContent);
			if (i == content_num-1){
				size_t space_length = REPORT_CONTENT_LENGTH - strlen(buf) -1;
				for (int i=0;i<space_length;i++){
					strcat(buf, " ");
				}
				strcat(buf, "|\n");
			}	
			else
				strcat(buf, " | ");
		}
		fwrite(buf, sizeof(char), strlen(buf), pFile);
		writeItemSparateLine(pFile);
	}
	va_end(vl);
}

void writeResultContents(FILE *pFile, Items *items, TestConfigs *configs, TestResults *results){
	char key[10] = {0};
	char item_name[MAX_PATH];
	char szBuf[128];
	char szMsg[MAX_PATH];
	memset(item_name, 0, sizeof(item_name));
	memset(szBuf, 0, sizeof(szBuf));
	memset(szMsg, 0, sizeof(szMsg));
	int count = 1;
	int lan_count = 0;
	Items *pItem = items;
	
	while(pItem != NULL){
		if (count < 10)
			sprintf(key, " %d", count);
		else
			sprintf(key, "%d", count);
		
		switch (pItem->item_kind)
		{
			case LAN:
				lan_count = sizeof(*results->pResult_LAN)/sizeof(LAN_RESULT);
				for (int i=0; i<lan_count;i++){
					sprintf(item_name, "LAN-%d", i+1);
					writeItemContent(pFile, 4, key, item_name, results->pResult_LAN[i].nResult == OK ? "PASS  ":"FAILED", "");
				}
				break;
			case WIFI:
				writeItemContent(pFile, 4, key, "WIFI", results->pResult_WIFI->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case BT:
				writeItemContent(pFile, 4, key, "BlueTooth", results->pResult_BT->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case SDCARD:
				writeItemContent(pFile, 4, key, "Micro SD", results->pResult_SDCard->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case mSATA:
				writeItemContent(pFile, 4, key, "mSATA", results->pResult_mSATA->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case mPCIE:
				writeItemContent(pFile, 4, key, "mPCIE", results->pResult_mPCIE->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case RS232:
				writeItemContent(pFile, 4, key, "RS232", results->pResult_RS232->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case UART:
				writeItemContent(pFile, 4, key, "UART", results->pResult_UART->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case UART2:
				writeItemContent(pFile, 4, key, "UART2", results->pResult_UART2->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case UART_CSI:
				writeItemContent(pFile, 4, key, "UART CSI", results->pResult_UART_CSI->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case GPIO:
				if (results->pResult_GPIO->nResult == OK){
					writeItemContent(pFile, 4, key, "GPIO  ", "PASS  ", "");
				}
				else{
					strcat(szMsg, "Failed Pin:");
					for (int i=0;i<results->pResult_GPIO->nPinCount;i++){
						if (results->pResult_GPIO->nPinResults[i] == FAILED){
							sprintf(szBuf, "%d ", configs->pCfg_GPIO->pGPIO_Pins[i]);
							strcat(szMsg, szBuf);
						}
					}
					writeItemContent(pFile, 4, key, "GPIO", "FAILED", szMsg);
					memset(szBuf, 0, sizeof(szBuf));
					memset(szMsg, 0, sizeof(szMsg));
				}
				
				break;
			case FAN:
				writeItemContent(pFile, 4, key, "FAN", results->pResult_FAN->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case eDP:
				writeItemContent(pFile, 4, key, "eDP", results->pResult_eDP->nResult == OK ? "PASS  ":"FAILED", "");
				break;				
			case CANbus:
				writeItemContent(pFile, 4, key, "CAN Bus", results->pResult_CAN->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case I2C:
				writeItemContent(pFile, 4, key, "I2C", results->pResult_I2C->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case I2C_II:
				writeItemContent(pFile, 4, key, "I2C_II", results->pResult_I2C_II->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case I2C_CSI:
				writeItemContent(pFile, 4, key, "I2C CSI", results->pResult_I2C_CSI->nResult == OK ? "PASS  ":"FAILED", "");
				break;			
			case SPI:
				writeItemContent(pFile, 4, key, "SPI", results->pResult_SPI->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case USB_Micro:	
				writeItemContent(pFile, 4, key, "USB Micro", results->pResult_USB_Micro->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case USB_Type_A:
				if (results->pResult_USB_Type_A->nResult == OK){
					writeItemContent(pFile, 4, key, "USB Type-A", "PASS  ", "");
				}
				else{
					strcat(szMsg, "Not Found, ");
					if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_3_0_TYPE_A_UP_FOUND) || !(results->pResult_USB_Type_A->nUsbFoundDevice & USB_2_0_TYPE_A_UP_FOUND)){
						strcat(szMsg, "Up:");
						if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_3_0_TYPE_A_UP_FOUND)){
							strcat(szMsg, "3.0 ");
						}
						if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_2_0_TYPE_A_UP_FOUND)){
							strcat(szMsg, "2.0 ");
						}
					}
					if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_3_0_TYPE_A_DOWN_FOUND) || !(results->pResult_USB_Type_A->nUsbFoundDevice & USB_2_0_TYPE_A_DOWN_FOUND)){
						strcat(szMsg, "Down:");
						if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_3_0_TYPE_A_DOWN_FOUND)){
							strcat(szMsg, "3.0 ");
						}
						if (!(results->pResult_USB_Type_A->nUsbFoundDevice & USB_2_0_TYPE_A_DOWN_FOUND)){
							strcat(szMsg, "2.0 ");
						}
					}
					writeItemContent(pFile, 4, key, "USB Type-A", "FAILED", szMsg);
				}
				memset(szMsg, 0, sizeof(szMsg));
				break;
			case USB_Type_C:
				writeItemContent(pFile, 4, key, "USB Type-C", results->pResult_USB_Type_C->nResult == OK ? "PASS  ":"FAILED", "");
				break;

			case m2_eKey:
				writeItemContent(pFile, 4, key, "M.2 E-Key", results->pResult_m2_eKey->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case m2_bKey:
				writeItemContent(pFile, 4, key, "M.2 B-Key", results->pResult_m2_bKey->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case m2_mKey_SATA:
				writeItemContent(pFile, 4, key, "M.2 M-Key (SATA)", results->pResult_m2_mKey_SATA->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case m2_mKey_PCIE:
				writeItemContent(pFile, 4, key, "M.2 M-Key (PCIE)", results->pResult_m2_mKey_PCIE->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case EEPROM:
				writeItemContent(pFile, 4, key, "EEPROM", results->pResult_EEPROM->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			case Camera:
				if (results->pResult_Camera->nResult == OK){
					writeItemContent(pFile, 4, key, "Camera", "PASS  ", "");
				}
				else{
					strcat(szMsg, "Failed Camera ID:");
					for (int i=0;i<configs->pCfg_Camera->nCameraCount;i++){
						if (i != configs->pCfg_Camera->nCameraCount-1)
							sprintf(szBuf, "%d, ", i+1);
						else
							sprintf(szBuf, "%d", i+1);
						strcat(szMsg, szBuf);
					}
					writeItemContent(pFile, 4, key, "Camera", "FAILED", szMsg);
					memset(szMsg, 0, sizeof(szMsg));
				}
				
				break;
			case Temperature:
				writeItemContent(pFile, 4, key, "Temperature", results->pResult_Temerature->nResult == OK ? "PASS  ":"FAILED", "");
				break;
			default:
				break;			
		}
		pItem = pItem->next;
		count++;
	}
}

TestReport* generateReport(TestReport *report, DeviceInfo *device_info, Items *items, TestConfigs *configs, TestResults *results, bool haveFailed){
	FILE *pFile = NULL;
	size_t file_name_length = 0;
	char *file_name = NULL;
	char szCMD[MAX_PATH] = {0};

	if (report->isOnlyBoard){
		if (report->pBoardNumber == NULL){
			logWARN("Board Number is empty");
			return;
		}
		else{
			file_name_length = strlen(report->pBoardNumber) + strlen("/opt/aetina/bin/") + strlen(".txt");
			report->pFileName = malloc(file_name_length * sizeof(char));
			memset(report->pFileName, 0, file_name_length);
			strcat(report->pFileName, "/opt/aetina/bin/");
			strcat(report->pFileName, report->pBoardNumber);
			if (haveFailed)
				strcat(report->pFileName, "_Failed");
			strcat(report->pFileName, ".txt");	
		}
	}
	else{
		if (report->pBoardNumber == NULL){
			logWARN("Board Number is empty");
			if (device_info->pModule == NULL){
				logWARN("Module Name is empty");
			}
			return;
		}
		else{
			file_name_length = strlen(report->pGondanNumber) + strlen(report->pBoardNumber) + + strlen("/opt/aetina/bin/") + strlen("_") + strlen(".txt");
			report->pFileName = malloc(file_name_length * sizeof(char));
			memset(report->pFileName, 0, file_name_length);
			strcat(report->pFileName, "/opt/aetina/bin/");
			strcat(report->pFileName, report->pGondanNumber);
			strcat(report->pFileName, "_");
			strcat(report->pFileName, report->pBoardNumber);
			if (haveFailed)
				strcat(report->pFileName, "_Failed");
			strcat(report->pFileName, ".txt");
		}	
	}

#ifdef _DEBUG_
	printf("File Name:%s\n", report->pFileName);
#endif
	
	if (!haveFailed){
		pFile = fopen(report->pFileName, "r");
		if (pFile == NULL){
			pFile = fopen(report->pFileName, "a");
			writeReportHead(pFile);
			writeInfoContent(pFile, 2, "Part No.", report->pPartNumber);
			writeInfoContent(pFile, 2, "Board No.", report->pBoardNumber);
			if (!report->isOnlyBoard){
				writeInfoContent(pFile, 4, "Module", device_info->pModule, "S/N", device_info->pszSerialNumber);
			}
			writeInfoContent(pFile, 2, "BSP", device_info->pszBSP_Version);
			writeInfoContent(pFile, 2, "DTS", device_info->pszDTS_Name);
			if (device_info->nLanCount > 0){
				for(int i=0;i<device_info->nLanCount;i++){
					sprintf(szCMD, "MAC-%d", i+1);
					writeInfoContent(pFile, 2, szCMD, device_info->pszMAC_Address[i]);
				}
			}
			writeItemContent(pFile, 4, " #", "Item", "Result", "Description");
			writeResultContents(pFile, items, configs, results);
		}
		else{
			pFile = fopen(report->pFileName, "a+");
			writeResultContents(pFile, items, configs, results);
		}
	}
	else{
		pFile = fopen(report->pFileName, "w+");
		writeReportHead(pFile);
		writeInfoContent(pFile, 2, "Part No.", report->pPartNumber);
		writeInfoContent(pFile, 2, "Board No.", report->pBoardNumber);
		if (!report->isOnlyBoard){
			writeInfoContent(pFile, 4, "Module", device_info->pModule, "S/N", device_info->pszSerialNumber);
		}
		writeInfoContent(pFile, 2, "BSP", device_info->pszBSP_Version);
		writeInfoContent(pFile, 2, "DTS", device_info->pszDTS_Name);
		if (device_info->nLanCount > 0){
				for(int i=0;i<device_info->nLanCount;i++){
					sprintf(szCMD, "MAC-%d", i+1);
					writeInfoContent(pFile, 2, szCMD, device_info->pszMAC_Address[i]);
				}
			}
		writeItemContent(pFile, 4, " #", "Item", "Result", "Description");
		writeResultContents(pFile, items, configs, results);
	}
	
	fclose(pFile);
	return report;
}

void showReport(TestReport *report){
	char szCMD[MAX_CMD];
	memset(szCMD, 0, sizeof(szCMD));

	sprintf(szCMD, "gedit %s", report->pFileName);
	int nResult = system(szCMD);
	logINFO("\nPlease [Enter] to shutdown system");
}

void uploadReport(TestReport *report){
	char *ftp_ip = NULL;
	ftp_ip = checkConfigInfo("ftpip", ftp_ip);
	char ftp_account[MAX_CMD] = "Jetson";
	char ftp_password[MAX_CMD] = "Aetina111111";
	char tmp_path[MAX_PATH];
	char szCMD[MAX_CMD];
	char szResult[MAX_PATH];

	memset(szCMD, 0, sizeof(szCMD));
	memset(szResult, 0, sizeof(szResult));
	memset(tmp_path, 0, sizeof(tmp_path));

	sprintf(tmp_path, "/mnt/%s", report->pGondanNumber);
	sprintf(szCMD, "mkdir -p %s", tmp_path);
	system(szCMD);
	memset(szCMD, 0, sizeof(szCMD));
	sprintf(szCMD, "chmod 666 %s/", tmp_path);
	system(szCMD);
	
	sprintf(szCMD, "/opt/aetina/bin/curlftpfs %s -o user=%s:%s %s", ftp_ip, ftp_account, ftp_password, tmp_path);
	system(szCMD);
	memset(szCMD, 0, sizeof(szCMD));
	sprintf(szCMD, "ls %s/%s/", tmp_path, report->pGondanNumber);
	FILE *fp = popen(szCMD, "r");
	if (fgets(szResult, MAX_PATH, fp) != NULL){
		if (strstr(szResult, "directory")){
			logWARN("No such directory\n");
		}
		else{
			memset(szCMD, 0, sizeof(szCMD));
			sprintf(szCMD, "cp %s %s/%s/", report->pFileName, tmp_path, report->pGondanNumber);
			system(szCMD);
			sleep(1);
		}
	}
	else{
		memset(szCMD, 0, sizeof(szCMD));
		sprintf(szCMD, "cp %s %s/%s/", report->pFileName, tmp_path, report->pGondanNumber);
		system(szCMD);
		sleep(1);
	}
	
	memset(szCMD, 0, sizeof(szCMD));
	sprintf(szCMD, "umount %s/", tmp_path);
	system(szCMD);

	memset(szCMD, 0, sizeof(szCMD));
	sprintf(szCMD, "rmdir /mnt/%s/", report->pGondanNumber);
	system(szCMD);
}

void deleteReport(){
	char szCMD[MAX_CMD];
	memset(szCMD, 0, sizeof(szCMD));

	sprintf(szCMD, "rm /opt/aetina/bin/*.txt");
	int nResult = system(szCMD);
	return;
}

TestCore* TestCoreInit(void){
	TestCore* core = malloc(sizeof(TestCore));
	core->pTestItems = NULL;
	core->pTestConfigs = TestConfigsInit();
	core->pTestResults = TestResultsInit();
	core->pTestReport = TestReportInit();
	return core;
}

void showDebugMenu(TestCore *core){
	Items* pItem = core->pTestItems;
	int i = 0;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
			case LAN:
				printf("[%c] LAN\n", i+97);
				break;
			case WIFI:
				printf("[%c] WIFI\n", i+97);
				break;
			case BT:
				printf("[%c] BlueTooth\n", i+97);
				break;
			case SDCARD:
				printf("[%c] Micro SD\n", i+97);
				break;
			case mSATA:
				printf("[%c] mSATA\n", i+97);
				break;
			case mPCIE:
				printf("[%c] mPCIE\n", i+97);
				break;
			case RS232:
				printf("[%c] RS232\n", i+97);
				break;
			case UART:
				printf("[%c] UART\n", i+97);
				break;
			case UART2:
				printf("[%c] UART J518\n", i+97);
				break;	
			case UART_CSI:
				printf("[%c] UART CSI\n", i+97);
				break;
			case GPIO:
				printf("[%c] GPIO\n", i+97);
				break;
			case FAN:
				printf("[%c] FAN\n", i+97);
				break;
			case eDP:
				printf("[%c] eDP\n", i+97);
				break;				
			case CANbus:
				printf("[%c] CAN Bus\n", i+97);
				break;
			case I2C:
				printf("[%c] I2C\n", i+97);
				break;
			case I2C_II:
				printf("[%c] I2C II\n", i+97);
				break;
			case I2C_CSI:
				printf("[%c] I2C CSI\n", i+97);
				break;	
			case SPI:
				printf("[%c] SPI\n", i+97);
				break;
			case USB_Micro:	
				printf("[%c] USB Micro\n", i+97);
				break;
			case USB_Type_A:
				printf("[%c] USB Type-A\n", i+97);
				break;
			case USB_Type_C:
				printf("[%c] USB Type-C\n", i+97);
				break;
			case m2_eKey:
				printf("[%c] M.2 E-Key\n", i+97);
				break;
			case m2_bKey:
				printf("[%c] M.2 B-Key\n", i+97);
				break;
			case m2_mKey_SATA:
				printf("[%c] M.2 M-Key (SATA)\n", i+97);
				break;
			case m2_mKey_PCIE:
				printf("[%c] M.2 M-Key (PCIE)\n", i+97);
				break;
			case EEPROM:
				printf("[%c] EEPROM\n", i+97);
				break;
			case Camera:
				printf("[%c] Camera\n", i+97);
				break;
			case Temperature:
				printf("[%c] Temperature\n", i+97);
				break;
			default:
				break;			
		}
		pItem = pItem->next;
		i++;
	}

	printf("[z] Quit\n");
	return;
}

TestReport* scanGondamAndPartNumber(TestReport *report){
	report->pGondanNumber = malloc(40 * sizeof(char));
	report->pPartNumber  = malloc(40 * sizeof(char));
	memset(report->pGondanNumber, 0, sizeof(report->pGondanNumber));
	memset(report->pPartNumber, 0, sizeof(report->pPartNumber));
	bool switchPart = false;
	logINFO("Please Scan 工單號碼及品號");
	while(1){
		char c = getchar();
		if (c != '\n'){
			if (c == ' '){
				switchPart = true;
				continue;
			}
			if (!switchPart){
				size_t strlength = strlen(report->pGondanNumber);
				report->pGondanNumber[strlength] = c;
				report->pGondanNumber[strlength+1] = '\0';
			}
			else{
				size_t strlength = strlen(report->pPartNumber);
				report->pPartNumber[strlength] = c;
				report->pPartNumber[strlength+1] = '\0';
			}
		}
		else{
			if (strlen(report->pGondanNumber) != GONDAM_NUMBER_LENGTH){
				logINFO("Please Scan 工單號碼及品號");
				memset(report->pGondanNumber, 0, sizeof(report->pGondanNumber));
				memset(report->pPartNumber, 0, sizeof(report->pPartNumber));
				continue;
			}
			else
			{
				break;
			}
		}
	}

	return report;
}

TestReport* scanBoardNumber(TestReport* report){
	report->pBoardNumber = malloc(40 * sizeof(char));
	memset(report->pBoardNumber, 0, sizeof(report->pBoardNumber));
	logINFO("Please Scan 底板序號");
	while(1){
		char c = getchar();
		if (c != '\n'){
			size_t strlength = strlen(report->pBoardNumber);
			report->pBoardNumber[strlength] = c;
			report->pBoardNumber[strlength+1] = '\0';
		}
		else{
			if (strlen(report->pBoardNumber) != BOARD_NUMBER_LENGTH){
				logINFO("Please Scan 底板序號");
				memset(report->pBoardNumber, 0, sizeof(report->pBoardNumber));
			}
			else{
				break;
			}
		}
	}

	return report;
}

/* add autorun item */

void configureAutoRunItems(TestCore *core){
	int szValue = 0;
	checkConfigItem(items_name[TIMES], &szValue);
	core->mTestTimes = szValue;
	szValue = 0;

	if (checkConfigItem(items_name[LAN], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, LAN);
		szValue = 0;
	}

	if (checkConfigItem(items_name[SDCARD], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, SDCARD);
		szValue = 0;
	}

	if (checkConfigItem(items_name[RS232], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, RS232);
		szValue = 0;
	}

	if (checkConfigItem(items_name[UART], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, UART);
		szValue = 0;
	}

	if (checkConfigItem(items_name[USB_Type_A], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, USB_Type_A);
		szValue = 0;
	}

	if (checkConfigItem(items_name[GPIO], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, GPIO);
		szValue = 0;
	}

	if (checkConfigItem(items_name[FAN], &szValue) == OK && szValue < 0){
		core->pTestItems = addItem(core->pTestItems, FAN);
		szValue = 0;
	}

	char *dts = core->pDeviceInfo->pszDTS_Name;
	if (strstr(dts, "IMX334") || strstr(dts, "IMX290") /*|| strstr(dts, "N622") */|| strstr(dts, "IMX179")){
		if (checkConfigItem(items_name[Camera], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, Camera);
			szValue = 0;
		}

		/*if (strstr(dts, "six") && strstr(dts, "310")){
			if (checkConfigItem(items_name[UART_CSI], &szValue) == OK && szValue < 0){
				core->pTestItems = addItem(core->pTestItems, UART_CSI);
				szValue = 0;
			}
		}*/

		if (strstr(dts, "six") && strstr(dts, "720")){
			if (checkConfigItem(items_name[I2C_CSI], &szValue) == OK && szValue < 0){
				core->pTestItems = addItem(core->pTestItems, I2C_CSI);
				szValue = 0;
			}	
		}
	}

	char *module = core->pDeviceInfo->pModule;
	if (strstr(module, "TX2")){
		if (!strstr(module, "TX2_4GB") && !strstr(module, "TX2i")){
			if (checkConfigItem(items_name[WIFI], &szValue) == OK && szValue < 0){
				core->pTestItems = addItem(core->pTestItems, WIFI);
				szValue = 0;
			}

			if (checkConfigItem(items_name[BT], &szValue) == OK && szValue < 0){
				core->pTestItems = addItem(core->pTestItems, BT);
				szValue = 0;
			}
		}
	}


	char *board = core->pDeviceInfo->pszCarrierBoard;

	if (!strstr(board, "AN110") && !strstr(board, "AN810") && !strstr(board, "AT017")){
		if (checkConfigItem(items_name[CANbus], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, CANbus);
			szValue = 0;
		}
		if (checkConfigItem(items_name[USB_Micro], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, USB_Micro);
			szValue = 0;
		}			
	}

	if (strstr(board, "N310")){

		if (checkConfigItem(items_name[mSATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, mSATA);
			szValue = 0;
		}

		if (checkConfigItem(items_name[mPCIE], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, mPCIE);
			szValue = 0;
		}

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[I2C_CSI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C_CSI);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[Temperature], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, Temperature);
			szValue = 0;
		}
		if (checkConfigItem(items_name[UART_CSI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, UART_CSI);
			szValue = 0;
		}
	}
	if (strstr(board, "N510")){

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[Temperature], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, Temperature);
			szValue = 0;
		}
	}
	if (strstr(board, "N622")){
		if (checkConfigItem(items_name[USB_Micro], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, USB_Micro);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[mSATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, mSATA);
			szValue = 0;
		}

		if (checkConfigItem(items_name[mPCIE], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, mPCIE);
			szValue = 0;
		}

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[SPI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, SPI);
			szValue = 0;
		}
	}

	if (strstr(board, "AX710") || strstr(board, "AX720")){	
		if (checkConfigItem(items_name[SPI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, SPI);
			szValue = 0;
		}

		if (checkConfigItem(items_name[USB_Type_C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, USB_Type_C);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[m2_eKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_eKey);
			szValue = 0;
		}

		if (checkConfigItem(items_name[m2_eKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_eKey);
			szValue = 0;
		}

		if (checkConfigItem(items_name[m2_mKey_SATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[m2_mKey_PCIE], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}	
	}

	if (strstr(board, "T012")){	
		if (checkConfigItem(items_name[UART2], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, UART2);
			szValue = 0;
		}

		if (checkConfigItem(items_name[USB_Type_C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, USB_Type_C);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[m2_bKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_bKey);
			szValue = 0;
		}		

		if (checkConfigItem(items_name[m2_mKey_SATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[m2_mKey_PCIE], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}	
	}

	if (strstr(board, "AN110")){
		if (checkConfigItem(items_name[eDP], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, eDP);
			szValue = 0;
		}

		if (checkConfigItem(items_name[SPI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, SPI);
			szValue = 0;
		}

		if (checkConfigItem(items_name[m2_eKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_eKey);
			szValue = 0;
		}		

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}
	
	}

	if (strstr(board, "AN810")){
		if (checkConfigItem(items_name[CANbus], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, CANbus);
			szValue = 0;
		}

		if (checkConfigItem(items_name[SPI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, SPI);
			szValue = 0;
		}

		if (checkConfigItem(items_name[m2_eKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_eKey);
			szValue = 0;
		}

		if (checkConfigItem(items_name[m2_bKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_bKey);
			szValue = 0;
		}		

		if (checkConfigItem(items_name[m2_mKey_SATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[m2_mKey_PCIE], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
			szValue = 0;
		}	

		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}

		if (checkConfigItem(items_name[I2C_II], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C_II);
			szValue = 0;
		}

		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}
			
	}
	if (strstr(board, "AT017")){
		if (checkConfigItem(items_name[SPI], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, SPI);
			szValue = 0;
		}
		if (checkConfigItem(items_name[m2_eKey], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, m2_eKey);
			szValue = 0;
		}
		if (checkConfigItem(items_name[I2C], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, I2C);
			szValue = 0;
		}
		if (checkConfigItem(items_name[EEPROM], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, EEPROM);
			szValue = 0;
		}			
		if (checkConfigItem(items_name[mSATA], &szValue) == OK && szValue < 0){
			core->pTestItems = addItem(core->pTestItems, mSATA);
			szValue = 0;
		}
	}

	#ifdef _DEBUG_
		showAddedItems(core->pTestItems);
	#endif
		return;
}

void configureDebugItems(TestCore *core){
	//common test item
	core->pTestItems = addItem(core->pTestItems, LAN);
	core->pTestItems = addItem(core->pTestItems, SDCARD);
	core->pTestItems = addItem(core->pTestItems, RS232);
	core->pTestItems = addItem(core->pTestItems, UART);
	core->pTestItems = addItem(core->pTestItems, GPIO);
	core->pTestItems = addItem(core->pTestItems, USB_Type_A);
	core->pTestItems = addItem(core->pTestItems, FAN);
	char *module = core->pDeviceInfo->pModule;

	if (strstr(module, "TX2")){

		if (!strstr(module, "TX2_4GB") && !strstr(module, "TX2i")){
			core->pTestItems = addItem(core->pTestItems, WIFI);
			core->pTestItems = addItem(core->pTestItems, BT);
		}
	}

	char *dts = core->pDeviceInfo->pszDTS_Name;
	/*if (strstr(dts, "six") && strstr(dts, "310")){
		core->pTestItems = addItem(core->pTestItems, UART_CSI);
	}*/
	
	/*if (strstr(module, "Xavier_NX")){
		if (strstr(dts, "AN110")){
			core->pTestItems = addItem(core->pTestItems, FAN);
		}
		if (strstr(dts, "AT017")){
			
		}
	}*/


	char *board = core->pDeviceInfo->pszCarrierBoard;
	if (!strstr(board, "AN110") && !strstr(board, "AN810") && !strstr(board, "AT017")){
		core->pTestItems = addItem(core->pTestItems, CANbus);
		core->pTestItems = addItem(core->pTestItems, USB_Micro);
	}
		
	if (strstr(board, "N310")){
		core->pTestItems = addItem(core->pTestItems, mSATA);
		core->pTestItems = addItem(core->pTestItems, mPCIE);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);
		core->pTestItems = addItem(core->pTestItems, Temperature);
		core->pTestItems = addItem(core->pTestItems, I2C_CSI);
		core->pTestItems = addItem(core->pTestItems, UART_CSI);		
	}

	else if (strstr(board, "N510")){
		core->pTestItems = addItem(core->pTestItems, EEPROM);
		core->pTestItems = addItem(core->pTestItems, Temperature);
	}


	else if (strstr(board, "N622")){
		core->pTestItems = addItem(core->pTestItems, mSATA);
		core->pTestItems = addItem(core->pTestItems, mPCIE);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);
		core->pTestItems = addItem(core->pTestItems, SPI);
	}

	else if (strstr(board, "AX710") || strstr(board, "AX720")){
		core->pTestItems = addItem(core->pTestItems, SPI);
		core->pTestItems = addItem(core->pTestItems, USB_Type_C);
		core->pTestItems = addItem(core->pTestItems, m2_eKey);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);

	}

	else if (strstr(board, "T012")){
		core->pTestItems = addItem(core->pTestItems, UART2);
		core->pTestItems = addItem(core->pTestItems, USB_Type_C);
		core->pTestItems = addItem(core->pTestItems, m2_bKey);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);
	}

	else if (strstr(board, "AN110")){
		core->pTestItems = addItem(core->pTestItems, eDP);		
		core->pTestItems = addItem(core->pTestItems, SPI);
		core->pTestItems = addItem(core->pTestItems, m2_eKey);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);

	}
	else if (strstr(board, "AN810")){
		core->pTestItems = addItem(core->pTestItems, CANbus);
		core->pTestItems = addItem(core->pTestItems, SPI);
		core->pTestItems = addItem(core->pTestItems, m2_eKey);
		core->pTestItems = addItem(core->pTestItems, m2_bKey);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_PCIE);
		core->pTestItems = addItem(core->pTestItems, m2_mKey_SATA);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, I2C_II);	
		core->pTestItems = addItem(core->pTestItems, EEPROM);

	
	}
	else if (strstr(board, "AT017")){
		core->pTestItems = addItem(core->pTestItems, SPI);
		core->pTestItems = addItem(core->pTestItems, m2_eKey);
		core->pTestItems = addItem(core->pTestItems, I2C);
		core->pTestItems = addItem(core->pTestItems, EEPROM);
		core->pTestItems = addItem(core->pTestItems, mSATA);
	}


	else {
		printf("Please add board and test items.\n");
		exit(0);
	}

	if (strstr(dts, "IMX334") || strstr(dts, "IMX290")/* || strstr(dts, "N622") */|| strstr(dts,"IMX179")){
		core->pTestItems = addItem(core->pTestItems, Camera);
		if (strstr(dts, "six") && strstr(dts, "720")){
			core->pTestItems = addItem(core->pTestItems, I2C_CSI);
		}
	}

	return;
}

void waitForShutdown(){
	while(1){
		char c = getchar();
		if (c){
			system("shutdown now");
		}
	}
}

void autoRunTests(TestCore *core){
	bool bHaveFailed = false;
	Items *passed_items = NULL;
	Items *failed_items = NULL;
	core->mTestTimes++;

	if (findItem(core->pTestItems, LAN)){
		bool bLAN_Allpass = true;
		for (int i=0;i<core->pTestConfigs->mLAN_Count;i++){
			if (doTest_LAN(&core->pTestConfigs->pCfg_LAN[i], &core->pTestResults->pResult_LAN[i]) != OK){
				bLAN_Allpass = false;
			}
		}
		if (bLAN_Allpass){
			setConfigItem(items_name[LAN], OK);
			passed_items = addItem(passed_items, LAN);
		}
		else{
			failed_items = addItem(failed_items, LAN);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, WIFI)){
		if (doTest_WIFI(core->pTestResults->pResult_WIFI) == OK){
			setConfigItem(items_name[WIFI], OK);
			passed_items = addItem(passed_items, WIFI);
		}	
		else{
			failed_items = addItem(failed_items, WIFI);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, BT)){
		if (doTest_BT(core->pTestResults->pResult_BT) == OK){
			setConfigItem(items_name[BT], OK);
			passed_items = addItem(passed_items, BT);
		}
		else{
			failed_items = addItem(failed_items, BT);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, SDCARD)){
		if (doTest_SDCARD(core->pTestConfigs->pCfg_SDCard, core->pTestResults->pResult_SDCard) == OK){
			setConfigItem(items_name[SDCARD], OK);
			passed_items = addItem(passed_items, SDCARD);
		}
		else{
			failed_items = addItem(failed_items, SDCARD);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, mSATA)){
		if (doTest_mSATA(core->pTestConfigs->pCfg_mSATA, core->pTestResults->pResult_mSATA) == OK){
			setConfigItem(items_name[mSATA], OK);
			passed_items = addItem(passed_items, mSATA);
		}
		else{
			failed_items = addItem(failed_items, mSATA);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, mPCIE)){
		if (doTest_mPCIE(core->pTestConfigs->pCfg_mPCIE, core->pTestResults->pResult_mPCIE) == OK){
			setConfigItem(items_name[mPCIE], OK);
			passed_items = addItem(passed_items, mPCIE);
		}
		else{
			failed_items = addItem(failed_items, mPCIE);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, RS232)){
		if (doTest_RS232(core->pTestConfigs->pCfg_RS232, core->pTestResults->pResult_RS232) == OK){
			setConfigItem(items_name[RS232], OK);
			passed_items = addItem(passed_items, RS232);
		}
		else{
			failed_items = addItem(failed_items, RS232);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, UART)){
		if (doTest_UART(core->pTestConfigs->pCfg_UART, core->pTestResults->pResult_UART) == OK){
			setConfigItem(items_name[UART], OK);
			passed_items = addItem(passed_items, UART);
		}
		else{
			failed_items = addItem(failed_items, UART);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, UART2)){
		if (doTest_UART2(core->pTestConfigs->pCfg_UART2, core->pTestResults->pResult_UART2) == OK){
			setConfigItem(items_name[UART2], OK);
			passed_items = addItem(passed_items, UART2);
		}
		else{
			failed_items = addItem(failed_items, UART2);
			bHaveFailed = true;
		}
	}


	if (findItem(core->pTestItems, UART_CSI)){
		if (doTest_UART_CSI(core->pTestConfigs->pCfg_UART_CSI, core->pTestResults->pResult_UART_CSI) == OK){
			setConfigItem(items_name[UART_CSI], OK);
			passed_items = addItem(passed_items, UART_CSI);
		}
		else{
			failed_items = addItem(failed_items, UART_CSI);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, GPIO)){
		if (doTest_GPIO(core->pTestConfigs->pCfg_GPIO, core->pTestResults->pResult_GPIO) == OK){
			setConfigItem(items_name[GPIO], OK);
			passed_items = addItem(passed_items, GPIO);
		}
		else{
			failed_items = addItem(failed_items, GPIO);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, FAN)){
		if (doTest_FAN(core->pTestConfigs->pCfg_FAN, core->pTestResults->pResult_FAN) == OK){
			setConfigItem(items_name[FAN], OK);
			passed_items = addItem(passed_items, FAN);
		}
		else{
			failed_items = addItem(failed_items, FAN);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, eDP)){
		if (doTest_eDP(core->pTestConfigs->pCfg_eDP, core->pTestResults->pResult_eDP) == OK){
			setConfigItem(items_name[eDP], OK);
			passed_items = addItem(passed_items, eDP);
		}
		else{
			failed_items = addItem(failed_items, eDP);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, CANbus)){
		if (doTest_CANbus(core->pTestConfigs->pCfg_CAN, core->pTestResults->pResult_CAN) == OK){
			setConfigItem(items_name[CANbus], OK);
			passed_items = addItem(passed_items, CANbus);
		}
		else{
			failed_items = addItem(failed_items, CANbus);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, I2C)){
		if (doTest_I2C(core->pTestConfigs->pCfg_I2C, core->pTestResults->pResult_I2C) == OK){
			setConfigItem(items_name[I2C], OK);
			passed_items = addItem(passed_items, I2C);
		}
		else{
			failed_items = addItem(failed_items, I2C);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, I2C_II)){
		if (doTest_I2C_II(core->pTestConfigs->pCfg_I2C_II, core->pTestResults->pResult_I2C_II) == OK){
			setConfigItem(items_name[I2C_II], OK);
			passed_items = addItem(passed_items, I2C_II);
		}
		else{
			failed_items = addItem(failed_items, I2C_II);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, I2C_CSI)){
		if (doTest_I2C_CSI(core->pTestConfigs->pCfg_I2C_CSI, core->pTestResults->pResult_I2C_CSI) == OK){
			setConfigItem(items_name[I2C_CSI], OK);
			passed_items = addItem(passed_items, I2C_CSI);
		}
		else{
			failed_items = addItem(failed_items, I2C_CSI);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, SPI)){
		if (doTest_SPI(core->pTestConfigs->pCfg_SPI, core->pTestResults->pResult_SPI) == OK){
			setConfigItem(items_name[SPI], OK);
			passed_items = addItem(passed_items, SPI);
		}
		else{
			failed_items = addItem(failed_items, SPI);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, USB_Micro)){
		if (doTest_USB_Micro(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Micro) == OK){
			setConfigItem(items_name[USB_Micro], OK);
			passed_items = addItem(passed_items, USB_Micro);
		}
		else{
			failed_items = addItem(failed_items, USB_Micro);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, USB_Type_A)){
		if (doTest_USB_Type_A(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Type_A) == OK){
			setConfigItem(items_name[USB_Type_A], OK);
			passed_items = addItem(passed_items, USB_Type_A);
		}
		else{
			failed_items = addItem(failed_items, USB_Type_A);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, USB_Type_C)){
		if (doTest_USB_Type_C(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Type_C) == OK){
			setConfigItem(items_name[USB_Type_C], OK);
			passed_items = addItem(passed_items, USB_Type_C);
		}
		else{
			failed_items = addItem(failed_items, USB_Type_C);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, m2_eKey)){
		if (doTest_m2_eKey(core->pTestConfigs->pCfg_m2_eKey, core->pTestResults->pResult_m2_eKey) == OK){
			setConfigItem(items_name[m2_eKey], OK);
			passed_items = addItem(passed_items, m2_eKey);
		}else{
			failed_items = addItem(failed_items, m2_eKey);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, m2_bKey)){
		if (doTest_m2_bKey(core->pTestConfigs->pCfg_m2_bKey, core->pTestResults->pResult_m2_bKey) == OK){
			setConfigItem(items_name[m2_bKey], OK);
			passed_items = addItem(passed_items, m2_bKey);
		}else{
			failed_items = addItem(failed_items, m2_bKey);
			bHaveFailed = true;
		}
	}	

	if (findItem(core->pTestItems, m2_mKey_SATA)){
		if (doTest_m2_mKey_SATA(core->pTestConfigs->pCfg_m2_mKey_SATA, core->pTestResults->pResult_m2_mKey_SATA) == OK){
			setConfigItem(items_name[m2_mKey_SATA], OK);
			passed_items = addItem(passed_items, m2_mKey_SATA);
		}
		else{
			failed_items = addItem(failed_items, m2_mKey_SATA);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, m2_mKey_PCIE)){
		if (doTest_m2_mKey_PCIE(core->pTestConfigs->pCfg_m2_mKey_PCIE, core->pTestResults->pResult_m2_mKey_PCIE) == OK){
			setConfigItem(items_name[m2_mKey_PCIE], OK);
			passed_items = addItem(passed_items, m2_mKey_PCIE);
		}
		else{
			failed_items = addItem(failed_items, m2_mKey_PCIE);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, EEPROM)){
		if (doTest_EEPROM(core->pTestConfigs->pCfg_EEPROM, core->pTestResults->pResult_EEPROM) == OK){
			setConfigItem(items_name[EEPROM], OK);
			passed_items = addItem(passed_items, EEPROM);
		}
		else{
			failed_items = addItem(failed_items, EEPROM);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, Camera)){
		if (doTest_Camera(core->pTestConfigs->pCfg_Camera, core->pTestResults->pResult_Camera) == OK){
			setConfigItem(items_name[Camera], OK);
			passed_items = addItem(passed_items, Camera);
		}
		else{
			failed_items = addItem(failed_items, Camera);
			bHaveFailed = true;
		}
	}

	if (findItem(core->pTestItems, Temperature)){
		if (doTest_Temperature(core->pTestConfigs->pCfg_Temperature, core->pTestResults->pResult_Temerature) == OK){
			setConfigItem(items_name[Temperature], OK);
			passed_items = addItem(passed_items, Temperature);
		}
		else{
			failed_items = addItem(failed_items, Temperature);
			bHaveFailed = true;
		}
	}

	if (bHaveFailed){
		if (core->mTestTimes >= RESET_TIMES){
			setFailedResults(core->pTestResults, failed_items);
			core->pTestReport = generateReport(core->pTestReport, core->pDeviceInfo, failed_items, core->pTestConfigs, core->pTestResults, true);
			showReport(core->pTestReport);
			resetConfigItem();
			resetConfigInfo();
			deleteReport();
		}
		else{
			generateReport(core->pTestReport, core->pDeviceInfo, passed_items, core->pTestConfigs, core->pTestResults, false);
			setConfigInfo("gondannumber", core->pTestReport->pGondanNumber);
			setConfigInfo("partnumber", core->pTestReport->pPartNumber);
			setConfigInfo("boardnumber", core->pTestReport->pBoardNumber);
			setConfigItem("TIMES", core->mTestTimes);
			showNonFinishItems(failed_items);
		}
	}
	else{
		core->pTestReport = generateReport(core->pTestReport, core->pDeviceInfo, core->pTestItems, core->pTestConfigs, core->pTestResults, false);
		uploadReport(core->pTestReport);
		showReport(core->pTestReport);
		resetConfigItem();
		resetConfigInfo();
		deleteReport();
	}
	waitForShutdown();
}

void runTest(TestCore *core, char index){
	Items *targetItem = core->pTestItems;
	int item_index = index - 'a';
	for (int i=0;i<item_index;i++){
		targetItem = targetItem->next;
	}

	switch (targetItem->item_kind)
	{
	case LAN:
		for (int i=0;i<core->pTestConfigs->mLAN_Count;i++){
			if (doTest_LAN(&core->pTestConfigs->pCfg_LAN[i], &core->pTestResults->pResult_LAN[i]) == OK){
			}
		}
		break;
	case WIFI:
		doTest_WIFI(core->pTestResults->pResult_WIFI);
		break;
	case BT:
		doTest_BT(core->pTestResults->pResult_BT);
		break;
	case SDCARD:
		doTest_SDCARD(core->pTestConfigs->pCfg_SDCard, core->pTestResults->pResult_SDCard);
		break;
	case mSATA:
		doTest_mSATA(core->pTestConfigs->pCfg_mSATA, core->pTestResults->pResult_mSATA);
		break;
	case mPCIE:
		doTest_mPCIE(core->pTestConfigs->pCfg_mPCIE, core->pTestResults->pResult_mPCIE);
		break;
	case RS232:
		doTest_RS232(core->pTestConfigs->pCfg_RS232, core->pTestResults->pResult_RS232);
		break;
	case UART:
		doTest_UART(core->pTestConfigs->pCfg_UART, core->pTestResults->pResult_UART);
		break;
	case UART2:
		doTest_UART2(core->pTestConfigs->pCfg_UART2, core->pTestResults->pResult_UART2);
		break;
	case UART_CSI:
		doTest_UART_CSI(core->pTestConfigs->pCfg_UART_CSI, core->pTestResults->pResult_UART_CSI);
		break;
	case GPIO:
		doTest_GPIO(core->pTestConfigs->pCfg_GPIO, core->pTestResults->pResult_GPIO);
		break;
	case FAN:
		doTest_FAN(core->pTestConfigs->pCfg_FAN, core->pTestResults->pResult_FAN);
		break;
	case eDP:
		doTest_eDP(core->pTestConfigs->pCfg_eDP, core->pTestResults->pResult_eDP);
		break;		
	case CANbus:
		doTest_CANbus(core->pTestConfigs->pCfg_CAN, core->pTestResults->pResult_CAN);
		break;
	case I2C:
		doTest_I2C(core->pTestConfigs->pCfg_I2C, core->pTestResults->pResult_I2C);
		break;
	case I2C_II:
		doTest_I2C_II(core->pTestConfigs->pCfg_I2C_II, core->pTestResults->pResult_I2C_II);
		break;
	case I2C_CSI:
		doTest_I2C_CSI(core->pTestConfigs->pCfg_I2C_CSI, core->pTestResults->pResult_I2C_CSI);
		break;		
	case SPI:
		doTest_SPI(core->pTestConfigs->pCfg_SPI, core->pTestResults->pResult_SPI);
		break;
	case USB_Micro:
		doTest_USB_Micro(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Micro);
		break;
	case USB_Type_A:
		doTest_USB_Type_A(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Type_A);
		break;
	case USB_Type_C:
		doTest_USB_Type_C(core->pTestConfigs->pCfg_USB, core->pTestResults->pResult_USB_Type_C);
		break;
	case m2_eKey:
		doTest_m2_eKey(core->pTestConfigs->pCfg_m2_eKey, core->pTestResults->pResult_m2_eKey);
		break;
	case m2_bKey:
		doTest_m2_bKey(core->pTestConfigs->pCfg_m2_bKey, core->pTestResults->pResult_m2_bKey);
		break;
	case m2_mKey_SATA:
		doTest_m2_mKey_SATA(core->pTestConfigs->pCfg_m2_mKey_SATA, core->pTestResults->pResult_m2_mKey_PCIE);
		break;
	case m2_mKey_PCIE:
		doTest_m2_mKey_PCIE(core->pTestConfigs->pCfg_m2_mKey_PCIE, core->pTestResults->pResult_m2_mKey_PCIE);
		break;
	case EEPROM:
		doTest_EEPROM(core->pTestConfigs->pCfg_EEPROM, core->pTestResults->pResult_EEPROM);
		break;
	case Camera:
		doTest_Camera(core->pTestConfigs->pCfg_Camera, core->pTestResults->pResult_Camera);
		break;
	case Temperature:
		doTest_Temperature(core->pTestConfigs->pCfg_Temperature, core->pTestResults->pResult_Temerature);
		break;
	default:
		break;
	}
	return;
}

void setupCfgAndRst(Items* items, TestConfigs* configs, TestResults* results){
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
		#ifdef _DEBUG_
			printf("setup LAN\n");
		#endif
			configs->pCfg_LAN = malloc(sizeof(LAN_CFG));
			memset(configs->pCfg_LAN, 0, sizeof(LAN_CFG));
			results->pResult_LAN = malloc(sizeof(LAN_RESULT));
			memset(results->pResult_LAN, 0, sizeof(LAN_RESULT));
			break;
		case WIFI:
		#ifdef _DEBUG_
			printf("setup WIFI\n");
		#endif
			results->pResult_WIFI = malloc(sizeof(WIFI_RESULT));
			memset(results->pResult_WIFI, 0, sizeof(WIFI_RESULT));
			break;
		case BT:
		#ifdef _DEBUG_
			printf("setup BT\n");
		#endif
			results->pResult_BT = malloc(sizeof(BT_RESULT));
			memset(results->pResult_BT, 0, sizeof(BT_RESULT));
			break;
		case SDCARD:
		#ifdef _DEBUG_
			printf("setup SDCARD\n");
		#endif
			configs->pCfg_SDCard = malloc(sizeof(SDCARD_CFG));
			memset(configs->pCfg_SDCard, 0, sizeof(SDCARD_CFG));
			results->pResult_SDCard = malloc(sizeof(SDCARD_RESULT));
			memset(results->pResult_SDCard, 0, sizeof(SDCARD_RESULT));
			break;
		case mSATA:
		#ifdef _DEBUG_
			printf("setup mSATA\n");
		#endif
			configs->pCfg_mSATA = malloc(sizeof(MSATA_CFG));
			memset(configs->pCfg_mSATA, 0, sizeof(MSATA_CFG));
			results->pResult_mSATA = malloc(sizeof(MSATA_RESULT));
			memset(results->pResult_mSATA, 0, sizeof(MSATA_RESULT));
			break;
		case mPCIE:
		#ifdef _DEBUG_
			printf("setup mPCIE\n");
		#endif
			configs->pCfg_mPCIE = malloc(sizeof(MPCIE_CFG));
			memset(configs->pCfg_mPCIE, 0, sizeof(MPCIE_CFG));
			results->pResult_mPCIE = malloc(sizeof(MPCIE_RESULT));
			memset(results->pResult_mPCIE, 0, sizeof(MPCIE_RESULT));
			break;
		case RS232:	
		#ifdef _DEBUG_
			printf("setup RS232\n");
		#endif
			configs->pCfg_RS232 = malloc(sizeof(RS232_CFG));
			memset(configs->pCfg_RS232, 0, sizeof(RS232_CFG));
			results->pResult_RS232 = malloc(sizeof(RS232_RESULT));
			memset(results->pResult_RS232, 0, sizeof(RS232_RESULT));
			break;
		case UART:
		#ifdef _DEBUG_
			printf("setup UART\n");
		#endif
			configs->pCfg_UART = malloc(sizeof(UART_CFG));
			memset(configs->pCfg_UART, 0, sizeof(UART_CFG));
			results->pResult_UART = malloc(sizeof(UART_RESULT));
			memset(results->pResult_UART, 0, sizeof(UART_RESULT));
			break;
		case UART2:
		#ifdef _DEBUG_
			printf("setup UART2\n");
		#endif
			configs->pCfg_UART2 = malloc(sizeof(UART2_CFG));
			memset(configs->pCfg_UART2, 0, sizeof(UART2_CFG));
			results->pResult_UART2 = malloc(sizeof(UART2_RESULT));
			memset(results->pResult_UART2, 0, sizeof(UART2_RESULT));
			break;
		case UART_CSI:
		#ifdef _DEBUG_
			printf("setup UART CSI\n");
		#endif
			configs->pCfg_UART_CSI = malloc(sizeof(UART_CSI_CFG));
			memset(configs->pCfg_UART_CSI, 0, sizeof(UART_CSI_CFG));
			results->pResult_UART_CSI = malloc(sizeof(UART_CSI_RESULT));
			memset(results->pResult_UART_CSI, 0, sizeof(UART_CSI_RESULT));
			break;
		case GPIO:
		#ifdef _DEBUG_
			printf("setup GPIO\n");
		#endif
			configs->pCfg_GPIO = malloc(sizeof(GPIO_CFG));
			memset(configs->pCfg_GPIO, 0, sizeof(GPIO_CFG));
			results->pResult_GPIO = malloc(sizeof(GPIO_RESULT));
			memset(results->pResult_GPIO, 0, sizeof(GPIO_RESULT));
			break;
		case FAN:
		#ifdef _DEBUG_
			printf("setup FAN\n");
		#endif
			configs->pCfg_FAN = malloc(sizeof(FAN_CFG));
			memset(configs->pCfg_FAN, 0, sizeof(FAN_CFG));
			results->pResult_FAN = malloc(sizeof(FAN_RESULT));
			memset(results->pResult_FAN, 0, sizeof(FAN_RESULT));
			break;
		case eDP:
		#ifdef _DEBUG_
			printf("setup eDP\n");
		#endif
			configs->pCfg_eDP = malloc(sizeof(EDP_CFG));
			memset(configs->pCfg_eDP, 0, sizeof(EDP_CFG));
			results->pResult_eDP = malloc(sizeof(EDP_RESULT));
			memset(results->pResult_eDP, 0, sizeof(EDP_RESULT));
			break;			
		case CANbus:
		#ifdef _DEBUG_
			printf("setup CAN Bus\n");
		#endif
			configs->pCfg_CAN = malloc(sizeof(CAN_CFG));
			memset(configs->pCfg_CAN, 0, sizeof(CAN_CFG));
			results->pResult_CAN = malloc(sizeof(CAN_RESULT));
			memset(results->pResult_CAN, 0, sizeof(CAN_RESULT));
			break;
		case I2C:
		#ifdef _DEBUG_
			printf("setup I2C\n");
		#endif
			configs->pCfg_I2C = malloc(sizeof(I2C_CFG));
			memset(configs->pCfg_I2C, 0, sizeof(I2C_CFG));
			results->pResult_I2C = malloc(sizeof(I2C_RESULT));
			memset(results->pResult_I2C, 0, sizeof(I2C_RESULT));
			break;
		case I2C_II:
		#ifdef _DEBUG_
			printf("setup I2C_II\n");
		#endif
			configs->pCfg_I2C_II = malloc(sizeof(I2C_II_CFG));
			memset(configs->pCfg_I2C_II, 0, sizeof(I2C_II_CFG));
			results->pResult_I2C_II = malloc(sizeof(I2C_II_RESULT));
			memset(results->pResult_I2C_II, 0, sizeof(I2C_II_RESULT));
			break;
		case I2C_CSI:
		#ifdef _DEBUG_
			printf("setup I2C CSI\n");
		#endif
			configs->pCfg_I2C_CSI = malloc(sizeof(I2C_CSI_CFG));
			memset(configs->pCfg_I2C_CSI, 0, sizeof(I2C_CSI_CFG));
			results->pResult_I2C_CSI = malloc(sizeof(I2C_CSI_RESULT));
			memset(results->pResult_I2C_CSI, 0, sizeof(I2C_CSI_RESULT));
			break;			
		case SPI:
		#ifdef _DEBUG_
			printf("setup SPI\n");
		#endif
			configs->pCfg_SPI = malloc(sizeof(SPI_CFG));
			memset(configs->pCfg_SPI, 0, sizeof(SPI_CFG));
			results->pResult_SPI = malloc(sizeof(SPI_RESULT));
			memset(results->pResult_SPI, 0, sizeof(SPI_RESULT));
			break;
		case USB_Micro:
		#ifdef _DEBUG_
			printf("setup USB Micro\n");
		#endif
			configs->pCfg_USB = malloc(sizeof(USB_CFG));
			memset(configs->pCfg_USB, 0, sizeof(USB_CFG));
			results->pResult_USB_Micro = malloc(sizeof(USB_RESULT));
			memset(results->pResult_USB_Micro, 0, sizeof(USB_RESULT));
			break;
		case USB_Type_A:
		#ifdef _DEBUG_
			printf("setup USB Type-A\n");
		#endif
			configs->pCfg_USB = malloc(sizeof(USB_CFG));
			memset(configs->pCfg_USB, 0, sizeof(USB_CFG));
			results->pResult_USB_Type_A = malloc(sizeof(USB_RESULT));
			memset(results->pResult_USB_Type_A, 0, sizeof(USB_RESULT));
			break;
		case USB_Type_C:
		#ifdef _DEBUG_
			printf("setup USB Type-C\n");
		#endif
			configs->pCfg_USB = malloc(sizeof(USB_CFG));
			memset(configs->pCfg_USB, 0, sizeof(USB_CFG));
			results->pResult_USB_Type_C = malloc(sizeof(USB_RESULT));
			memset(results->pResult_USB_Type_C, 0, sizeof(USB_RESULT));
			break;
		case m2_eKey:
		#ifdef _DEBUG_
			printf("setup M.2 E-Key\n");
		#endif
			configs->pCfg_m2_eKey = malloc(sizeof(M2_EKEY_CFG));
			memset(configs->pCfg_m2_eKey, 0, sizeof(M2_EKEY_CFG));
			results->pResult_m2_eKey = malloc(sizeof(M2_EKEY_RESULT));
			memset(results->pResult_m2_eKey, 0, sizeof(M2_EKEY_RESULT));
			break;
		case m2_bKey:
		#ifdef _DEBUG_
			printf("setup M.2 B-Key\n");
		#endif
			configs->pCfg_m2_bKey = malloc(sizeof(M2_BKEY_CFG));
			memset(configs->pCfg_m2_bKey, 0, sizeof(M2_BKEY_CFG));
			results->pResult_m2_bKey = malloc(sizeof(M2_BKEY_RESULT));
			memset(results->pResult_m2_bKey, 0, sizeof(M2_BKEY_RESULT));
			break;
		case m2_mKey_SATA:
		#ifdef _DEBUG_
			printf("setup M.2 M-Key SATA\n");
		#endif
			configs->pCfg_m2_mKey_SATA = malloc(sizeof(M2_MKEY_CFG));
			memset(configs->pCfg_m2_mKey_SATA, 0, sizeof(M2_MKEY_CFG));
			results->pResult_m2_mKey_SATA = malloc(sizeof(M2_MKEY_RESULT));
			memset(results->pResult_m2_mKey_SATA, 0, sizeof(M2_MKEY_RESULT));
			break;
		case m2_mKey_PCIE:
		#ifdef _DEBUG_
			printf("setup M.2 M-Key PCIE\n");
		#endif
			configs->pCfg_m2_mKey_PCIE = malloc(sizeof(M2_MKEY_CFG));
			memset(configs->pCfg_m2_mKey_PCIE, 0, sizeof(M2_MKEY_CFG));
			results->pResult_m2_mKey_PCIE = malloc(sizeof(M2_MKEY_RESULT));
			memset(results->pResult_m2_mKey_PCIE, 0, sizeof(M2_MKEY_RESULT));
			break;
		case EEPROM:
		#ifdef _DEBUG_
			printf("setup EEPROM\n");
		#endif
			configs->pCfg_EEPROM = malloc(sizeof(EEPROM_CFG));
			memset(configs->pCfg_EEPROM, 0, sizeof(EEPROM_CFG));
			results->pResult_EEPROM = malloc(sizeof(EEPROM_RESULT));
			memset(results->pResult_EEPROM, 0, sizeof(EEPROM_RESULT));
			break;
		case Camera:
		#ifdef _DEBUG_
			printf("setup Camera\n");
		#endif
			configs->pCfg_Camera = malloc(sizeof(CAMERA_CFG));
			memset(configs->pCfg_Camera, 0, sizeof(CAMERA_CFG));
			results->pResult_Camera = malloc(sizeof(CAMERA_RESULT));
			memset(results->pResult_Camera, 0, sizeof(CAMERA_RESULT));
			break;
		case Temperature:
		#ifdef _DEBUG_
			printf("setup Temperature\n");
		#endif
			configs->pCfg_Temperature = malloc(sizeof(TEMPERATURE_CFG));
			memset(configs->pCfg_Temperature, 0, sizeof(TEMPERATURE_CFG));
			results->pResult_Temerature = malloc(sizeof(TEMPERATURE_RESULT));
			memset(results->pResult_Temerature, 0, sizeof(TEMPERATURE_RESULT));
			break;		
		default:
			break;
		}
		pItem = pItem->next;
	}
}

void setCfgNRst_TX2(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){

	printf("Setting up Config and Result on TX2.\n\n");
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS2";
	char uart_dev_path[] = "/dev/ttyS0";
	char uart_csi_dev_path[] = "/dev/ttyTHS1";
	char serial_test_data[] = "Aetina";
	char spi_dev_path[] = "/dev/spidev3.0";
	int gpio[] = {488, 388, 389, 481, 397};
	char canbus_sender[] = "can1";
	char canbus_reciver[] = "can0";
	char canbus_id[10] = "123";
	char canbus_data[] = "abcdabcd";
  	char usb_3_type_a_up_usb3_dev[10];
  	char usb_3_type_a_up_usb2_dev[10];
	char usb_3_type_a_down_usb3_dev[10];
	char usb_3_type_a_donw_usb2_dev[10];
  	char usb_3_micro_dev[10];
	char eeprom_data[] = "Aetina Corporation";
	memset(usb_3_type_a_up_usb3_dev, 0, sizeof(usb_3_type_a_up_usb3_dev));
	memset(usb_3_type_a_up_usb2_dev, 0, sizeof(usb_3_type_a_up_usb2_dev));
	memset(usb_3_type_a_down_usb3_dev, 0, sizeof(usb_3_type_a_down_usb3_dev));
	memset(usb_3_type_a_donw_usb2_dev, 0, sizeof(usb_3_type_a_donw_usb2_dev));
	memset(usb_3_micro_dev, 0, sizeof(usb_3_micro_dev));

	if (folio == AN310 || folio == AN310_IMX290 || folio == AN310_IMX290_six || folio == AN310_IMX290_ISP || folio == AN310_IMX290_ISP_six || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){	
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-1");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}

	else if (folio == AN510){
		gpio[0] = 298;
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}

	else if (folio == N622){
		gpio[0] = 298;
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-2");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb5/5-1");
	}
	
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case WIFI:
			break;
		case BT:
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mSATA:
			configs->pCfg_mSATA->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_mSATA->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mPCIE:
			if (folio == AN310_IMX290 || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){	
				configs->pCfg_mPCIE->nMPCIE_type = STANDARD;
			}
			else if (folio == N622){
				configs->pCfg_mPCIE->nMPCIE_type = DUALMPCIE;
			}
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART_CSI:
			configs->pCfg_UART_CSI->pszDeviceName = malloc(strlen(uart_csi_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszDeviceName, uart_csi_dev_path, strlen(uart_csi_dev_path)+1);
			configs->pCfg_UART_CSI->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			if (folio == AN510 || folio == N622){
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/(sizeof(int)+1);
				results->pResult_GPIO->nPinCount = 4;
			}
			else {
			configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
			results->pResult_GPIO->nPinCount = 5;
			}
			configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
			memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
			results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
			memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			break;
		case FAN:
			configs->pCfg_FAN->nValue = 255;
			break;
		case CANbus:
			configs->pCfg_CAN->pszSendCAN = malloc(strlen(canbus_sender)*sizeof(char));
			configs->pCfg_CAN->pszReceiveCAN = malloc(strlen(canbus_reciver)*sizeof(char));
			configs->pCfg_CAN->pszId = malloc(strlen(canbus_id)*sizeof(char));
			configs->pCfg_CAN->pszData = malloc(strlen(canbus_data)*sizeof(char));
			strcpy(configs->pCfg_CAN->pszSendCAN, canbus_sender);
			strcpy(configs->pCfg_CAN->pszReceiveCAN, canbus_reciver);
			strcpy(configs->pCfg_CAN->pszId, canbus_id);
			strcpy(configs->pCfg_CAN->pszData, canbus_data);
			break;
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 0;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;
		case I2C_CSI:
			configs->pCfg_I2C_CSI->nI2C_ID = 1;
			configs->pCfg_I2C_CSI->nAddress = 0x50;
			configs->pCfg_I2C_CSI->nData = 0xa5;
			break;			
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Micro:
		case USB_Type_A:
		case USB_Type_C:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			configs->pCfg_USB->pMICRO_USB_DEV = malloc(strlen(usb_3_micro_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pMICRO_USB_DEV, usb_3_micro_dev);
			break;
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 7;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case Camera:
			if (folio == N622){
				configs->pCfg_Camera->nCameraType = CameraType_N622;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX290_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
	
			else if (folio == AN310_IMX290_ISP_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN310_IMX290_Thine){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_Thine;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			break;
		case Temperature:
			configs->pCfg_Temperature->nTemperature_i2c_ID = 0;
			configs->pCfg_Temperature->nTemperature_i2c_Address = 0x48;
			break;		
		default:
			break;
		}
		pItem = pItem->next;
	}
}

void setCfgNRst_TX2_4GB(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS2";
	char uart_dev_path[] = "/dev/ttyS0";
	char uart_csi_dev_path[] = "/dev/ttyTHS1";
	char serial_test_data[] = "Aetina";
	int gpio[5] = {488, 388, 389, 481, 397};
	char spi_dev_path[] = "/dev/spidev3.0";
	char canbus_sender[] = "can1";
	char canbus_reciver[] = "can0";
	char canbus_id[] = "123";
	char canbus_data[] = "abcdabcd";
  	char usb_3_type_a_up_usb3_dev[10];
  	char usb_3_type_a_up_usb2_dev[10];
	char usb_3_type_a_down_usb3_dev[10];
	char usb_3_type_a_donw_usb2_dev[10];
  	char usb_3_micro_dev[10];
	char eeprom_data[] = "Aetina Corporation";
	memset(usb_3_type_a_up_usb3_dev, 0, sizeof(usb_3_type_a_up_usb3_dev));
	memset(usb_3_type_a_up_usb2_dev, 0, sizeof(usb_3_type_a_up_usb2_dev));
	memset(usb_3_type_a_down_usb3_dev, 0, sizeof(usb_3_type_a_down_usb3_dev));
	memset(usb_3_type_a_donw_usb2_dev, 0, sizeof(usb_3_type_a_donw_usb2_dev));
	memset(usb_3_micro_dev, 0, sizeof(usb_3_micro_dev));

	if (folio == AN310 || folio == AN310_IMX290 || folio == AN310_IMX290_six || folio == AN310_IMX290_ISP || folio == AN310_IMX290_ISP_six || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-1");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	else if (folio == AN510){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	else if (folio == N622){
		gpio[0] = 298;
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-2/");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mSATA:
			configs->pCfg_mSATA->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_mSATA->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mPCIE:
			if (folio == AN310 || folio == AN310_IMX290 || folio == AN310_IMX290_six || folio == AN310_IMX290_ISP || folio == AN310_IMX290_ISP_six || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){
				configs->pCfg_mPCIE->nMPCIE_type = STANDARD;
			}
			else if (folio == N622){
				configs->pCfg_mPCIE->nMPCIE_type = N622;			
			}
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
                case UART_CSI:
			configs->pCfg_UART_CSI->pszDeviceName = malloc(strlen(uart_csi_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszDeviceName, uart_csi_dev_path, strlen(uart_csi_dev_path)+1);
			configs->pCfg_UART_CSI->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
			configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
			memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
			results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
			memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			results->pResult_GPIO->nPinCount = 5;
			break;
		case FAN:
			configs->pCfg_FAN->nValue = 255;
			break;
		case CANbus:
			configs->pCfg_CAN->pszSendCAN = malloc(strlen(canbus_sender)*sizeof(char));
			configs->pCfg_CAN->pszReceiveCAN = malloc(strlen(canbus_reciver)*sizeof(char));
			configs->pCfg_CAN->pszId = malloc(strlen(canbus_id)*sizeof(char));
			configs->pCfg_CAN->pszData = malloc(strlen(canbus_data)*sizeof(char));
			strcpy(configs->pCfg_CAN->pszSendCAN, canbus_sender);
			strcpy(configs->pCfg_CAN->pszReceiveCAN, canbus_reciver);
			strcpy(configs->pCfg_CAN->pszId, canbus_id);
			strcpy(configs->pCfg_CAN->pszData, canbus_data);
			break;
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 0;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;
                case I2C_CSI:
			configs->pCfg_I2C_CSI->nI2C_ID = 1;
			configs->pCfg_I2C_CSI->nAddress = 0x50;
			configs->pCfg_I2C_CSI->nData = 0xa5;
			break;	
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Micro:
			break;
		case USB_Type_A:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			configs->pCfg_USB->pMICRO_USB_DEV = malloc(strlen(usb_3_micro_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pMICRO_USB_DEV, usb_3_micro_dev);
			break;
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 7;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case Camera:
			if (folio == N622){
				configs->pCfg_Camera->nCameraType = CameraType_N622;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX290_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX290_ISP_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN310_IMX290_Thine){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_Thine;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			break;
		case Temperature:
			configs->pCfg_Temperature->nTemperature_i2c_ID = 0;
			configs->pCfg_Temperature->nTemperature_i2c_Address = 0x48;
			break;		
		default:
			break;
		}
		pItem = pItem->next;
	}
}

void setCfgNRst_TX2i(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS2";
	char uart_dev_path[] = "/dev/ttyS0";
	char uart_csi_dev_path[] = "/dev/ttyTHS1";
	char serial_test_data[] = "Aetina";
	char spi_dev_path[] = "/dev/spidev3.0";
	int gpio[] = {488, 388, 389, 481, 397};
	char canbus_sender[] = "can1";
	char canbus_reciver[] = "can0";
	char canbus_id[10] = "123";
	char canbus_data[] = "abcdabcd";
  	char usb_3_type_a_up_usb3_dev[10];
  	char usb_3_type_a_up_usb2_dev[10];
	char usb_3_type_a_down_usb3_dev[10];
	char usb_3_type_a_donw_usb2_dev[10];
  	char usb_3_micro_dev[10];
	char eeprom_data[] = "Aetina Corporation";
	memset(usb_3_type_a_up_usb3_dev, 0, sizeof(usb_3_type_a_up_usb3_dev));
	memset(usb_3_type_a_up_usb2_dev, 0, sizeof(usb_3_type_a_up_usb2_dev));
	memset(usb_3_type_a_down_usb3_dev, 0, sizeof(usb_3_type_a_down_usb3_dev));
	memset(usb_3_type_a_donw_usb2_dev, 0, sizeof(usb_3_type_a_donw_usb2_dev));
	memset(usb_3_micro_dev, 0, sizeof(usb_3_micro_dev));
	if (folio == AN310 || folio == AN310_IMX290 || folio == AN310_IMX290_six || folio == AN310_IMX290_ISP || folio == AN310_IMX290_ISP_six || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){	
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-1");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}

	else if (folio == AN510){
		gpio[0] = 298;
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}

	else if (folio == N622){
		gpio[0] = 298;
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-2/");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mSATA:
			configs->pCfg_mSATA->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_mSATA->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case mPCIE:
			if (folio == AN310_IMX290 || folio == AN310_IMX334 || folio == AN310_IMX334_ISP){	
				configs->pCfg_mPCIE->nMPCIE_type = STANDARD;
			}
			else if (folio == N622){
				configs->pCfg_mPCIE->nMPCIE_type = DUALMPCIE;
			}
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART_CSI:
			configs->pCfg_UART_CSI->pszDeviceName = malloc(strlen(uart_csi_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszDeviceName, uart_csi_dev_path, strlen(uart_csi_dev_path)+1);
			configs->pCfg_UART_CSI->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART_CSI->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			if (folio == AN510 || folio == N622){
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/(sizeof(int)+1);
				results->pResult_GPIO->nPinCount = 4;
			}
			else {
			configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
			results->pResult_GPIO->nPinCount = 5;
			}
			configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
			memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
			results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
			memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			break;
		case FAN:
			configs->pCfg_FAN->nValue = 255;
			break;
		case CANbus:
			configs->pCfg_CAN->pszSendCAN = malloc(strlen(canbus_sender)*sizeof(char));
			configs->pCfg_CAN->pszReceiveCAN = malloc(strlen(canbus_reciver)*sizeof(char));
			configs->pCfg_CAN->pszId = malloc(strlen(canbus_id)*sizeof(char));
			configs->pCfg_CAN->pszData = malloc(strlen(canbus_data)*sizeof(char));
			strcpy(configs->pCfg_CAN->pszSendCAN, canbus_sender);
			strcpy(configs->pCfg_CAN->pszReceiveCAN, canbus_reciver);
			strcpy(configs->pCfg_CAN->pszId, canbus_id);
			strcpy(configs->pCfg_CAN->pszData, canbus_data);
			break;
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 0;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;
		case I2C_CSI:
			configs->pCfg_I2C_CSI->nI2C_ID = 1;
			configs->pCfg_I2C_CSI->nAddress = 0x50;
			configs->pCfg_I2C_CSI->nData = 0xa5;
			break;			
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Micro:
		case USB_Type_A:
		case USB_Type_C:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			configs->pCfg_USB->pMICRO_USB_DEV = malloc(strlen(usb_3_micro_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pMICRO_USB_DEV, usb_3_micro_dev);
			break;
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 7;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case Camera:
			if (folio == N622){
				configs->pCfg_Camera->nCameraType = CameraType_N622;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX334_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN310_IMX290_ISP){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN310_IMX290_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
	
			else if (folio == AN310_IMX290_ISP_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN310_IMX290_Thine){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_Thine;
				configs->pCfg_Camera->nCameraCount = 3;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			break;
		case Temperature:
			configs->pCfg_Temperature->nTemperature_i2c_ID = 0;
			configs->pCfg_Temperature->nTemperature_i2c_Address = 0x48;
			break;		
		default:
			break;
		}
		pItem = pItem->next;
	}
}

void setCfgNRst_Xavier(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS0";
	char rs232_test_data[] = "Aetina";
	char uart_dev_path[] = "/dev/ttyTHS1";
	char uart2_dev_path[] = "/dev/ttyTHS4";
	char spi_dev_path[] = "/dev/spidev0.0";
	char ekey_keyword[] = "Realtek";
	char bkey_keyword[] = "Telit";
	int  gpio[5] = {437, 417, 393, 424, 344};
	char canbus_sender[] = "can1";
	char canbus_reciver[] = "can0";
	char canbus_id[] = "123";
	char canbus_data[] = "abcdabcd";
  	char usb_3_type_a_up_usb3_dev[10] = "usb2/2-3";
  	char usb_3_type_a_up_usb2_dev[10] = "usb1/1-3";
	char usb_3_type_a_down_usb3_dev[10] = "usb2/2-1";
	char usb_3_type_a_donw_usb2_dev[10] = "usb1/1-2";
  	char usb_3_type_c_usb3_dev[10] = "usb2/2-3";
  	char usb_3_type_c_usb2_dev[10] = "usb1/1-2";
  	char usb_3_micro_dev[10] = "usb1/1-1";
	char eeprom_data[] = "Aetina Corporation";
	memset(usb_3_type_a_up_usb3_dev, 0, sizeof(usb_3_type_a_up_usb3_dev));
	memset(usb_3_type_a_up_usb2_dev, 0, sizeof(usb_3_type_a_up_usb2_dev));
	memset(usb_3_type_a_down_usb3_dev, 0, sizeof(usb_3_type_a_down_usb3_dev));
	memset(usb_3_type_a_donw_usb2_dev, 0, sizeof(usb_3_type_a_donw_usb2_dev));
	memset(usb_3_micro_dev, 0, sizeof(usb_3_micro_dev));

	if (folio == AX710){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-1");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-4");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-4");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-4");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	else if (folio == T012){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-4");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-4");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-4");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-4");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}
	else if (folio == AX720){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-4/");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-4/");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-4/");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-4/");
		sprintf(usb_3_micro_dev,            "usb1/1-1");
	}

	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			if (folio == AX710 || folio == AX710_IMX334 || folio == AX720){
				configs->mLAN_Count = 3;
				configs->pCfg_LAN = realloc(configs->pCfg_LAN, 3 * sizeof(LAN_CFG));
				results->pResult_LAN = realloc(results->pResult_LAN, 3 * sizeof(LAN_RESULT));
				memset(results->pResult_LAN, 0, 3 * sizeof(LAN_RESULT));

				for (int i=0;i<3;i++){
					configs->pCfg_LAN[i].nLAN_ID = i;
				}
			}
			else {
				configs->mLAN_Count = 1;
				configs->pCfg_LAN->nLAN_ID = 0;
			}
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(rs232_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, rs232_test_data, strlen(rs232_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			break;
		case UART2:
			configs->pCfg_UART2->pszDeviceName = malloc(strlen(uart2_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART2->pszDeviceName, uart2_dev_path, strlen(uart2_dev_path)+1);
			break;
		case GPIO:
			configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
			configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
			memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
			results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
			memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			results->pResult_GPIO->nPinCount = 5;
			break;
		case FAN:
			configs->pCfg_FAN->nValue = 255;
			break;
		case CANbus:
			configs->pCfg_CAN->pszSendCAN = malloc(strlen(canbus_sender)*sizeof(char));
			configs->pCfg_CAN->pszReceiveCAN = malloc(strlen(canbus_reciver)*sizeof(char));
			configs->pCfg_CAN->pszId = malloc(strlen(canbus_id)*sizeof(char));
			configs->pCfg_CAN->pszData = malloc(strlen(canbus_data)*sizeof(char));
			strcpy(configs->pCfg_CAN->pszSendCAN, canbus_sender);
			strcpy(configs->pCfg_CAN->pszReceiveCAN, canbus_reciver);
			strcpy(configs->pCfg_CAN->pszId, canbus_id);
			strcpy(configs->pCfg_CAN->pszData, canbus_data);
			break;
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 8;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;
		case I2C_CSI:
			configs->pCfg_I2C_CSI->nI2C_ID = 7;
			configs->pCfg_I2C_CSI->nAddress = 0x50;
			configs->pCfg_I2C_CSI->nData = 0xa5;
			break;
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Micro:
		case USB_Type_A:
		case USB_Type_C:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_C_USB3DEV = malloc(strlen(usb_3_type_c_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_C_USB3DEV, usb_3_type_c_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_C_USB2DEV = malloc(strlen(usb_3_type_c_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_C_USB2DEV, usb_3_type_c_usb2_dev);
			configs->pCfg_USB->pMICRO_USB_DEV = malloc(strlen(usb_3_micro_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pMICRO_USB_DEV, usb_3_micro_dev);
			break;
		case m2_eKey:
			configs->pCfg_m2_eKey->pszKeyword = malloc(strlen(ekey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_eKey->pszKeyword, ekey_keyword);
			break;
		case m2_bKey:
			configs->pCfg_m2_bKey->pszKeyword = malloc(strlen(bkey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_bKey->pszKeyword, bkey_keyword);
			break;
		case m2_mKey_SATA:
			configs->pCfg_m2_mKey_SATA->nDeviceType = SATA;
			configs->pCfg_m2_mKey_SATA->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			break;
		case m2_mKey_PCIE:
			configs->pCfg_m2_mKey_PCIE->nDeviceType = PCIE;
			break;
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 0;
			configs->pCfg_EEPROM->nAddress = 0x56;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case Camera:
			break;
				
		default:
			break;
		}
		pItem = pItem->next;
	}

	return;
}

void setCfgNRst_Xavier_NX(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){

	// init
	printf("Setting up Config and Result on NX.\n\n");
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS1";
	char uart_dev_path[] = "/dev/ttyTHS0";
	char serial_test_data[] = "Aetina";
	char spi_dev_path[] = "/dev/spidev2.0";
	char ekey_keyword[] = "Intel";
	char bkey_keyword[] = "Telit";
	int gpio[] = {421, 422, 445, 268, 393};
	int gpio_AT017[] =  {421, 422, 445, 268, 393, 491, 492, 493, 495};

 	char usb_3_type_a_up_usb3_dev[] = "usb2/2-3/2-3.1";
  	char usb_3_type_a_up_usb2_dev[] = "usb1/1-2/1-2.1";
	char usb_3_type_a_down_usb3_dev[] = "usb2/2-3/2-3.2";
	char usb_3_type_a_donw_usb2_dev[] = "usb1/1-2/1-2.2";
	
	char eeprom_data[] = "Aetina Corporation";
	char canbus_sender[] = "can1";
	char canbus_reciver[] = "can0";
	char canbus_id[] = "123";
	char canbus_data[] = "abcdabcd";

	// 不同的位置進行額外的宣告
	if (folio == AN810){
		memcpy(canbus_sender,"can3", 5);
	}
	
	if (folio == AT017){
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-3/2-3.3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-2/1-2.3");
		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-3/2-3.2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2/1-2.2");
	}
	// 各項目進行測試
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case WIFI:
			break;
		case BT:
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			if (folio == AT017){
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio_AT017)/sizeof(int);
				results->pResult_GPIO->nPinCount = 9;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio_AT017));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio_AT017, sizeof(gpio_AT017));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio_AT017));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio_AT017));
			}
			else{
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
				results->pResult_GPIO->nPinCount = 5;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			}
			break;
		case FAN:
			configs->pCfg_FAN->nFAN_Number = 448;
			configs->pCfg_FAN->nValue = 255;
			break;
		case eDP:
			configs->pCfg_eDP->nEDP_Number = 446;
			break;			
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 1;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;
		case I2C_II:
			configs->pCfg_I2C_II->nI2C_ID = 8;
			configs->pCfg_I2C_II->nAddress = 0x50;
			configs->pCfg_I2C_II->nData = 0xa5;	
			break;			
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Type_A:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			break;
		case m2_eKey:
			configs->pCfg_m2_eKey->pszKeyword = malloc(strlen(ekey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_eKey->pszKeyword, ekey_keyword);
			break;
		case m2_bKey:
			configs->pCfg_m2_bKey->pszKeyword = malloc(strlen(bkey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_bKey->pszKeyword, bkey_keyword);
			break;			
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 0;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case CANbus:
			configs->pCfg_CAN->pszSendCAN = malloc(strlen(canbus_sender)*sizeof(char));
			configs->pCfg_CAN->pszReceiveCAN = malloc(strlen(canbus_reciver)*sizeof(char));
			configs->pCfg_CAN->pszId = malloc(strlen(canbus_id)*sizeof(char));
			configs->pCfg_CAN->pszData = malloc(strlen(canbus_data)*sizeof(char));
			strcpy(configs->pCfg_CAN->pszSendCAN, canbus_sender);
			strcpy(configs->pCfg_CAN->pszReceiveCAN, canbus_reciver);
			strcpy(configs->pCfg_CAN->pszId, canbus_id);
			strcpy(configs->pCfg_CAN->pszData, canbus_data);
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;			
		case Camera:
			if (folio == AN110_IMX179_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX179;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX334_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX334ISP_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX334THCV_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_Thine;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio ==  AN110_IMX290_Dual){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 2;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX290ISP_Dual){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 2;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio ==  AN110_IMX290_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN110_IMX290_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX290ISP_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX290ISP_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX290THCV_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_Thine;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN810_IMX290THCV_six){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_six;
				configs->pCfg_Camera->nCameraCount = 6;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			break;
		default:
			break;
		}
		pItem = pItem->next;
	}
}

void setCfgNRst_Nano(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){
	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS2";
	char uart_dev_path[] = "/dev/ttyTHS1";
	char serial_test_data[] = "Aetina";
	char spi_dev_path[] = "/dev/spidev1.0";
	char ekey_keyword[] = "Intel";
	int gpio[] = {149, 79, 200, 194, 38};
	int gpio_AT017[] =  {149, 200,79, 194, 38,16,17,18,20};

	
 	char usb_3_type_a_up_usb3_dev[] = "usb2/2-1/2-1.2";
  	char usb_3_type_a_up_usb2_dev[] = "usb1/1-2/";
	char usb_3_type_a_down_usb3_dev[] = "usb2/2-1/2-1.1";
	if (folio == AT017){
		sprintf(usb_3_type_a_down_usb3_dev,"usb2/2-1/2-1.3");
	}
	char usb_3_type_a_donw_usb2_dev[] = "usb1/1-2/";
	char eeprom_data[] = "Aetina Corporation";
	
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case WIFI:
			break;
		case BT:
			break;
		case mSATA:
			configs->pCfg_mSATA->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_mSATA->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_mSATA->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			if (folio == AT017){
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio_AT017)/sizeof(int);
				results->pResult_GPIO->nPinCount = 9;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio_AT017));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio_AT017, sizeof(gpio_AT017));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio_AT017));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio_AT017));
			}
			else{
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
				results->pResult_GPIO->nPinCount = 5;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			}
			break;
		case FAN:
			configs->pCfg_FAN->nValue = 255;
			break;
		case eDP:
			configs->pCfg_eDP->nValue = 255;
			break;			
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 0;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;			
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Type_A:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			break;
		case m2_eKey:
			configs->pCfg_m2_eKey->pszKeyword = malloc(strlen(ekey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_eKey->pszKeyword, ekey_keyword);
			break;			
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 2;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;			
		case Camera:
			if (folio == AN110_IMX179_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX179;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX334_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX334ISP_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX334THCV_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX334_Thine;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio ==  AN110_IMX290_Dual){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 2;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX290ISP_Dual){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 2;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio ==  AN110_IMX290_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio ==  AN110_IMX290_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX290ISP_J13){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			else if (folio == AN110_IMX290ISP_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_ISP;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}			
			else if (folio == AN110_IMX290THCV_J8){
				configs->pCfg_Camera->nCameraType = CameraType_IMX290_Thine;
				configs->pCfg_Camera->nCameraCount = 1;
				results->pResult_Camera->pCameraResults = realloc(results->pResult_Camera->pCameraResults, configs->pCfg_Camera->nCameraCount * sizeof(int));
			}
			break;
		default:
			break;
		}
		pItem = pItem->next;
	}
}

// 0904_Add_TX2NX
void setCfgNRst_TX2_NX(const ProductPortFolio folio, Items *items, TestConfigs *configs, TestResults *results){

	// init
	printf("Setting up Config and Result on TX2-NX.\n\n");

	char tmp_path[] = "/mnt";
	char tmp_file_name[] = "100M.dd";
	char rs232_dev_path[] = "/dev/ttyTHS1";
	char uart_dev_path[] = "/dev/ttyTHS0";
	char serial_test_data[] = "Aetina";
	char spi_dev_path[] = "/dev/spidev2.0";
	char ekey_keyword[] = "Intel";
	char bkey_keyword[] = "Telit";
	int gpio[] = {421, 422, 445, 268, 393};
	// int gpio_AT017[] =  {421, 422, 445, 268, 393, 491, 492, 493, 495};
	// 0907 from Chris 應該要有9個 目前只有8個
	int gpio_AT017[] =  {425, 306, 338, 269, 376, 377, 378, 392};

 	char usb_3_type_a_up_usb3_dev[] = "usb2/2-3/2-3.1";
  	char usb_3_type_a_up_usb2_dev[] = "usb1/1-2/1-2.1";
	char usb_3_type_a_down_usb3_dev[] = "usb2/2-3/2-3.2";
	char usb_3_type_a_donw_usb2_dev[] = "usb1/1-2/1-2.2";
	
	char eeprom_data[] = "Aetina Corporation";

	
	if (folio == AT017){
		// 使用 udevadm monitor --kernel --property --subsystem-match=usb 進行監控
		sprintf(usb_3_type_a_up_usb3_dev,   "usb2/2-2/2-2.3");
		sprintf(usb_3_type_a_up_usb2_dev,   "usb1/1-2/1-2.3");

		sprintf(usb_3_type_a_down_usb3_dev, "usb2/2-2/2-2.2");
		sprintf(usb_3_type_a_donw_usb2_dev, "usb1/1-2/1-2.2");
	}
	// 各項目進行測試
	Items *pItem = items;
	while(pItem != NULL){
		switch (pItem->item_kind)
		{
		case LAN:
			configs->mLAN_Count = 1;
			configs->pCfg_LAN->nLAN_ID = 0;
			break;
		case WIFI:
			break;
		case BT:
			break;
		case RS232:	
			configs->pCfg_RS232->pszDeviceName = malloc(strlen(rs232_dev_path)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszDeviceName, rs232_dev_path, strlen(rs232_dev_path)+1);
			configs->pCfg_RS232->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_RS232->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case UART:
			configs->pCfg_UART->pszDeviceName = malloc(strlen(uart_dev_path)*sizeof(char));
			memcpy(configs->pCfg_UART->pszDeviceName, uart_dev_path, strlen(uart_dev_path)+1);
			configs->pCfg_UART->pszTestingData = malloc(strlen(serial_test_data)*sizeof(char));
			memcpy(configs->pCfg_UART->pszTestingData, serial_test_data, strlen(serial_test_data)+1);
			break;
		case GPIO:
			if (folio == AT017){
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio_AT017)/sizeof(int);
				results->pResult_GPIO->nPinCount = 9;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio_AT017));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio_AT017, sizeof(gpio_AT017));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio_AT017));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio_AT017));
			}
			else{
				configs->pCfg_GPIO->nGPIO_Pins_Number = sizeof(gpio)/sizeof(int);
				results->pResult_GPIO->nPinCount = 5;
				configs->pCfg_GPIO->pGPIO_Pins = realloc(configs->pCfg_GPIO->pGPIO_Pins, sizeof(gpio));
				memcpy(configs->pCfg_GPIO->pGPIO_Pins, gpio, sizeof(gpio));
				results->pResult_GPIO->nPinResults = realloc(results->pResult_GPIO->nPinResults, sizeof(gpio));
				memset(results->pResult_GPIO->nPinResults, 0, sizeof(gpio));
			}
			break;
		case FAN:
			configs->pCfg_FAN->nFAN_Number = 448;
			configs->pCfg_FAN->nValue = 255;
			break;		
		case I2C:
			configs->pCfg_I2C->nI2C_ID = 0;
			configs->pCfg_I2C->nAddress = 0x50;
			configs->pCfg_I2C->nData = 0xa5;
			break;		
		case SPI:
			configs->pCfg_SPI->pszDeviceName = malloc(strlen(spi_dev_path)*sizeof(char));
			strcpy(configs->pCfg_SPI->pszDeviceName, spi_dev_path);
			break;
		case USB_Type_A:
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV = malloc(strlen(usb_3_type_a_up_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB3DEV, usb_3_type_a_up_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV = malloc(strlen(usb_3_type_a_up_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_UP_USB2DEV, usb_3_type_a_up_usb2_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV = malloc(strlen(usb_3_type_a_down_usb3_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB3DEV, usb_3_type_a_down_usb3_dev);
			configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV = malloc(strlen(usb_3_type_a_donw_usb2_dev)*sizeof(char));
			strcpy(configs->pCfg_USB->pUSB_3_0_TYPE_A_DOWN_USB2DEV, usb_3_type_a_donw_usb2_dev);
			break;
		case m2_eKey:
			configs->pCfg_m2_eKey->pszKeyword = malloc(strlen(ekey_keyword)*sizeof(char));
			strcpy(configs->pCfg_m2_eKey->pszKeyword, ekey_keyword);
			break;		
		case EEPROM:
			configs->pCfg_EEPROM->nI2C_ID = 7;
			configs->pCfg_EEPROM->nAddress = 0x57;
			configs->pCfg_EEPROM->pszContent = malloc(strlen(eeprom_data)*sizeof(char));
			strcpy(configs->pCfg_EEPROM->pszContent, eeprom_data);
			results->pResult_EEPROM->pszResult = malloc(strlen(eeprom_data)*sizeof(char));
			break;
		case SDCARD:
			configs->pCfg_SDCard->pszTmp_path = malloc(strlen(tmp_path)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_path, tmp_path, strlen(tmp_path)+1);
			configs->pCfg_SDCard->pszTmp_file = malloc(strlen(tmp_file_name)*sizeof(char));
			memcpy(configs->pCfg_SDCard->pszTmp_file, tmp_file_name, strlen(tmp_file_name)+1);
			break;			
		case Camera:
			break;
		default:
			break;
		}
		pItem = pItem->next;
	}
}

/* 
 * Function: configureTestCore
 * ---------------------------
 * 
 * Descrition: Get module and board informations, 
 * 						 and set parameter according to the informations.
 * @param: TestCore *core 
 * return: void
*/
void configureTestCore(TestCore *core, TestMode mode){
	char szMsg[MAX_PATH];
	memset(szMsg, 0, sizeof(szMsg));
	core->pDeviceInfo = (DeviceInfo*)malloc(sizeof(DeviceInfo));
	core->pDeviceInfo->nLanCount = 0;
	core->pDeviceInfo = getDeviceInfo(core->pDeviceInfo);

	if (mode == TestMode_Auto){
		configureAutoRunItems(core);
		core->pTestReport->pGondanNumber = checkConfigInfo("gondannumber", core->pTestReport->pGondanNumber);
		core->pTestReport->pPartNumber   = checkConfigInfo("partnumber", core->pTestReport->pPartNumber);
		core->pTestReport->pBoardNumber  = checkConfigInfo("boardnumber", core->pTestReport->pBoardNumber);
		if (core->pTestReport->pGondanNumber == NULL || core->pTestReport->pBoardNumber  == NULL || core->pTestReport->pPartNumber == NULL){
			core->pTestReport = scanGondamAndPartNumber(core->pTestReport);
			core->pTestReport = scanBoardNumber(core->pTestReport);
		}
		
		if (core->pTestReport->pGondanNumber != NULL){
			sprintf(szMsg, "工單號碼: %s", core->pTestReport->pGondanNumber);
			logINFO(szMsg);
		}
		if (core->pTestReport->pPartNumber != NULL){
			sprintf(szMsg, "品號: %s", core->pTestReport->pPartNumber);
			logINFO(szMsg);
		}
		if (core->pTestReport->pBoardNumber != NULL){
			sprintf(szMsg, "底板序號: %s", core->pTestReport->pBoardNumber);
			logINFO(szMsg);
		}

		if (core->pTestReport->pPartNumber[0] == 'A' || core->pTestReport->pPartNumber[0] == '9'){
			//logINFO("It a product");
			
		}
		else{
			//logINFO("It a Board");
			core->pTestReport->isOnlyBoard = true;
		}
	}
	else{
		configureDebugItems(core);
	}

	setupCfgAndRst(core->pTestItems, core->pTestConfigs, core->pTestResults);

	char *module = core->pDeviceInfo->pModule;
	char *dts = core->pDeviceInfo->pszDTS_Name;
	if (strstr(module, "TX2")){
		if (strstr(module, "TX2_NX")){
			if (strstr(dts, "CT41_1")){
				setCfgNRst_TX2_NX(AT017, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
		}
		else if (strstr(module, "TX2_4GB")){
			// TX2-4GB
			if (strstr(dts, "N310")){
				if (strstr(dts, "IMX334")){
					if (strstr(dts, "M12MO")){
						setCfgNRst_TX2_4GB(AN310_IMX334_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2_4GB(AN310_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else if (strstr(dts, "IMX290")){
					if (strstr(dts, "IMX290_six")){
						setCfgNRst_TX2_4GB(AN310_IMX290_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO_six")){
						setCfgNRst_TX2_4GB(AN310_IMX290_ISP_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO")){
						setCfgNRst_TX2_4GB(AN310_IMX290_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "Thine")){
						setCfgNRst_TX2_4GB(AN310_IMX290_Thine, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2_4GB(AN310_IMX290, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else{
					setCfgNRst_TX2_4GB(AN310, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}
			else if (strstr(dts, "N510")){
				setCfgNRst_TX2_4GB(AN510, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
		}
		else if (strstr(module, "TX2i")){
			// TX2i
			if (strstr(dts, "N310")){
				if (strstr(dts, "IMX334")){
					if (strstr(dts, "M12MO")){
						setCfgNRst_TX2i(AN310_IMX334_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2i(AN310_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else if (strstr(dts, "IMX290")){
					if (strstr(dts, "IMX290_six")){
						setCfgNRst_TX2i(AN310_IMX290_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO_six")){
						setCfgNRst_TX2i(AN310_IMX290_ISP_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO")){
						setCfgNRst_TX2i(AN310_IMX290_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "Thine")){
						setCfgNRst_TX2i(AN310_IMX290_Thine, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2i(AN310_IMX290, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else{
					setCfgNRst_TX2i(AN310, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}
			else if (strstr(dts, "N510")){
				setCfgNRst_TX2i(AN510, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else if (strstr(dts, "N622")){
				setCfgNRst_TX2i(N622, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}			
		}		
		else{
			// TX2
			if (strstr(dts, "N310")){
				if (strstr(dts, "IMX334")){
					if (strstr(dts, "M12MO")){
						setCfgNRst_TX2(AN310_IMX334_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2(AN310_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else if (strstr(dts, "IMX290")){
					if (strstr(dts, "IMX290_six")){
						setCfgNRst_TX2(AN310_IMX290_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO_six")){
						setCfgNRst_TX2(AN310_IMX290_ISP_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "M12MO")){
						setCfgNRst_TX2(AN310_IMX290_ISP, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else if (strstr(dts, "Thine")){
						setCfgNRst_TX2(AN310_IMX290_Thine, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
					else{
						setCfgNRst_TX2(AN310_IMX290, core->pTestItems, core->pTestConfigs, core->pTestResults);
					}
				}
				else{
					setCfgNRst_TX2(AN310, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}	
			}
			else if (strstr(dts, "N510")){
				setCfgNRst_TX2(AN510, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else if (strstr(dts, "N622")){
				setCfgNRst_TX2(N622, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
		}
	}
	//xavier & NX
	else if (strstr(module, "Xavier_NX")){
		if (strstr(dts, "AN110")){
			if (strstr(dts, "IMX179_J13")){
				setCfgNRst_Xavier_NX(AN110_IMX179_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else if (strstr(dts, "IMX334")){
				if (strstr(dts, "ISP_J8")){
					setCfgNRst_Xavier_NX(AN110_IMX334ISP_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "THCV")){
					setCfgNRst_Xavier_NX(AN110_IMX290THCV_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}			
				else {
					setCfgNRst_Xavier_NX(AN110_IMX334_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}		
			else if (strstr(dts, "IMX290")){
				if (strstr(dts, "IMX290_Dual")){
					setCfgNRst_Xavier_NX(AN110_IMX290_Dual, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "IMX290ISP_Dual")){
					setCfgNRst_Xavier_NX(AN110_IMX290ISP_Dual, core->pTestItems, core->pTestConfigs, core->pTestResults);
					printf("AN110_IMX290ISP_Dual\n");
					sleep(2);
				}			
				else if (strstr(dts, "IMX290_J13")){
					setCfgNRst_Xavier_NX(AN110_IMX290_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "IMX290_J8")){
					setCfgNRst_Xavier_NX(AN110_IMX290_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "ISP_J13")){
					setCfgNRst_Xavier_NX(AN110_IMX290ISP_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "ISP_J8")){
					setCfgNRst_Xavier_NX(AN110_IMX290ISP_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}			
				else if (strstr(dts, "THCV")){
					setCfgNRst_Xavier_NX(AN110_IMX290THCV_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}
			else {
				setCfgNRst_Xavier_NX(AN110, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
		}

		else if (strstr(dts, "AN810")){
			if (strstr(dts, "IMX290THCV_six")){
				setCfgNRst_Xavier_NX(AN810_IMX290THCV_six, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else{
				setCfgNRst_Xavier_NX(AN810, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			
		}
		else if (strstr(dts, "AT017")){
			setCfgNRst_Xavier_NX(AT017, core->pTestItems, core->pTestConfigs, core->pTestResults);
		}
	}	
	else if (strstr(module, "Xavier")){;
		if (strstr(dts, "AX710")){
			if (strstr(dts, "IMX334")){
				setCfgNRst_Xavier(AX710_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else{
				setCfgNRst_Xavier(AX710, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
		}
		else if (strstr(dts, "T012")){
			//if (strstr(dts, "IMX334")){
			//	setCfgNRst_Xavier(T012_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
			//}
			//else{
				setCfgNRst_Xavier(T012, core->pTestItems, core->pTestConfigs, core->pTestResults);
			//}
		}
		else if (strstr(dts, "AX720")){
			//if (strstr(dts, "IMX334")){
			//	setCfgNRst_Xavier(T012_IMX334, core->pTestItems, core->pTestConfigs, core->pTestResults);
			//}
			//else{
				setCfgNRst_Xavier(AX720, core->pTestItems, core->pTestConfigs, core->pTestResults);
			//}
		}
	}	
	else if (strstr(module, "Nano")){
		if (strstr(dts, "AN110")){
			if (strstr(dts, "IMX179_J13")){
				setCfgNRst_Nano(AN110_IMX179_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}
			else if (strstr(dts, "IMX334")){
				if (strstr(dts, "ISP_J8")){
					setCfgNRst_Nano(AN110_IMX334ISP_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "THCV")){
					setCfgNRst_Nano(AN110_IMX290THCV_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}			
				else {
					setCfgNRst_Nano(AN110_IMX334_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}		
			else if (strstr(dts, "IMX290")){
				if (strstr(dts, "IMX290_Dual")){
					setCfgNRst_Nano(AN110_IMX290_Dual, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "IMX290ISP_Dual")){
					setCfgNRst_Nano(AN110_IMX290ISP_Dual, core->pTestItems, core->pTestConfigs, core->pTestResults);
					printf("AN110_IMX290ISP_Dual\n");
					sleep(2);
				}			
				else if (strstr(dts, "IMX290_J13")){
					setCfgNRst_Nano(AN110_IMX290_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "IMX290_J8")){
					setCfgNRst_Nano(AN110_IMX290_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "ISP_J13")){
					setCfgNRst_Nano(AN110_IMX290ISP_J13, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
				else if (strstr(dts, "ISP_J8")){
					setCfgNRst_Nano(AN110_IMX290ISP_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}			
				else if (strstr(dts, "THCV")){
					setCfgNRst_Nano(AN110_IMX290THCV_J8, core->pTestItems, core->pTestConfigs, core->pTestResults);
				}
			}
			else{
				setCfgNRst_Nano(AN110, core->pTestItems, core->pTestConfigs, core->pTestResults);
			}			
		}
		else if (strstr(dts, "AT017")){
			setCfgNRst_Nano(AT017, core->pTestItems, core->pTestConfigs, core->pTestResults);
		}
		
	}
}
