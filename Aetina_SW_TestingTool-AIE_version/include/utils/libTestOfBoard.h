#ifndef _LIBTESTOFBOARD_
#define _LIBTESTOFBOARD_

#define MAC_SIZE 18
#define IP_SIZE	 16
#define MAX_PATH 256
#define MAX_CMD	 1024

/****************************************
 * 
 * Jetson Variable
 * 
****************************************/

enum JETSON_MODULE_ID{
	TX2_ID		= 24,
	XAVIER_ID	= 25,
	NANO_ID = 33
};

typedef enum JETSON_TYPE_{
	TX2,
	TX2_4GB,
	TX2i,
	TX2_NX,
	XAVIER,
	XAVIER_NX,
	NANO
}JetsonModule;

/****************************************
 * 
 * Carriers Boards
 * 
****************************************/

typedef enum ProductPortFolio_{
	AN310,
	AN310_IMX334,
	AN310_IMX334_ISP,
	AN310_IMX290,
	AN310_IMX290_six,
	AN310_IMX290_ISP,
	AN310_IMX290_ISP_six,
	AN310_IMX290_Thine,
	AN510,
	N622,
	AX710,
	AX710_IMX334,
	AN110,
	AN110_IMX179_J13,
	AN110_IMX334_J8,
	AN110_IMX334ISP_J8,
	AN110_IMX334THCV_J8,
	AN110_IMX290_J13,
	AN110_IMX290_J8,
	AN110_IMX290_Dual,
	AN110_IMX290THCV_J8,	
	AN110_IMX290ISP_J13,
	AN110_IMX290ISP_J8,
	AN110_IMX290ISP_Dual,	
	T012,
	AX720,
	AN810,
	AN810_IMX290THCV_six,
	AT017,
	AIE_CN11,
	AIE_CN12,
	AIE_CN13,
	AIE_CN14,
	AIE_CO11,
	AIE_CO12,
	AIE_CO13,
	AIE_CO14,
	AIE_CT41,
	AIE_CT42,
	AIE_CT43,
	AIE_CT44

}ProductPortFolio;

/****************************************
 *
 * Error code
 *
 ****************************************/
enum ERROR_CODE{
	FAILED = -1,
	OK,
	INVALID_ARG,	
	NOT_FOUND_DEVICE,
};

/****************************************
 *
 * Device information
 *
 ****************************************/
typedef struct tagDeviceInfo{
	int   nLanCount;
	char *pModule;
	char *pszCarrierBoard;
	char *pszSerialNumber;
	char **pszMAC_Address;
	char *pszBSP_Version;
	char *pszDTS_Name;
}DeviceInfo;

/****************************************
 *
 * LAN Testing
 *
 ****************************************/
typedef struct tagLAN_CFG{
	int		nLAN_ID;
}LAN_CFG;

typedef struct tagLAN_Result{
	int 				nResult;
	char 				szIP[IP_SIZE];
}LAN_RESULT, *pLAN_RESULT;

/****************************************
 *
 * WIFI Testing
 *
 ****************************************/
typedef struct tagWIFI_Result{
	int 				nResult;
	char				szMACNum[MAC_SIZE];
}WIFI_RESULT;

/****************************************
 *
 * Bluetooth Testing
 *
 ****************************************/
typedef struct tagBT_Result{
	int 				nResult;
	char 				szMACNum[MAC_SIZE];
}BT_RESULT;

/****************************************
 *
 * SDCARD Testing
 *
 ****************************************/
typedef struct tagSDCARD_CFG{
	char 	*pszDevPath;
	char 	*pszTmp_path;
	char 	*pszTmp_file;
}SDCARD_CFG;

typedef struct tagSDCARD_Result{
	int 				nResult;
	unsigned long long 	nTimeCost_ms;
	unsigned int 		nTransferSpeed;
}SDCARD_RESULT;

/****************************************
 *
 * mSATA Testing
 *
 ****************************************/
typedef struct tagMSATA_CFG{
	char    *pszTmp_path;
	char    *pszTmp_file;
}MSATA_CFG;

typedef struct tagMSATA_Result{
	int                     nResult;
	unsigned long long      nTimeCost_ms;
	unsigned int            nTransferSpeed;
}MSATA_RESULT;

/****************************************
 *
 * mPCIE Testing
 *
 ****************************************/
enum MPCIE_Type{
	STANDARD,
	DUALMPCIE,
	HYBRID
};

typedef struct tagMPCIE_CFG{
        int nMPCIE_type;
		char *pMPCIE_UP;
		char *pMPCIE_DOWN;
}MPCIE_CFG;

typedef struct tagMPCIE_Result{
        int nResult;
}MPCIE_RESULT;

/****************************************
 *
 * RS232 Testing
 *
 ****************************************/
typedef struct tagRS232_CFG{
	char	*pszDeviceName;
	char	*pszTestingData;
}RS232_CFG;

typedef struct tagRS232_RESULT{
	int		nResult;
}RS232_RESULT;

/****************************************
 *
 * GPIO Testing
 *
 ****************************************/
typedef struct tagGPIO_CFG{
	int  nGPIO_Pins_Number;
	int *pGPIO_Pins;
}GPIO_CFG;

typedef struct tagGPIO_RESULT{
	int  nResult;
	int  nPinCount;
	int *nPinResults;
}GPIO_RESULT;

/****************************************
 *
 * UART Testing
 *
 ****************************************/
typedef struct tagUART_CFG{
	char	*pszDeviceName;
	char	*pszTestingData;
}UART_CFG;

typedef struct tagUART_RESULT{
	int		nResult;
}UART_RESULT;

/****************************************
 *
 * UART J518 Testing
 *
 ****************************************/
typedef struct tagUART2_CFG{
	char	*pszDeviceName;
	char	*pszTestingData;
}UART2_CFG;

typedef struct tagUART2_RESULT{
	int		nResult;
}UART2_RESULT;

/****************************************
 *
 * UART CSI Testing
 *
 ****************************************/
typedef struct tagUART_CSI_CFG{
	char	*pszDeviceName;
	char	*pszTestingData;
}UART_CSI_CFG;

typedef struct tagUART_CSI_RESULT{
	int		nResult;
}UART_CSI_RESULT;


/****************************************
 *
 * FAN Testing
 *
 ****************************************/
typedef struct tagFAN_CFG{
	int 	nValue;
	int		nFAN_Number;
}FAN_CFG;

typedef struct tagFAN_RESULT{
	int		nResult;
}FAN_RESULT;

/****************************************

 *
 * eDP Testing
 *
 ****************************************/
typedef struct tagEDP_CFG{
	int 	nValue;
	int 	nEDP_Number;
}EDP_CFG;

typedef struct tagEDP_RESULT{
	int		nResult;
}EDP_RESULT;

/****************************************
 *
 * CSI Testing
 *
 ****************************************/

/****************************************
 *
 * CANBus Testing
 *
 ****************************************/
typedef struct tagCAN_CFG{
	char	*pszSendCAN;
	char	*pszReceiveCAN;
	char 	*pszId;
	char	*pszData;
}CAN_CFG;

typedef struct tagCAN_RESULT{
	int 	nResult;
}CAN_RESULT;

/****************************************
 *
 * I2C Testing
 *
 ****************************************/
typedef struct tagI2C_CFG{
	int		nI2C_ID;
	int		nAddress;
	int		nData;
}I2C_CFG;

typedef struct tagI2C_RESULT{
	int		nResult;
}I2C_RESULT;

/****************************************
 *
 * I2C Testing
 *
 ****************************************/
typedef struct tagI2C_II_CFG{
	int		nI2C_ID;
	int		nAddress;
	int		nData;
}I2C_II_CFG;

typedef struct tagI2C_II_RESULT{
	int		nResult;
}I2C_II_RESULT;

/****************************************
 *
 * I2C CSI Testing
 *
 ****************************************/
typedef struct tagI2C_CSI_CFG{
	int		nI2C_ID;
	int		nAddress;
	int		nData;
}I2C_CSI_CFG;

typedef struct tagI2C_CSI_RESULT{
	int		nResult;
}I2C_CSI_RESULT;

/****************************************
 *
 * SPI Testing
 *
 ****************************************/
typedef struct tagSPI_CFG{
	char*	pszDeviceName;
	char*	pszData;
	int		nDatalength;
}SPI_CFG;

typedef struct tagSPI_RESULT{
	int nResult;
}SPI_RESULT;

/****************************************
 *
 * SENSORS Testing
 *
 ****************************************/
enum SENSORS_TYPE{
	TEMPERATURE,
};

typedef struct tagSENSORS_CFG{
	int nType;
}SENSORS_CFG;

typedef struct tagSENSORS_RESULT{
	int nResult;
}SENSORS_RESULT;

/****************************************
 * 
 * Thermal Testing
 * 
 ****************************************/
typedef struct tagTEMPERATURE_CFG{
	int   nTemperature_i2c_ID;
	char  nTemperature_i2c_Address;
}TEMPERATURE_CFG;

typedef struct tagTEMPERATURE_RESULT{
	int    nResult;
	float  nTemperature;
}TEMPERATURE_RESULT;

/****************************************
 *
 * USB Testing
 *
 ****************************************/
#define USB_3_0_TYPE_A_UP_FOUND    0x01
#define USB_3_0_TYPE_A_DOWN_FOUND  0x02
#define USB_2_0_TYPE_A_UP_FOUND    0x04
#define USB_2_0_TYPE_A_DOWN_FOUND  0x08

enum USB_PORT_POSITION{
	UP,
	DOWN
};

enum INTERFACE_TYPE{
	USB_3_0_TYPE_A,
	USB_3_0_TYPE_C,
	MICRO_USB
};

enum USB_Type{
	USB_2_0,
	USB_3_0
};

typedef struct tagUSB_CFG{
	int	 nInterfaceType;
	int  nInterfaceNum;
	char *pUSB_3_0_TYPE_A_UP_USB3DEV;
	char *pUSB_3_0_TYPE_A_UP_USB2DEV;
	char *pUSB_3_0_TYPE_A_DOWN_USB3DEV;
	char *pUSB_3_0_TYPE_A_DOWN_USB2DEV;
	char *pUSB_3_0_TYPE_C_USB3DEV;
	char *pUSB_3_0_TYPE_C_USB2DEV;
	char *pMICRO_USB_DEV;
	//SKU3 210609_mental
	//2nd
	char *pUSB_2nd_TYPE_A_UP_USB3DEV;
	char *pUSB_2nd_TYPE_A_UP_USB2DEV;
	char *pUSB_2nd_TYPE_A_DOWN_USB3DEV;
	char *pUSB_2nd_TYPE_A_DOWN_USB2DEV;
	//3rd
	char *pUSB_3rd_TYPE_A_UP_USB3DEV;
	char *pUSB_3rd_TYPE_A_UP_USB2DEV;
	char *pUSB_3rd_TYPE_A_DOWN_USB3DEV;
	char *pUSB_3rd_TYPE_A_DOWN_USB2DEV;
	//4th
	char *pUSB_4th_TYPE_A_UP_USB3DEV;
	char *pUSB_4th_TYPE_A_UP_USB2DEV;
	char *pUSB_4th_TYPE_A_DOWN_USB3DEV;
	char *pUSB_4th_TYPE_A_DOWN_USB2DEV;
	//
}USB_CFG;

typedef struct tagUSB_RESULT{
	int					 nResultNum;
	char                 nUsbFoundDevice;
	int                  nResult;
	int 				*pUSBResults;
	int					*pUSBType;
	int					*pPortPosition;
	char                *pDevicePath;
	unsigned long long 	*pTimeCost_ms;
	unsigned int 		*pTransferSpeed;
}USB_RESULT;

/****************************************
 *
 * M.2 M Key Testing
 *
 ****************************************/
enum M2_MKEY_DEVICE_TYPE{
	SATA,
	PCIE
};

typedef struct tagM2_MKEY_CFG{
	int      nDeviceType;
	char    *pszTmp_path;
	char    *pszTmp_file;
}M2_MKEY_CFG;

typedef struct tagM2_MKEY_RESULT{
	int 				nResult;
	unsigned long long 	nTimeCost_ms;
	unsigned int 		nTransferSpeed;
}M2_MKEY_RESULT;

/****************************************
 *
 * M.2 E Key Testing
 *
 ****************************************/
typedef struct tagM2_EKEY_CFG{
	char *pszKeyword;
}M2_EKEY_CFG;

typedef struct tagM2_EKEY_RESULT{
	int nResult;
}M2_EKEY_RESULT;

/****************************************
 *
 * M.2 B Key Testing
 *
 ****************************************/
typedef struct tagM2_BKEY_CFG{
	char *pszKeyword;
}M2_BKEY_CFG;

typedef struct tagM2_BKEY_RESULT{
	int nResult;
}M2_BKEY_RESULT;

/****************************************
 *
 * EEPROM Testing
 *
 ****************************************/

#define EEPROM_SIZE 88

enum EEPROM_ACTION{
	WRITE_EEPROM,
	READ_EEPROM,
};

typedef struct tagEEPROM_CFG{
	int	   nI2C_ID;
	int    nAddress;
	char  *pszContent;
}EEPROM_CFG;

typedef struct tagEEPROM_Result{
	int nResult;
	char *pszResult;
}EEPROM_RESULT;

/****************************************
 * 
 * Camera Test
 * 
 ****************************************/
typedef enum{
	CameraType_N622,
	CameraType_IMX179,
	CameraType_IMX334,
	CameraType_IMX334_ISP,
	CameraType_IMX334_Thine,
	CameraType_IMX290,
	CameraType_IMX290_six,
	CameraType_IMX290_ISP,
	CameraType_IMX290_ISP_six,
	CameraType_IMX290_Thine,

}CameraType;

typedef struct tagCAMERA_CFG{
	int        nCameraCount;
	CameraType nCameraType;
}CAMERA_CFG;

typedef struct tagCAMERA_Result{
	int  nResult;
	int *pCameraResults;
}CAMERA_RESULT;

char *gst_N622(void);
char *gst_IMX334(void);
char *gst_IMX334_ISP(void);
char *gst_IMX290(void);
char *gst_IMX290_six(void);
char *gst_IMX290_ISP(void);
char *gst_IMX290_ISP_six(void);
char *gst_IMX290_Thine(void);

/****************************************
 *
 * APIs of Library
 *
 ****************************************/
typedef void (*CALLBACK_LOG_FUNC)(char *szMsg);

#ifdef __cplusplus
extern "C"{	
	void 		TOB_GetLibVersion(char *szVersion);
	void		TOB_SetLogFunc(CALLBACK_LOG_FUNC func);
	DeviceInfo*	TOB_GetDeviceInfo(DeviceInfo *devinfo_result);
	int			TOB_LAN_testing(LAN_CFG *lan_cfg, LAN_RESULT *lan_result);
	int			TOB_WIFI_testing(WIFI_RESULT *wifi_result);
	int			TOB_BT_testing(BT_RESULT *bt_result);
	int			TOB_SDCARD_testing(SDCARD_CFG *sdcard_cfg, SDCARD_RESULT *sdcard_result);
	int			TOB_MSATA_testing(MSATA_CFG *msata_cfg, MSATA_RESULT *msata_result);
	int			TOB_MPCIE_testing(MPCIE_CFG *mpcie_cfg, MPCIE_RESULT *mpcie_result);
	int			TOB_RS232_testing(RS232_CFG *rs232_cfg, RS232_RESULT *rs232_result);
	int 		TOB_I2C_Testing(I2C_CFG *i2c_cfg, I2C_RESULT *i2c_result);
	int 		TOB_I2C_II_Testing(I2C_II_CFG *i2c_II_cfg, I2C_II_RESULT *i2c_II_result);
	int 		TOB_I2C_CSI_Testing(I2C_CSI_CFG *i2c_CSI_cfg, I2C_CSI_RESULT *i2c_CSI_result);
	int			TOB_SPI_testing(SPI_CFG *spi_cfg, SPI_RESULT *spi_result);
	int			TOB_UART_Testing(UART_CFG *uart_cfg, UART_RESULT *uart_result);
	int			TOB_FAN_Testing(FAN_CFG *fan_cfg, FAN_RESULT *fan_result);
	int			TOB_EDP_Testing(EDP_CFG *eDP_cfg, EDP_RESULT *eDP_result);	
	int			TOB_CANBus_Testing(CAN_CFG *can_cfg, CAN_RESULT *can_result);
	int			TOB_Sensors_Testing(SENSORS_CFG *sensors_cfg, SENSORS_RESULT *sensors_result);
	int 		TOB_USB_Testing(USB_CFG *usb_cfg, USB_RESULT *usb_result);
	int			TOB_M2_EKEY_Testing(M2_EKEY_CFG *ekey_cfg, M2_EKEY_RESULT *ekey_result);
	int			TOB_M2_MKEY_Testing(M2_MKEY_CFG *mkey_Cfg, M2_MKEY_RESULT *mkey_result);
	int 		TOB_WriteEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_Result *eeprom_result);
	int			TOB_ReadEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_Result *eeprom_result);
	int			TOB_Camera_Testing(CAMERA_CFG *camera_cfg, CAMERA_Result *camera_result);
	int     TOB_Temperature_Testing(TEMPERATURE_CFG *thermal_cfg, TEMPERATURE_RESULT *thermal_result);
}
#else
extern void 	TOB_GetLibVersion(char *szVersion);
extern void		TOB_SetLogFunc(CALLBACK_LOG_FUNC func);
extern DeviceInfo* TOB_GetDeviceInfo(DeviceInfo *devinfo_result);
extern int		TOB_LAN_testing(LAN_CFG *lan_cfg, LAN_RESULT *lan_result);
extern int		TOB_WIFI_testing(WIFI_RESULT *wifi_result);
extern int		TOB_BT_testing(BT_RESULT *bt_result);
extern int		TOB_SDCARD_testing(SDCARD_CFG *sdcard_cfg, SDCARD_RESULT *sdcard_result);
extern int		TOB_MSATA_testing(MSATA_CFG *msata_cfg, MSATA_RESULT *msata_result);
extern int		TOB_MPCIE_testing(MPCIE_CFG *mpcie_cfg, MPCIE_RESULT *mpcie_result);
extern int		TOB_RS232_testing(RS232_CFG *rs232_cfg, RS232_RESULT *rs232_result);
extern int		TOB_I2C_Testing(I2C_CFG *i2c_cfg, I2C_RESULT *i2c_result);
extern int 		TOB_I2C_II_Testing(I2C_II_CFG *i2c_II_cfg, I2C_II_RESULT *i2c_II_result);
extern int		TOB_I2C_CSI_Testing(I2C_CSI_CFG *i2c_CSI_cfg, I2C_CSI_RESULT *i2c_CSI_result);
extern int		TOB_SPI_testing(SPI_CFG *spi_cfg, SPI_RESULT *spi_result);
extern int		TOB_UART_Testing(UART_CFG *uart_cfg, UART_RESULT *uart_result);
extern int		TOB_FAN_Testing(FAN_CFG *fan_cfg, FAN_RESULT *fan_result);
extern int		TOB_EDP_Testing(EDP_CFG *eDP_cfg, EDP_RESULT *eDP_result);
extern int		TOB_CANBus_Testing(CAN_CFG *can_cfg, CAN_RESULT *can_result);
extern int		TOB_Sensors_Testing(SENSORS_CFG *sensors_cfg, SENSORS_RESULT *sensors_result);
extern int		TOB_USB_Testing(USB_CFG *usb_cfg, USB_RESULT *usb_result);
extern int		TOB_M2_EKEY_Testing(M2_EKEY_CFG *ekey_cfg, M2_EKEY_RESULT *ekey_result);
extern int		TOB_M2_MKEY_Testing(M2_MKEY_CFG *mkey_Cfg, M2_MKEY_RESULT *mkey_result);
extern int 		TOB_WriteEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_RESULT *eeprom_result);
extern int		TOB_ReadEEPROM(EEPROM_CFG *eeprom_cfg, EEPROM_RESULT *eeprom_result);
extern int		TOB_Camera_Testing(CAMERA_CFG *camera_cfg, CAMERA_RESULT *camera_result);
extern int    TOB_Temperature_Testing(TEMPERATURE_CFG *temperature_cfg, TEMPERATURE_RESULT *temperature_result);
#endif
#endif
