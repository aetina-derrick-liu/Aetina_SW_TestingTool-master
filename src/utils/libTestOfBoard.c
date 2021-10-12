#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/i2c.h>
#include <pthread.h>
#include "utils/libTestOfBoard.h"

#define LIB_VERSION		"0.1.1"
#define MAX_CMD			1024
#define MAX_PATH		256
#define BAUDRATE 		B38400

typedef struct tagTOB{
	CALLBACK_LOG_FUNC 	log_func;
	int					nModuleType;
}TOB_OBJ;

static TOB_OBJ g_TOBObj = {0};

void TOB_GetLibVersion(char *szVersion){
	if(szVersion != NULL){
		sprintf(szVersion, "%s", LIB_VERSION);
	}
}

void TOB_SetLogFunc(CALLBACK_LOG_FUNC func){
	if(func != NULL)
		g_TOBObj.log_func = func;
}

DeviceInfo* TOB_GetDeviceInfo(DeviceInfo *devinfo_result){
	char szResult[MAX_PATH] = {0};
	char szCMD[MAX_PATH] = {0};
	FILE *fp = NULL;

	if(devinfo_result == NULL)
		return;

	switch(GetModuleType()){
		case TX2:
			devinfo_result->pModule = malloc(strlen("TX2")*sizeof(char));
			strcpy(devinfo_result->pModule, "TX2");
			break;
		case TX2_4GB:
			devinfo_result->pModule = malloc(strlen("TX2_4GB")*sizeof(char));
			strcpy(devinfo_result->pModule, "TX2_4GB");
			break;
		case TX2i:
			devinfo_result->pModule = malloc(strlen("TX2i")*sizeof(char));
			strcpy(devinfo_result->pModule, "TX2i");
			break;
		case TX2_NX:
			devinfo_result->pModule = malloc(strlen("TX2_NX")*sizeof(char));
			strcpy(devinfo_result->pModule, "TX2_NX");
			break;
		case XAVIER:
			devinfo_result->pModule = malloc(strlen("Xavier")*sizeof(char));
			strcpy(devinfo_result->pModule, "Xavier");
			break;
		case XAVIER_NX:
			devinfo_result->pModule = malloc(strlen("Xavier_NX")*sizeof(char));
			strcpy(devinfo_result->pModule, "Xavier_NX");
			break;
		case NANO:
			devinfo_result->pModule = malloc(strlen("Nano")*sizeof(char));
			strcpy(devinfo_result->pModule, "Nano");
			break;
	}

	//get serial number
	memset(szResult, 0, MAX_PATH);
	fp = popen("cat /sys/firmware/devicetree/base/serial-number", "r");
	if(fp)
		fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	if(devinfo_result->pszSerialNumber == NULL && strlen(szResult)){
		devinfo_result->pszSerialNumber = malloc(strlen(szResult));
		strcpy(devinfo_result->pszSerialNumber, szResult);
	}

	memset(szResult, 0, MAX_PATH);
	fp = popen("cat /proc/net/dev | grep eth", "r");
	while (fgets(szResult, MAX_PATH, fp)){
		devinfo_result->nLanCount++;
	}
	pclose(fp);

	if (devinfo_result->nLanCount > 0){
		devinfo_result->pszMAC_Address = malloc(devinfo_result->nLanCount * sizeof(char*));
		for (int i=0;i<devinfo_result->nLanCount;i++){
			devinfo_result->pszMAC_Address[i] = malloc((MAC_SIZE-1)*sizeof(char));
			memset(devinfo_result->pszMAC_Address[i], 0, sizeof(devinfo_result->pszMAC_Address[i]));
		}
	}

	//get mac address
	memset(szResult, 0, MAX_PATH);
	for (int i = 0; i < devinfo_result->nLanCount; i++)
	{
		sprintf(szCMD, "ip addr show eth%d", i);
		fp = popen(szCMD, "r");
		if (fp){
			while (fgets(szResult, MAX_PATH, fp)){
				if (strstr(szResult, "link/ether")){
					strncpy(devinfo_result->pszMAC_Address[i], strstr(szResult, ":")-2, MAC_SIZE-1);
					devinfo_result->pszMAC_Address[i][MAC_SIZE] = '\0';
				}
				else if (strstr(szResult, "not exist")){
					continue;
				}
			}
		}
	}

	//get bsp version
	memset(szResult, 0, MAX_PATH);
	fp = popen("head -n 1 /etc/nv_tegra_release", "r");
	if(fp)
		fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	if(strlen(szResult)){
		devinfo_result->pszBSP_Version = malloc((strlen(szResult) - strlen(strstr(szResult, "GCID:")) - 2)*sizeof(char));
		strncpy(devinfo_result->pszBSP_Version, szResult + 2, strlen(szResult) - strlen(strstr(szResult, "GCID:")) - 1);
		char *tok = strrchr(devinfo_result->pszBSP_Version, ',');
		strncpy(tok, "\0", 1);
	}

	//get dts name
	memset(szResult, 0, MAX_PATH);
	//fp = popen("dmesg | grep dts", "r");
	fp = popen("cat /proc/device-tree/nvidia,dtsfilename", "r");
	
	if(fp)
		fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	if(devinfo_result->pszDTS_Name == NULL && strlen(szResult)){
		//char *pDts = strstr(szResult, ":")+2;
		char *pDts = szResult;
		devinfo_result->pszDTS_Name = realloc(devinfo_result->pszDTS_Name, strlen(pDts) * sizeof(char));
		strncpy(devinfo_result->pszDTS_Name, pDts, strlen(pDts)+1);
		//char *eof = strstr(devinfo_result->pszDTS_Name, "\n");
		//strncpy(eof, "\0", 1);
		if (strstr(szResult, "N310")){
			devinfo_result->pszCarrierBoard = malloc(strlen("N310") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "N310");
		}
		else if (strstr(szResult, "N510")){
			devinfo_result->pszCarrierBoard = malloc(strlen("N510") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "N510");
		}
		else if (strstr(szResult, "N622")){
			devinfo_result->pszCarrierBoard = malloc(strlen("N622") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "N622");
		}
		else if (strstr(szResult, "AX710")){
			devinfo_result->pszCarrierBoard = malloc(strlen("AX710") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AX710");
		}
		else if (strstr(szResult, "AX720")){
			devinfo_result->pszCarrierBoard = malloc(strlen("AX720") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AX720");
		}
		else if (strstr(szResult, "AN110")){
			devinfo_result->pszCarrierBoard = malloc(strlen("AN110") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AN110");
		}
		else if (strstr(szResult, "AN810")){
			devinfo_result->pszCarrierBoard = malloc(strlen("AN810") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AN810");
		}
		else if (strstr(szResult, "T012")){
			devinfo_result->pszCarrierBoard = malloc(strlen("T012") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "T012");
		}
		else if (strstr(szResult, "AT017")){
			devinfo_result->pszCarrierBoard = malloc(strlen("AT017") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AT017");
		}
		else if (strstr(szResult, "CT41_1")){
			printf("This DTS is for CT41_1 ( M1 ), but setup to AT107.\n");
			devinfo_result->pszCarrierBoard = malloc(strlen("AT017") * sizeof(char));
			strcpy(devinfo_result->pszCarrierBoard, "AT017");
		}
		else {
			printf("This Board is not support.\n");
			exit(0);
		}
		printf("BPS: %s\n",devinfo_result->pszBSP_Version);
		printf("Module: %s\n",devinfo_result->pModule);
		printf("Board: %s\n",devinfo_result->pszCarrierBoard);
		printf("--------------------TestItems-----------------------------------\n");
	}

	return devinfo_result;
}

int SystemCMD(char *szCMD){
	char szMsg[MAX_CMD] = {0};
	int status = 0;
	if(szCMD == NULL)
	{
		return -EINVAL;
	}
#ifdef _DEBUG_
	else{
		g_TOBObj.log_func(szCMD);
	}
#endif

	status = system(szCMD);
	if(status < 0)
	{
		sprintf(szMsg, "szCMD : %s error : %s", szCMD, strerror(errno));
		return -EINVAL;
	}

	if(WIFEXITED(status))
	{
#ifdef _DEBUG_
		sprintf(szMsg, "normal termination, exit status = %s", WEXITSTATUS(status));
#endif
	}
	else if(WIFSIGNALED(status))
	{
		sprintf(szMsg, "abnormal termination,signal number =%d\n", WTERMSIG(status));
	}
	else if(WIFSTOPPED(status))
	{
		sprintf(szMsg, "process stopped, signal number =%d\n", WSTOPSIG(status));
	}

	return OK;
}

int GetModuleType(){
	unsigned short chip_id = -1;
	FILE *fp = popen("cat /sys/module/tegra_fuse/parameters/tegra_chip_id", "r");
	char szCMD[MAX_PATH] = {0};
	fgets(szCMD, MAX_PATH, fp);
	pclose(fp);
	
	switch(atoi(szCMD)){
		case TX2_ID:				
			chip_id = TX2_ID;
			break;
		case XAVIER_ID:
			chip_id = XAVIER_ID;
			break;
		case NANO_ID:
			chip_id = NANO_ID;
			break;
	}

	if (chip_id == TX2_ID){
		char szResult[MAX_PATH] = {0};
		//FILE *pf_dts = popen("dmesg | grep dts", "r");
		FILE *pf_dts = popen("cat /proc/device-tree/nvidia,dtsfilename", "r");
		
		while(fgets(szResult, MAX_PATH, pf_dts)){

			if (strstr(szResult, "_TX2-NX_")){
				return TX2_NX;
			}
			else if (strstr(szResult, "_TX2_")){
				return TX2;
			}
			else if(strstr(szResult, "_TX2-4GB_")){
				return TX2_4GB;
			}
			else if(strstr(szResult, "_TX2i_")){
				return TX2i;
			}
		}
	}
	else if(chip_id == XAVIER_ID){
		char szResult[MAX_PATH] = {0};
		//FILE *pf_dts = popen("dmesg | grep dts", "r");
		FILE *pf_dts = popen("cat /proc/device-tree/nvidia,dtsfilename", "r");

		while(fgets(szResult, MAX_PATH, pf_dts)){
			if (strstr(szResult, "_Xavier_")){
				return XAVIER;
			}
			else if(strstr(szResult, "_Xavier-NX_")){
				return XAVIER_NX;
			}
		}
	}
	else if (chip_id == NANO_ID){
		return NANO;
	}
}

int GetDts(){
	unsigned short chip_id = -1;
	FILE *fp = popen("cat /sys/module/tegra_fuse/parameters/tegra_chip_id", "r");
	char szCMD[MAX_PATH] = {0};
	fgets(szCMD, MAX_PATH, fp);
	pclose(fp);
	
	char szResult[MAX_PATH] = {0};
	//FILE *pf_dts = popen("dmesg | grep dts", "r");
	FILE *pf_dts = popen("cat /proc/device-tree/nvidia,dtsfilename", "r");
	while(fgets(szResult, MAX_PATH, pf_dts)){
		
		if (strstr(szResult, "AN110")){
			return AN110;
		}
		if (strstr(szResult, "AN810")){
			return AN810;
		}
		if (strstr(szResult, "AT017")){
			return AT017;
		}
		if (strstr(szResult, "CT41_1")){
			return AT017;
		}
		if (strstr(szResult, "T012")){
			return T012;
		}
		
	}
}

int TOB_LAN_testing(LAN_CFG *lan_cfg, LAN_RESULT *lan_result){
	bool bPass = false;
	char szResult[MAX_PATH] = {0};
	FILE *pFile = popen("ifconfig", "r");
	if(pFile){
		char szEthernetPort[MAX_PATH] = {0};
		bool bFind = false;
		sprintf(szEthernetPort, "eth%d", lan_cfg->nLAN_ID);
		while(fgets(szResult, MAX_PATH, pFile)){
			if (strstr(szResult, szEthernetPort)){
				while(fgets(szResult, MAX_PATH, pFile) && strstr(szResult, "inet ")){
					strncpy(lan_result->szIP, strstr(szResult, "inet ") + strlen("inet "), strstr(szResult, "  netmask") - strstr(szResult, "inet ") - strlen("inet "));
					if(strlen(lan_result->szIP)){
						bFind = true;
						break;
					}
				}
			}
			
			if(bFind)
				break;
		}
		pclose(pFile);

		if(bFind){
			int nCount = 0;
			sprintf(szResult, "ping %s", lan_result->szIP);
			pFile  = popen(szResult, "r");
			if(pFile){
				while(fgets(szResult, MAX_PATH, pFile)){
					if(strstr(szResult, "icmp_seq") && !strstr(szResult, "Unreachable")){
						nCount++;
					}
					if(nCount > 3){
						bPass = true;
						break;
					}
				}
				pclose(pFile);
			}

			if(bPass)
				lan_result->nResult = OK;
			else
				lan_result->nResult = FAILED;

			return lan_result->nResult;
		}
		else
		{
			lan_result->nResult = NOT_FOUND_DEVICE;
			return lan_result->nResult;
		}
	}
}

int	TOB_WIFI_testing(WIFI_RESULT *wifi_result){
	wifi_result->nResult = NOT_FOUND_DEVICE;
	bool bFind = false;
	char szResult[MAX_PATH] = {0};
	FILE *fp = NULL;
	fp = popen("iwconfig wlan0", "r");
	while(fgets(szResult, MAX_PATH, fp)){

		if(strstr(szResult, "Tx-Power")){
			if(strstr(szResult, "dBm")){
				wifi_result->nResult = OK;
				break;
			}
		}
	}
	pclose(fp);

	return wifi_result->nResult;
}

int	TOB_BT_testing(BT_RESULT *bt_result){
	bt_result->nResult = NOT_FOUND_DEVICE;
	char szResult[MAX_PATH] = {0};
	FILE *fp = NULL;
	fp = popen("rfkill unblock 0", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	sleep(1);
	fp = popen("hcitool dev", "r");
	while(fgets(szResult, MAX_PATH, fp)){
		if(!strstr(szResult, "hci"))
			continue;

		char szMAC[MAX_PATH] = {0};
		fgets(szResult, MAX_PATH, fp);
		if(strstr(szResult, "hci")){
			bt_result->nResult = OK;
			strncpy(szMAC, strstr(szResult, ":") - 2, 17);
			sprintf(bt_result->szMACNum, "%s", szMAC);
		}
	}
	pclose(fp);

	fp = popen("rfkill block 0", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);

	return bt_result->nResult;
}

void append(char *s, char c){
	int len = strlen(s);
	s[len] = c;
	s[len+1] = '\0';
}

void splitSDCardDev(char *s, char *r)
{
	char result[MAX_PATH] = {0};
	size_t string_len = strlen(s);
	char Disk_head = strstr(s, "Disk");
	size_t Disk_head_len = strlen("Disk");
	if (Disk_head != NULL){
		for (int i=Disk_head_len+1;i<string_len;i++){
			if (s[i] != ':'){
				append(result, s[i]);
			}
			else
				break;
		}	
	}
	else{
		for (int i=0;i<string_len;i++){
			if (s[i] != ' '){
				append(result, s[i]);
			}
			else
				break;		
		}	
	}

	strcpy(r,result);
	return;
}

int TOB_SDCARD_testing(SDCARD_CFG *sdcard_cfg, SDCARD_RESULT *sdcard_result){
	char szCMD[MAX_CMD] = {0};
	char szDev[MAX_CMD] = {0};
	FILE *fp = NULL;
	bool bIsRoot = false;
	struct timeval start_time = {0};
	struct timeval end_time = {0};
	unsigned long long ms_start_time = 0;
	unsigned long long ms_end_time = 0;
	sprintf(szCMD, "sudo fdisk -l | grep -v \"mmcblk0\" | grep \"mmcblk\"");
	fp = popen(szCMD, "r");
	memset(szCMD, 0, MAX_CMD);
	while((fgets(szCMD, MAX_CMD, fp)) != NULL)
	{
		if ( !strstr(szCMD, "mmcblk0") && strstr(szCMD, "mmcblk")){
			splitSDCardDev(szCMD, szDev);		
		}
	}
	pclose(fp);
	
	if (strlen(szDev) == 0){
		return NOT_FOUND_DEVICE;	
	} 

	sdcard_cfg->pszDevPath = szDev;

	fp = popen("df -T $PWD", "r");
	while(fgets(szCMD, MAX_CMD, fp) != NULL){
		if(strstr(szCMD, sdcard_cfg->pszDevPath) != NULL){
			bIsRoot = true;
		}
	}
	pclose(fp);

	if(!bIsRoot){
		sprintf(szCMD, "mount %s %s", szDev, "/mnt");
		SystemCMD(szCMD);
	}

	gettimeofday(&start_time, NULL);
	ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

	sprintf(szCMD, "dd if=/dev/zero of=%s/%s conv=fdatasync bs=10M count=10", "/mnt", "100M.dd");
	SystemCMD(szCMD);

	gettimeofday(&end_time, NULL);
	ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

	sprintf(szCMD, "rm %s/%s", "/mnt", "100M.dd");
	SystemCMD(szCMD);

	if(!bIsRoot){
		sprintf(szCMD, "umount %s", "/mnt");
		SystemCMD(szCMD);
	}

	sdcard_result->nResult = OK;
	sdcard_result->nTimeCost_ms = ms_end_time - ms_start_time;
  sdcard_result->nTransferSpeed = 100/((ms_end_time - ms_start_time)/1000.0);

	return OK;
}

void splitDisk(char *s, char *r)
{
	char result[MAX_PATH] = {0};
	size_t string_len = strlen(s);

	for (int i=0;i<string_len;i++){
		if (s[i] != ' ' && s[i] != ':'){
			append(result, s[i]);
		}
		else
			break;		
	}	

	strcpy(r,result);
	return;
}

int TOB_MSATA_testing(MSATA_CFG *msata_cfg, MSATA_RESULT *msata_result){
	char szPath[MAX_PATH] = {0};
	char szCMD[MAX_CMD] = {0};
	char szDisk[MAX_PATH] = {0};
	char sata[MAX_PATH] = {0};
	bool bFind = false;
	struct timeval start_time = {0};
	struct timeval end_time = {0};
	unsigned long long ms_start_time = 0;
	unsigned long long ms_end_time = 0;
	FILE *fp = NULL;
	switch(GetDts()){
		case AT017:{
			FILE *pFile = popen("blkid", "r");
			while(fgets(szCMD, MAX_CMD, pFile)){
				if(!strstr(szCMD, "mmcblk") && !strstr(szCMD, "/dev/loop") && (strstr(szCMD, "fat") || strstr(szCMD, "ext") || strstr(szCMD, "ntfs"))){
					char szPath[MAX_PATH] = {0};
					strncpy(szPath, szCMD, strlen(szCMD) - strlen(strstr(szCMD, ":")));
					if(szPath){
						char szResult[MAX_PATH] = {0};
						sprintf(szCMD, "udevadm info --query=path --name=%s", szPath);
						
						FILE *fp = popen(szCMD, "r");
						if(fp){
							fgets(szResult, MAX_PATH, fp);
							int nType = 0;
							int nPortPosition = -1;

							if(strstr(szResult, "usb2/2-3/2-3.1")){
								nType = USB_3_0;									
								// nPortPosition = strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB3DEV) ? UP : DOWN;
								bFind = true;
							}
							else if(strstr(szResult, "usb2/2-1")){
								nType = USB_3_0;									
								// nPortPosition = strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB3DEV) ? UP : DOWN;
								bFind = true;
							}
						}
						pclose(fp);
					}
				}
			}
			pclose(pFile);
			if(!bFind)
				return NOT_FOUND_DEVICE;
			sprintf(szCMD, "mount %s %s", "/dev/sdb1", "/mnt");
			SystemCMD(szCMD);

			gettimeofday(&start_time, NULL);
			ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

			sprintf(szCMD, "dd if=/dev/zero of=%s/%s conv=fdatasync bs=10M count=10", "/mnt", "100M.dd");
			SystemCMD(szCMD);

			gettimeofday(&end_time, NULL);
			ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

			sprintf(szCMD, "rm %s/%s", "/mnt", "100M.dd");
			SystemCMD(szCMD);

			sprintf(szCMD, "umount %s", "/mnt");
			SystemCMD(szCMD);

			msata_result->nResult = OK;
			msata_result->nTimeCost_ms = ms_end_time - ms_start_time;
			msata_result->nTransferSpeed = 100/((ms_end_time - ms_start_time)/1000.0);

			return OK;
		}				

		default:{
			fp = popen("blkid | grep -v \"mmcblk\"", "r");
			while (fgets(szCMD, MAX_CMD, fp) != NULL){
				splitDisk(szCMD, szPath);
				if(szPath){
					sprintf(szCMD, "udevadm info --query=path --name=%s", szPath);
					FILE *pFile = popen(szCMD, "r");
					while(fgets(szCMD, MAX_CMD, pFile) != NULL){
						if(strstr(szCMD, "achi") || strstr(szCMD, "ata")){
							bFind = true;
							break;
						}
					}
					pclose(pFile);
				}
			}
			pclose(fp);

			if(!bFind)
				return NOT_FOUND_DEVICE;

			sprintf(szCMD, "mount %s %s", szPath, "/mnt");
			SystemCMD(szCMD);

			gettimeofday(&start_time, NULL);
			ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

			sprintf(szCMD, "dd if=/dev/zero of=%s/%s conv=fdatasync bs=10M count=10", "/mnt", "100M.dd");
			SystemCMD(szCMD);

			gettimeofday(&end_time, NULL);
			ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

			sprintf(szCMD, "rm %s/%s", "/mnt", "100M.dd");
			SystemCMD(szCMD);

			sprintf(szCMD, "umount %s", "/mnt");
			SystemCMD(szCMD);

			msata_result->nResult = OK;
			msata_result->nTimeCost_ms = ms_end_time - ms_start_time;
			msata_result->nTransferSpeed = 100/((ms_end_time - ms_start_time)/1000.0);

			return OK;
		}
	}
}

int TOB_MPCIE_testing(MPCIE_CFG *mpcie_cfg, MPCIE_RESULT *mpcie_result){
	char szCMD[MAX_CMD] = {0};
	FILE *fp = NULL;
	bool bIsPCIEenable = false;

	fp = popen("cat /sys/module/tegra_fuse/parameters/tegra_chip_id", "r");
	fgets(szCMD, MAX_CMD, fp);
	pclose(fp);

	fp = popen("lspci", "r");
	//while(fgets(szCMD, MAX_CMD, fp) != NULL && strstr(szCMD, "PCI bridge: NVIDIA Corporation Device") != NULL){
	int i=0;
	while(fgets(szCMD, MAX_CMD, fp) != NULL){
		switch(mpcie_cfg->nMPCIE_type){
			case STANDARD:
				bIsPCIEenable = true;
				break;
			case DUALMPCIE:
				if(strstr(szCMD, "01:00.0") != NULL)
					i++;
					//mpciePortPosition = 1;
				if(strstr(szCMD, "02:00.0") != NULL)
					i++;
					//mpciePortPosition = 2;
				if( i == 2 )
					bIsPCIEenable = true;
				break;
			case HYBRID:
				switch(GetModuleType()){
					// case TX1:
					// 	if(strstr(szCMD, "00:02.0") != NULL)
					// 		bIsPCIEenable = true;
					// 	break;
					case TX2:
						if(strstr(szCMD, "00:03.0") != NULL)
							bIsPCIEenable = true;
						break;
				}
				break;
		}
		//printf("%d\n",i);
	}
	pclose(fp);

	if(!bIsPCIEenable){
		return NOT_FOUND_DEVICE;
	}

	return OK;
}

int TOB_RS232_testing(RS232_CFG *rs232_cfg, RS232_RESULT *rs232_result){
	return serial_test(rs232_cfg->pszDeviceName, rs232_cfg->pszTestingData);
}

int TOB_GPIO_Testing(GPIO_CFG *gpio_cfg, GPIO_RESULT *gpio_result){
	char szMsg[MAX_PATH] = {0};
	char szCMD[MAX_PATH] = {0};	
	char szResult[10] = {0};
	FILE *fp = NULL;
	int i = 0;
	
	for(i = 0 ; i < gpio_cfg->nGPIO_Pins_Number ; i++){
		int nGPIO_Number = gpio_cfg->pGPIO_Pins[i];
		gpio_result->nPinResults[i] = FAILED;
		printf("GPIO[%d]-Pin[%d] Testing...\n", i, gpio_result->nPinResults[i]);
		
		sprintf(szCMD, "echo %d > /sys/class/gpio/export", nGPIO_Number);
		SystemCMD(szCMD);

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "echo \"out\" > /sys/class/gpio/gpio%d/direction", nGPIO_Number);
		SystemCMD(szCMD);

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "echo 1 > /sys/class/gpio/gpio%d/value", nGPIO_Number);
		SystemCMD(szCMD);

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nGPIO_Number);		
		fp = popen(szCMD, "r");
		fgets(szResult, 10, fp);
		pclose(fp);
		if(atoi(szResult) != 1){
			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nGPIO_Number);
			SystemCMD(szCMD);
			gpio_result->nResult = FAILED;
			continue;
		}

		sleep(5);


		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "echo 0 > /sys/class/gpio/gpio%d/value", nGPIO_Number);
		SystemCMD(szCMD);
		switch(GetDts()){
			case AT017:		
				if (nGPIO_Number == 79 ||nGPIO_Number ==  445){
					sleep(5);
					printf("it is pin 79\n");
				}
				break;
			break;
		}		
		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nGPIO_Number);
		memset(szResult, 0, sizeof(char) * 10);
		fp = popen(szCMD, "r");
		fgets(szResult, 10, fp);
		pclose(fp);



		if(atoi(szResult) != 0){
			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nGPIO_Number);
			SystemCMD(szCMD);
			gpio_result->nResult = FAILED;
			continue;
		}

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nGPIO_Number);		
		SystemCMD(szCMD);

		gpio_result->nPinResults[i] = OK;
		
	}

	return OK;
}

int TOB_UART_Testing(UART_CFG *uart_cfg, UART_RESULT *uart_result){
	return serial_test(uart_cfg->pszDeviceName, uart_cfg->pszTestingData);
}

int TOB_UART2_Testing(UART2_CFG *uart2_cfg, UART2_RESULT *uart2_result){
	return serial_test(uart2_cfg->pszDeviceName, uart2_cfg->pszTestingData);
}

int TOB_UART_CSI_Testing(UART_CSI_CFG *uart_CSI_cfg, UART_CSI_RESULT *uart_CSI_result){
	return serial_test(uart_CSI_cfg->pszDeviceName, uart_CSI_cfg->pszTestingData);
}

int	TOB_FAN_Testing(FAN_CFG *fan_cfg, FAN_RESULT *fan_result){

	char szPath[MAX_PATH] = {0}, szCMD[MAX_CMD] = {0}, szResult[10] = {0}, szMsg[MAX_PATH] = {0};
	FILE *fp = NULL;

	int nFAN_Number = fan_cfg->nFAN_Number;

	switch(GetModuleType()){
		case XAVIER_NX:
			switch(GetDts()){
				case AN110:

					printf("Fan Testing...\n\n");

					sprintf(szCMD, "echo %d > /sys/class/gpio/export", nFAN_Number);
					SystemCMD(szCMD);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "echo \"out\" > /sys/class/gpio/gpio%d/direction", nFAN_Number);
					SystemCMD(szCMD);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "echo 0 > /sys/class/gpio/gpio%d/value", nFAN_Number);
					SystemCMD(szCMD);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nFAN_Number);
					memset(szResult, 0, sizeof(char) * 10);
					fp = popen(szCMD, "r");
					fgets(szResult, 10, fp);
					pclose(fp);
					if(atoi(szResult) != 0){
						memset(szCMD, 0, sizeof(char) * MAX_PATH);
						sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nFAN_Number);
						SystemCMD(szCMD);
						return FAILED;
					}

					sleep(3);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "echo \"out\" > /sys/class/gpio/gpio%d/direction", nFAN_Number);
					SystemCMD(szCMD);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "echo 1 > /sys/class/gpio/gpio%d/value", nFAN_Number);
					SystemCMD(szCMD);

					memset(szCMD, 0, sizeof(char) * MAX_PATH);
					sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nFAN_Number);		
					fp = popen(szCMD, "r");
					fgets(szResult, 5, fp);
					pclose(fp);
					if(atoi(szResult) != 1){
						memset(szCMD, 0, sizeof(char) * MAX_PATH);
						sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nFAN_Number);
						SystemCMD(szCMD);
						return FAILED;
					};		
					return OK;		
				break;
			}

		default:

		sprintf(szPath, "/sys/devices/pwm-fan/target_pwm");

		sprintf(szCMD, "echo %d > %s", fan_cfg->nValue, szPath);
		SystemCMD(szCMD);

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "cat %s", szPath);
		memset(szResult, 0, sizeof(char) * 10);
		fp = popen(szCMD, "r");
		fgets(szResult, 10, fp);
		pclose(fp);
		if(atoi(szResult) != fan_cfg->nValue){
			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo 0 > %s", szPath);
			SystemCMD(szCMD);
			return FAILED;
		}
		
		sleep(5);

		sprintf(szCMD, "echo 0 > %s", szPath);
		SystemCMD(szCMD);

		memset(szCMD, 0, sizeof(char) * MAX_PATH);
		sprintf(szCMD, "cat %s", szPath);
		memset(szResult, 0, sizeof(char) * 10);
		fp = popen(szCMD, "r");
		fgets(szResult, 10, fp);
		pclose(fp);	
		if(atoi(szResult) != 0){
			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo 0 > %s", szPath);
			SystemCMD(szCMD);
			return FAILED;
		}
		return OK;

		break;
	}	
}

int	TOB_EDP_Testing(EDP_CFG *eDP_cfg, EDP_RESULT *eDP_result){

	char szPath[MAX_PATH] = {0}, szCMD[MAX_CMD] = {0}, szResult[10] = {0}, szMsg[MAX_PATH] = {0};
	FILE *fp = NULL;

	int nEDP_Number = eDP_cfg->nEDP_Number;
	
	switch(GetModuleType()){
		case XAVIER_NX:

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo \"out\" > /sys/class/gpio/gpio%d/direction", nEDP_Number);
			SystemCMD(szCMD);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo 1 > /sys/class/gpio/gpio%d/value", nEDP_Number);
			SystemCMD(szCMD);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nEDP_Number);		
			fp = popen(szCMD, "r");
			fgets(szResult, 5, fp);
			pclose(fp);
			if(atoi(szResult) != 1){
				memset(szCMD, 0, sizeof(char) * MAX_PATH);
				sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nEDP_Number);
				SystemCMD(szCMD);
				return FAILED;
			}

			sleep(3);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo 0 > /sys/class/gpio/gpio%d/value", nEDP_Number);
			SystemCMD(szCMD);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "cat /sys/class/gpio/gpio%d/value", nEDP_Number);
			memset(szResult, 0, sizeof(char) * 10);
			fp = popen(szCMD, "r");
			fgets(szResult, 10, fp);
			pclose(fp);
			if(atoi(szResult) != 0){
				memset(szCMD, 0, sizeof(char) * MAX_PATH);
				sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nEDP_Number);
				SystemCMD(szCMD);
				return FAILED;
			}

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "echo %d > /sys/class/gpio/unexport", nEDP_Number);		
			SystemCMD(szCMD);
		
			return OK;
		break;

		case NANO:

			sprintf(szPath, "/sys/devices/pwm-fan/target_pwm");

			sprintf(szCMD, "echo %d > %s", eDP_cfg->nValue, szPath);
			SystemCMD(szCMD);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "cat %s", szPath);
			memset(szResult, 0, sizeof(char) * 10);
			fp = popen(szCMD, "r");
			fgets(szResult, 10, fp);
			pclose(fp);
			if(atoi(szResult) != eDP_cfg->nValue){
				memset(szCMD, 0, sizeof(char) * MAX_PATH);
				sprintf(szCMD, "echo 0 > %s", szPath);
				SystemCMD(szCMD);
				return FAILED;
			}
			
			sleep(5);

			sprintf(szCMD, "echo 0 > %s", szPath);
			SystemCMD(szCMD);

			memset(szCMD, 0, sizeof(char) * MAX_PATH);
			sprintf(szCMD, "cat %s", szPath);
			memset(szResult, 0, sizeof(char) * 10);
			fp = popen(szCMD, "r");
			fgets(szResult, 10, fp);
			pclose(fp);	
			if(atoi(szResult) != 0){
				memset(szCMD, 0, sizeof(char) * MAX_PATH);
				sprintf(szCMD, "echo 0 > %s", szPath);
				SystemCMD(szCMD);
				return FAILED;
			}

			return OK;
		break;
	
	}
}

static bool g_bReceived = false;
void *CANDUMP_Thread(void *arg){
	char szCMD[MAX_CMD] = {0}, szResult[MAX_PATH] = {0};
	CAN_CFG *can_cfg = (CAN_CFG*)arg;
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "timeout 3 candump %s", can_cfg->pszReceiveCAN);
	FILE *fp = popen(szCMD, "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	
	if(strstr(szResult, can_cfg->pszId) != NULL)
		g_bReceived = true;

	return;
}

int TOB_CANBus_Testing(CAN_CFG *can_cfg, CAN_RESULT *can_result){
	char szCMD[MAX_CMD] = {0};
	pthread_t pt_Thread;
	void *retThread = NULL;

	g_bReceived = false;

	sprintf(szCMD, "[Send]: %s, [Receive]: %s, [Id]: %s, [Data]: %s", can_cfg->pszSendCAN, can_cfg->pszReceiveCAN, can_cfg->pszId, can_cfg->pszData);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ifconfig %s down", can_cfg->pszSendCAN);
	SystemCMD(szCMD);

	sleep(0.5);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ifconfig %s down", can_cfg->pszReceiveCAN);
	SystemCMD(szCMD);

	sleep(0.5);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ip link set %s up type can bitrate 1000000", can_cfg->pszSendCAN);
	SystemCMD(szCMD);

	sleep(0.5);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ip link set %s up type can bitrate 1000000", can_cfg->pszReceiveCAN);
	SystemCMD(szCMD);

	sleep(0.5);

	pthread_create(&pt_Thread, NULL, CANDUMP_Thread, (void*)can_cfg);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "cansend %s %s#%s", can_cfg->pszSendCAN, can_cfg->pszId, can_cfg->pszData);
	SystemCMD(szCMD);
	sleep(0.3);
	SystemCMD(szCMD);
	sleep(0.3);
	SystemCMD(szCMD);

	pthread_join(pt_Thread, retThread);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ifconfig %s down", can_cfg->pszReceiveCAN);
	SystemCMD(szCMD);

	sleep(0.5);

	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "ifconfig %s down", can_cfg->pszSendCAN);
	SystemCMD(szCMD);

	return g_bReceived ? OK : FAILED;
}

int TOB_I2C_Testing(I2C_CFG *i2c_cfg, I2C_RESULT *i2c_result){

	char szResult[5] = {0}, szCMD[MAX_CMD] = {0}, szTemp[MAX_PATH] = {0};
	bool bNoDevice = false;
	
	sprintf(szCMD, "i2cdump -f -y %d 0x%02x", i2c_cfg->nI2C_ID, i2c_cfg->nAddress);

	FILE *fp = popen(szCMD, "r");
	if(fp){
		while(fgets(szTemp, MAX_PATH, fp)){
			if(strstr(szTemp, "00:") && strstr(szTemp, "XX")){
				bNoDevice = true;
				break;
			}
		}
	}
	pclose(fp);

	if(bNoDevice){
		i2c_result->nResult = NOT_FOUND_DEVICE;
		return NOT_FOUND_DEVICE;
	}
	
	sleep(1);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0xff", i2c_cfg->nI2C_ID, i2c_cfg->nAddress);
	if(SystemCMD(szCMD) < 0)
		return FAILED;

	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0x%02x", i2c_cfg->nI2C_ID, i2c_cfg->nAddress, i2c_cfg->nData);
	if(SystemCMD(szCMD) < 0)
		return FAILED;
	
	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cget -f -y %d 0x%02x 0x00", i2c_cfg->nI2C_ID, i2c_cfg->nAddress);
	fp = popen(szCMD, "r");
	fgets(szResult, 5, fp);
	pclose(fp);

	sprintf(szCMD, "szResult : %s, szResult : 0x%02x, i2c_cfg->nData : 0x%02x", szResult, strtol(szResult, NULL, 16), i2c_cfg->nData);
	
	return strtol(szResult, NULL, 16) == i2c_cfg->nData ? OK : FAILED;
}

int TOB_I2C_II_Testing(I2C_II_CFG *i2c_II_cfg, I2C_II_RESULT *i2c_II_result){

	char szResult[5] = {0}, szCMD[MAX_CMD] = {0}, szTemp[MAX_PATH] = {0};
	bool bNoDevice = false;
	
	sprintf(szCMD, "i2cdump -f -y %d 0x%02x", i2c_II_cfg->nI2C_ID, i2c_II_cfg->nAddress);
	FILE *fp = popen(szCMD, "r");
	if(fp){
		while(fgets(szTemp, MAX_PATH, fp)){
			if(strstr(szTemp, "00:") && strstr(szTemp, "XX")){
				bNoDevice = true;
				break;
			}
		}
	}
	pclose(fp);

	if(bNoDevice){
		i2c_II_result->nResult = NOT_FOUND_DEVICE;
		return NOT_FOUND_DEVICE;
	}
	
	sleep(1);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0xff", i2c_II_cfg->nI2C_ID, i2c_II_cfg->nAddress);
	if(SystemCMD(szCMD) < 0)
		return FAILED;

	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0x%02x", i2c_II_cfg->nI2C_ID, i2c_II_cfg->nAddress, i2c_II_cfg->nData);
	if(SystemCMD(szCMD) < 0)
		return FAILED;
	
	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cget -f -y %d 0x%02x 0x00", i2c_II_cfg->nI2C_ID, i2c_II_cfg->nAddress);
	fp = popen(szCMD, "r");
	fgets(szResult, 5, fp);
	pclose(fp);

	sprintf(szCMD, "szResult : %s, szResult : 0x%02x, i2c_cfg->nData : 0x%02x", szResult, strtol(szResult, NULL, 16), i2c_II_cfg->nData);
	
	return strtol(szResult, NULL, 16) == i2c_II_cfg->nData ? OK : FAILED;
}

int TOB_I2C_CSI_Testing(I2C_CSI_CFG *i2c_CSI_cfg, I2C_CSI_RESULT *i2c_CSI_result){

	char szResult[5] = {0}, szCMD[MAX_CMD] = {0}, szTemp[MAX_PATH] = {0};
	bool bNoDevice = false;
       	
	sprintf(szCMD,"i2cset -f -y 0 0x77 0x3 0xfb");
	if(SystemCMD(szCMD) < 0){
		printf("poweron-failed\n");}
	else{ 
		printf("poweron\n");}
	
	sprintf(szCMD, "i2cdump -f -y %d 0x%02x", i2c_CSI_cfg->nI2C_ID, i2c_CSI_cfg->nAddress);
        //printf("I2C_ID:%d\n",i2c_CSI_cfg->nI2C_ID);
        //printf("nAddress:%d\n",i2c_CSI_cfg->nAddress);
        //printf("szCMD:=======%s\n", szCMD);
	FILE *fp = popen(szCMD, "r");
	if(fp){
		printf("fp is true\n");
		while(fgets(szTemp, MAX_PATH, fp)){
			if(strstr(szTemp, "00:") && strstr(szTemp, "XX")){
				bNoDevice = true;
				printf("bNodevice=true\n");
				break;
			}
		}
	}
	pclose(fp);

	if(bNoDevice){
		i2c_CSI_result->nResult = NOT_FOUND_DEVICE;
		return NOT_FOUND_DEVICE;
	}
	
	sleep(1);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0xff", i2c_CSI_cfg->nI2C_ID, i2c_CSI_cfg->nAddress);
	if(SystemCMD(szCMD) < 0){
		return FAILED;
	};
	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cset -f -y %d 0x%02x 0x00 0x%02x", i2c_CSI_cfg->nI2C_ID, i2c_CSI_cfg->nAddress, i2c_CSI_cfg->nData);
	if(SystemCMD(szCMD) < 0)
		return FAILED;
	
	sleep(1);
	memset(szCMD, 0, MAX_CMD);
	sprintf(szCMD, "i2cget -f -y %d 0x%02x 0x00", i2c_CSI_cfg->nI2C_ID, i2c_CSI_cfg->nAddress);
	fp = popen(szCMD, "r");
	fgets(szResult, 5, fp);
	pclose(fp);
	sprintf(szCMD,"i2cset -f -y 0 0x77 0x3 0xf9");
	if(SystemCMD(szCMD) < 0){
		printf("power off Failed\n");}
	else{ 
		printf("poweroff\n");
	}
	sprintf(szCMD, "szResult : %s, szResult : 0x%02x, i2c_CSI_cfg->nData : 0x%02x", szResult, strtol(szResult, NULL, 16), i2c_CSI_cfg->nData);
       
	
	return strtol(szResult, NULL, 16) == i2c_CSI_cfg->nData ? OK : FAILED;
	
	
}

int TOB_SPI_testing(SPI_CFG *spi_cfg, SPI_RESULT *spi_result){
	return spi_test(spi_cfg->pszDeviceName);
}

int TOB_Sensors_Testing(SENSORS_CFG *sensors_cfg, SENSORS_RESULT *sensors_result){
	return OK;
}

int TOB_M2_MKEY_Testing(M2_MKEY_CFG *mkey_cfg,M2_MKEY_RESULT *mkey_result){
	char szResult[MAX_PATH] = {0};
	char szPath[MAX_PATH] = {0};
	char szCMD[MAX_CMD] = {0};
	struct timeval start_time = {0};
	struct timeval end_time = {0};
	unsigned long long ms_start_time = 0;
	unsigned long long ms_end_time = 0;
	int nResult = FAILED;
	bool bFind = false;
	int Count = 0;
	FILE *pfile = popen("blkid | grep -v \"mmcblk\" | grep -v \"loop\"", "r");
	while(fgets(szResult, MAX_PATH, pfile)){
		if(!bFind){
			if((strstr(szResult, "fat") || strstr(szResult, "ext") || strstr(szResult, "ntfs"))){
				if (mkey_cfg->nDeviceType == PCIE){
					switch(GetDts()){
						case T012:
							if(strstr(szResult, "nvme0")){
								strncpy(szPath, szResult, strlen(szResult) - strlen(strstr(szResult, ":")));
								printf("Found PCIE Disk:%s\n",szPath);
								
								sprintf(szCMD, "cat /etc/mtab | grep %s", szPath);
								FILE *p_check_mount = popen("cat /etc/mtab", "r");
								while(fgets(szResult, MAX_PATH, p_check_mount)){
									if (strstr(szResult, szPath)){
										sprintf(szCMD, "umount %s", szPath);
										SystemCMD(szCMD);
									}
								}
								pclose(p_check_mount);

								sprintf(szCMD, "mount %s %s", szPath, "/mnt");

								gettimeofday(&start_time, NULL);
								ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

								sprintf(szCMD, "dd if=/dev/zero of=%s/100M conv=fdatasync bs=10M count=10", "/mnt");
								SystemCMD(szCMD);

								gettimeofday(&end_time, NULL);
								ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

								memset(szCMD, 0, MAX_CMD);
								sprintf(szCMD, "rm %s/100M", "/mnt");
								SystemCMD(szCMD);

								Count += 1;
							}
							if (strstr(szResult, "nvme1")){
								strncpy(szPath, szResult, strlen(szResult) - strlen(strstr(szResult, ":")));
								Count += 1;
							}
							if(Count ==2){
								logINFO("Found two Devices\n");
								bFind = true;
							}
						break;

						default:
							if (strstr(szResult, "nvme")){
								strncpy(szPath, szResult, strlen(szResult) - strlen(strstr(szResult, ":")));
								bFind = true;
							}
						break;
					}	
				if (bFind){
						break;
					}
					
				}
				else{
					memset(szPath, 0, sizeof(szPath));
					strncpy(szPath, szResult, strlen(szResult) - strlen(strstr(szResult, ":")));
					sprintf(szCMD, "udevadm info --query=all --name=%s", szPath);
					FILE *p_udevInfo = popen(szCMD, "r");
					while(fgets(szResult, MAX_PATH, p_udevInfo)){
						
						if ((strstr(szResult, "SATA"))){
							//printf("%s\n",szResult);
							bFind = true;
							break;
						}
					}
					
					pclose(p_udevInfo);
				}
			}
		}
		else{
			break;
		}
	}
	pclose(pfile);

	if(bFind){
		printf("Found Disk:%s\n",szPath);
		sprintf(szCMD, "cat /etc/mtab | grep %s", szPath);
		FILE *p_check_mount = popen("cat /etc/mtab", "r");
		while(fgets(szResult, MAX_PATH, p_check_mount)){
			if (strstr(szResult, szPath)){
				sprintf(szCMD, "umount %s", szPath);
				SystemCMD(szCMD);
			}
		}
		pclose(p_check_mount);

		sprintf(szCMD, "mount %s %s", szPath, "/mnt");

		gettimeofday(&start_time, NULL);
		ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

		sprintf(szCMD, "dd if=/dev/zero of=%s/100M conv=fdatasync bs=10M count=10", "/mnt");
		SystemCMD(szCMD);

		gettimeofday(&end_time, NULL);
		ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

		memset(szCMD, 0, MAX_CMD);
		sprintf(szCMD, "rm %s/100M", "/mnt");
		SystemCMD(szCMD);

		mkey_result->nResult = OK;
		mkey_result->nTimeCost_ms = ms_end_time - ms_start_time;
		mkey_result->nTransferSpeed = 100/((ms_end_time - ms_start_time)/1000.0);
	}
	else{
		return NOT_FOUND_DEVICE;
	}

	return OK;
}

int TOB_M2_EKEY_Testing(M2_EKEY_CFG *ekey_cfg, M2_EKEY_RESULT *ekey_result){
	char szResult[MAX_PATH] = {0};
	char szCMD[MAX_CMD] = {0};
	int nResult = FAILED;
	int wifi_result = FAILED;
	int bt_result = FAILED;
	/*sprintf(szCMD, "lspci |grep \"Network controller\"");
	FILE *fp = popen(szCMD, "r");

	while(fgets(szResult, MAX_PATH, fp) != NULL && strstr(szResult, ekey_cfg->pszKeyword) != NULL){
			printf("Find Intel Ekey\n");		
			nResult = OK;
			break;
	}	
	pclose(fp);*/
	FILE *fp = NULL;
	/*
	fp = popen("nmcli d", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	sleep(1);
	fp = popen("nmcli d wifi connect Aetina_Guest password At77092568", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	sleep(5);*/


/*------wifi-----------*/
	fp = popen("iwconfig wlan0", "r");
	while(fgets(szResult, MAX_PATH, fp)){

		if(strstr(szResult, "Tx-Power")){
			if(strstr(szResult, "dBm")){
				wifi_result = OK;
				break;
			}
		}
	}
	pclose(fp);
/*--------BT-----------*/
	fp = popen("rfkill unblock 0", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);
	sleep(1);
	fp = popen("hcitool dev", "r");
	while(fgets(szResult, MAX_PATH, fp)){
		if(!strstr(szResult, "hci"))
			continue;

		char szMAC[MAX_PATH] = {0};
		fgets(szResult, MAX_PATH, fp);
		if(strstr(szResult, "hci")){
			bt_result = OK;
			strncpy(szMAC, strstr(szResult, ":") - 2, 17);
		}
	}
	pclose(fp);

	/*fp = popen("rfkill block 0", "r");
	fgets(szResult, MAX_PATH, fp);
	pclose(fp);*/

	if (wifi_result == OK && bt_result ==OK){
		nResult = OK;
	}

	if (wifi_result == FAILED){
		logFAILED("WIFI Failed\n");
	}
	else if (wifi_result == OK){
		logPASS("WIFI PASS\n");
	}

	if (bt_result == FAILED){
		logFAILED("BT Failed\n");
	}
	else if (bt_result == OK){
		logPASS("BT PASS\n");
	}


	return nResult;
}

int TOB_M2_BKEY_Testing(M2_BKEY_CFG *bkey_cfg, M2_BKEY_RESULT *bkey_result){
	char szResult[MAX_PATH] = {0};
	char szCMD[MAX_CMD] = {0};
	int nResult = FAILED;
	switch(GetModuleType()){
		case XAVIER_NX:
			switch(GetDts()){
				case AN810:
					//sprintf(szCMD, "i2cset -f -y 0 0x77 0x06 0xfe");
					SystemCMD("i2cset -f -y 0 0x77 0x06 0xde");
					//memset(szCMD, 0, MAX_CMD);
					sleep(2);
					//sprintf(szCMD, "i2cset -f -y 0 0x77 0x06 0xf6");
					SystemCMD("i2cset -f -y 0 0x77 0x06 0xd6");
					//memset(szCMD, 0, MAX_CMD);
					
					printf("Wait for device Warm up...\n");
					sleep(15);
					break;
			}
		break;
	}

	sprintf(szCMD, "lsusb |grep \"Telit\"");
	FILE *fp = popen(szCMD, "r");
	while(fgets(szResult, MAX_PATH, fp) != NULL && strstr(szResult, bkey_cfg->pszKeyword) != NULL){	
			nResult = OK;
			break;
	}	
	pclose(fp);

	return nResult;
}

int TOB_USB_Testing(USB_CFG *usb_cfg, USB_RESULT *usb_result){
	char szCMD[MAX_CMD] = {0};	
	char szMsg[MAX_PATH] = {0};
	char szDevPath[MAX_PATH][MAX_PATH] = {0};
	struct timeval start_time = {0};
	struct timeval end_time = {0};
	unsigned long long ms_start_time = 0;
	unsigned long long ms_end_time = 0;

	FILE *pFile = popen("blkid", "r");
	while(fgets(szCMD, MAX_CMD, pFile)){
		if(!strstr(szCMD, "mmcblk") && !strstr(szCMD, "/dev/loop") && (strstr(szCMD, "fat") || strstr(szCMD, "ext") || strstr(szCMD, "ntfs"))){
			char szPath[MAX_PATH] = {0};
			strncpy(szPath, szCMD, strlen(szCMD) - strlen(strstr(szCMD, ":")));
			if(szPath){
				char szResult[MAX_PATH] = {0};
				sprintf(szCMD, "udevadm info --query=path --name=%s", szPath);
				
				FILE *fp = popen(szCMD, "r");
				if(fp){
					fgets(szResult, MAX_PATH, fp);
					bool bFind = false;
					int nType = 0;
					int nPortPosition = -1;
					switch(usb_cfg->nInterfaceType){
						case USB_3_0_TYPE_A:
							{
								//USB 3.0
								if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB3DEV) || strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_DOWN_USB3DEV)){
									nType = USB_3_0;									
									// nPortPosition = strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB3DEV) ? UP : DOWN;
									bFind = true;
									if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB3DEV)){
										usb_result->nUsbFoundDevice = usb_result->nUsbFoundDevice | USB_3_0_TYPE_A_UP_FOUND;
									}
									if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_DOWN_USB3DEV)){
										usb_result->nUsbFoundDevice = usb_result->nUsbFoundDevice | USB_3_0_TYPE_A_DOWN_FOUND;	
									}
								}
								//USB 2.0
								else if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB2DEV) || strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_DOWN_USB2DEV)){
									nType = USB_2_0;									
									// nPortPosition = strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB2DEV) ? UP : DOWN;z
									bFind = true;
									if (strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_UP_USB2DEV)){
										usb_result->nUsbFoundDevice = usb_result->nUsbFoundDevice | USB_2_0_TYPE_A_UP_FOUND;
									}
									if (strstr(szResult, usb_cfg->pUSB_3_0_TYPE_A_DOWN_USB2DEV)){
										usb_result->nUsbFoundDevice = usb_result->nUsbFoundDevice | USB_2_0_TYPE_A_DOWN_FOUND;
									}
								}
							}
							break;

						case USB_3_0_TYPE_C:
							{
								//USB 3.0
								if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_C_USB3DEV)){
									nType = USB_3_0;
									bFind = true;
								}
								//USB 2.0
								else if(strstr(szResult, usb_cfg->pUSB_3_0_TYPE_C_USB2DEV)){
									nType = USB_2_0;
									bFind = true;
								}								
							}
							break;

						case MICRO_USB:
							{
								if(strstr(szResult, usb_cfg->pMICRO_USB_DEV)){
									nType = USB_2_0;
									bFind = true;
								}
							}
							break;	
					}

					if(bFind){
						usb_result->nResultNum++;
						usb_result->pUSBResults    = realloc(usb_result->pUSBResults, usb_result->nResultNum * sizeof(int));
						usb_result->pUSBType       = realloc(usb_result->pUSBType, usb_result->nResultNum * sizeof(int));
						usb_result->pPortPosition  = realloc(usb_result->pPortPosition, usb_result->nResultNum * sizeof(int));
						usb_result->pTimeCost_ms   = realloc(usb_result->pTimeCost_ms, usb_result->nResultNum * sizeof(unsigned long long));
						usb_result->pTransferSpeed = realloc(usb_result->pTransferSpeed, usb_result->nResultNum * sizeof(unsigned int));
						usb_result->pUSBType[usb_result->nResultNum-1] = nType;
						usb_result->pPortPosition[usb_result->nResultNum-1] = nPortPosition;
						sprintf(szDevPath[usb_result->nResultNum-1], "%s", szPath);
					}
				}
				pclose(fp);
			}
		}
	}
	pclose(pFile);
	
	if(usb_result->nResultNum < 1){
		return NOT_FOUND_DEVICE;
	}
	else{
		if (usb_cfg->nInterfaceType == USB_3_0_TYPE_A){
			if (!(usb_result->nUsbFoundDevice & USB_3_0_TYPE_A_UP_FOUND) || !(usb_result->nUsbFoundDevice & USB_3_0_TYPE_A_DOWN_FOUND) ||
			  	!(usb_result->nUsbFoundDevice & USB_2_0_TYPE_A_UP_FOUND) || !(usb_result->nUsbFoundDevice & USB_2_0_TYPE_A_DOWN_FOUND)){
				return NOT_FOUND_DEVICE;
			}
		}
	}

	for(int i = 0 ; i < usb_result->nResultNum ; i++){
		char szOriginalPath[MAX_PATH] = {0};
		char szTemp[MAX_CMD] = {0};
		sprintf(szTemp, "df -T | grep  %s",szDevPath[i]);
		FILE *p = popen(szTemp, "r");
		if(fgets(szTemp, MAX_CMD, p)){
			sprintf(szOriginalPath, "%s", (char*)(strstr(szTemp, "%") + 2));
			szOriginalPath[strlen(szOriginalPath) - 1] = '\0';
		}
		pclose(p);

		if(szOriginalPath){
			gettimeofday(&start_time, NULL);
			ms_start_time = (unsigned long long)(start_time.tv_sec) * 1000 + (unsigned long long)(start_time.tv_usec) / 1000;

			sprintf(szCMD, "dd if=/dev/zero of=%s/100M conv=fdatasync bs=10M count=10", szOriginalPath);
			SystemCMD(szCMD);

			gettimeofday(&end_time, NULL);
			ms_end_time = (unsigned long long)(end_time.tv_sec) * 1000 + (unsigned long long)(end_time.tv_usec) / 1000;

			memset(szCMD, 0, MAX_CMD);
			sprintf(szCMD, "rm %s/100M", szOriginalPath);
			SystemCMD(szCMD);

			usb_result->pUSBResults[i] = OK;
			usb_result->pTimeCost_ms[i] = ms_end_time - ms_start_time;
			usb_result->pTransferSpeed[i] = 100/((ms_end_time - ms_start_time)/1000.0);	
		}
	}

	return OK;
}

int TOB_WriteEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_RESULT *eeprom_result){
	char szMsg[MAX_PATH] = {0}, szData[10] = {0}, szCMD[MAX_CMD] = {0};
	int fd = 0, ret = 0;

	if(eeprom_cfg == NULL || eeprom_result == NULL)
		return INVALID_ARG;

	sprintf(szCMD, "/dev/i2c-%d", eeprom_cfg->nI2C_ID);

	fd = open(szCMD,O_RDWR);
	if(fd<0)
	{
		return FAILED;
	}

	if (ioctl(fd , I2C_SLAVE, eeprom_cfg->nAddress) < 0) {
    	/* ERROR HANDLING; you can check errno to see what went wrong */
		close(fd);
		return FAILED;
	}

	if(eeprom_cfg->pszContent){
		for(int n = 0 ; n < strlen(eeprom_cfg->pszContent) ; n++){
			szData[0] = n;
			szData[1] = eeprom_cfg->pszContent[n];
			sprintf(szMsg, "[0x%02X] : 0x%02X\n", szData[0], szData[1]);
		#ifdef _DEBUG_
			printf(szMsg);
		#endif
			if(write(fd, szData, 2) != 2){
				printf("write failed.\n");
				break;
			}
			sleep(1);
		}
	}
	
	close(fd);
	return OK;
}

int	TOB_ReadEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_RESULT *eeprom_result){
	char szMsg[MAX_PATH] = {0}, szData[10] = {0}, szCMD[MAX_CMD] = {0};
	int fd = 0, ret = 0;

	if(eeprom_cfg == NULL || eeprom_result == NULL)
		return INVALID_ARG;

	sprintf(szCMD, "/dev/i2c-%d", eeprom_cfg->nI2C_ID);
	fd = open(szCMD,O_RDWR);
	if(fd<0)
	{
		return FAILED;
	}

	if (ioctl(fd , I2C_SLAVE, eeprom_cfg->nAddress) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		close(fd);
		return FAILED;
	}

	for(int n = 0 ; n < strlen(eeprom_cfg->pszContent) ; n++){
		szData[0] = n;  //eeprom address in Page 0
		if(write(fd, szData, 1) != 1)
		{
			/* ERROR HANDLING: i2c transaction failed */
			printf("Failed to write to the i2c bus.\n");
			break;
		}
		//now if the eeprom points byte which You want to read, do random read sequence:
		if(read(fd, eeprom_result->pszResult + n, 1) != 1)  //now we know what address will be read:
		{
			/* ERROR HANDLING: i2c transaction failed */
			printf("Failed to read from the i2c bus.\n");
			break;
		}
	}
	
	close(fd);
	return OK;
}

char *gst_N622(void){
	return "gst-launch-1.0 nvarguscamerasrc timeout=10 ! 'video/x-raw(memory:NVMM), width=(int)3864, height=(int)2180, format=(string)NV12, \
											framerate=(fraction)30/1' ! nvvidconv ! xvimagesink -e";
}

char *gst_IMX179(void){
	return "gst-launch-1.0 nvarguscamerasrc sensor-id=0 sensor-mode=4 timeout=5 ! 'video/x-raw(memory:NVMM), width=(int)1640, height=(int)1232, format=(string)NV12, \
											framerate=(fraction)30/1' ! nvvidconv ! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! nveglglessink -e";
}

char *gst_IMX334(void){
	return "gst-launch-1.0 nvarguscamerasrc sensor-id=%d timeout=10 ! 'video/x-raw(memory:NVMM), width=(int)3864, height=(int)2180, format=(string)NV12, \
											framerate=(fraction)30/1' ! nvvidconv ! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! xvimagesink -e";
}

char *gst_IMX334_ISP(void){
	return "gst-launch-1.0 v4l2src io-mode=4 device=/dev/video%d do-timestamp=true num-buffers=200 ! 'video/x-raw, width=3840, height=2160, framerate=30/1, format=UYVY' \
											! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! xvimagesink sync=false";
}

char *gst_IMX290(void){
	return "gst-launch-1.0 nvarguscamerasrc sensor-id=%d sensor-mode=0 timeout=10 ! 'video/x-raw(memory:NVMM), width=(int)1948, height=(int)1096, format=(string)NV12, \
											framerate=(fraction)30/1' ! nvvidconv ! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! nveglglessink -e";
}

char *gst_IMX290_ISP(void){
	return "gst-launch-1.0 v4l2src io-mode=4 device=/dev/video%d do-timestamp=true num-buffers=200 ! 'video/x-raw, width=1920, height=1080, framerate=30/1, format=UYVY' \
											! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! xvimagesink sync=false";
}


char *gst_IMX290_six(void){
	return "gst-launch-1.0 nvarguscamerasrc sensor-id=%d sensor-mode=0 timeout=10 ! 'video/x-raw(memory:NVMM), width=(int)1948, height=(int)1096, format=(string)NV12, \
											framerate=(fraction)30/1' ! nvvidconv ! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! nveglglessink -e";
}

char *gst_IMX290_ISP_six(void){
	return "gst-launch-1.0 v4l2src io-mode=4 device=/dev/video%d do-timestamp=true num-buffers=200 ! 'video/x-raw, width=1920, height=1080, framerate=30/1, format=UYVY' \
											! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! xvimagesink sync=false";
}

char *gst_IMX290_Thine(void){
	return "gst-launch-1.0 v4l2src io-mode=4 device=/dev/video%d do-timestamp=true num-buffers=200 ! 'video/x-raw, width=1920, height=1080, framerate=30/1, format=UYVY' \
											! textoverlay font-desc=\"Sans 24\" text=\"ID-%d\" valignment=top halignment=center ! xvimagesink sync=false";
}

int TOB_Camera_Testing(CAMERA_CFG *camera_cfg, CAMERA_RESULT *camera_result){
	char szCMD[MAX_CMD] = {0};
	char szResult[MAX_CMD] = {0};
	char szDevice[MAX_PATH] = {0};
	int pass_count = 0;

	if (camera_cfg->nCameraType == CameraType_N622){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_N622(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "Setup Complete")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX179){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX179(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "continuing")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX290){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "continuing")) {
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}	

	if (camera_cfg->nCameraType == CameraType_IMX290_ISP){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290_ISP(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "GstSystemClock")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}	

	if (camera_cfg->nCameraType == CameraType_IMX334){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX334(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "Setup Complete")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX334_ISP){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX334_ISP(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "GstSystemClock")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX290_six){
		for (int index=0;index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290_six(), index, index);
			FILE *pCamera = popen(szCMD, "r");

			while (fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "Setup Complete")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX290_ISP){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290_ISP(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "GstSystemClock")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX290_ISP_six){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290_ISP_six(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "GstSystemClock")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}				
			}
			pclose(pCamera);
		}
	}

	if (camera_cfg->nCameraType == CameraType_IMX290_Thine){
		for (int index=0; index<camera_cfg->nCameraCount;index++){
			camera_result->pCameraResults[index] = false;
			sprintf(szCMD, gst_IMX290_Thine(), index, index);
			FILE *pCamera = popen(szCMD, "r");
			while(fgets(szResult, MAX_CMD, pCamera)){
				if (strstr(szResult, "Setup Complete")){
					camera_result->pCameraResults[index] = OK;
					pass_count++;
				}
			}
			pclose(pCamera);
		}
	}

	if (pass_count == camera_cfg->nCameraCount){
		camera_result->nResult = OK;
		return OK;
	}

	return FAILED;
}

int TOB_Temperature_Testing(TEMPERATURE_CFG *temperature_cfg,TEMPERATURE_RESULT *temperature_result){
	char szCMD[MAX_CMD] = {0};
	char szResult[MAX_CMD] = {0};
	char buffer[2] = {0};

	int nResult = i2c_read_data(temperature_cfg->nTemperature_i2c_ID, temperature_cfg->nTemperature_i2c_Address, &buffer);
	if (nResult != 0){
		return FAILED;
	}

	int conbined_data = (buffer[0] * 256 + (buffer[1] & 0xF0)) / 16;
	if (conbined_data > 2047){
		conbined_data -= 4096;
	}
	temperature_result->nTemperature = conbined_data * 0.0625;

	return OK;
}
