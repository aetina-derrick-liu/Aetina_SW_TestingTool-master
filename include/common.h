#ifndef __COMMON__
#define __COMMON__

#include <stdarg.h>
#include "utils/iniparser.h"
#include "doTests.h"

//#define EXE_ARGS_AUTOMODE								"-autorun"

#define RESET_TIMES 5 //default =3

#define REPORT_CONTENT_LENGTH 80

#define GONDAM_NUMBER_LENGTH 14
#define BOARD_NUMBER_LENGTH  11


int   checkConfigItem(char*, int*);
int   setConfigItem(char*, int*);
void  resetConfigItem(void);
char* checkConfigInfo(char*, char*);
int   setConfigInfo(char*, char*);
void  resetConfigInfo(void);

typedef enum{
  TIMES,
  LAN,
  WIFI,
  BT,
  SDCARD,
  mSATA,
  mPCIE,
  RS232,
  UART,
  UART2,  
  UART_CSI,
  GPIO,
  FAN,
  eDP,
  CANbus,
  I2C,
  I2C_II,
  I2C_CSI,
  SPI,
  USB_Micro,
  USB_Type_A,
  USB_Type_C,
  m2_eKey,
  m2_bKey,
  m2_mKey_SATA,
  m2_mKey_PCIE,
  EEPROM,
  Camera,
  Temperature
}ItemKind;

struct Items_{
  ItemKind item_kind;
  struct Items_ *next;
};

typedef struct Items_ Items;

Items* ItemsInit(void);
Items* addItem(Items*, ItemKind);
Items* findItem(Items*, ItemKind);
void showAddedItems(Items*);
void showNonFinishItems(Items*);

typedef struct TestConfigs_
{
  char mTmp_Path[MAX_PATH];
  char mTmp_File[MAX_PATH];
  int  mLAN_Count;
  int  mCamera_Count;
  LAN_CFG            *pCfg_LAN;
  SDCARD_CFG         *pCfg_SDCard;
  MSATA_CFG          *pCfg_mSATA;
  MPCIE_CFG          *pCfg_mPCIE;
  RS232_CFG          *pCfg_RS232;
  UART_CFG           *pCfg_UART;
  UART2_CFG          *pCfg_UART2;
  UART_CSI_CFG       *pCfg_UART_CSI;
  GPIO_CFG           *pCfg_GPIO;  
  FAN_CFG            *pCfg_FAN; 
  EDP_CFG            *pCfg_eDP;   
  CAN_CFG            *pCfg_CAN;
  I2C_CFG            *pCfg_I2C;
  I2C_II_CFG         *pCfg_I2C_II;   
  I2C_CSI_CFG        *pCfg_I2C_CSI;  
  SPI_CFG            *pCfg_SPI;  
  USB_CFG            *pCfg_USB;
  M2_EKEY_CFG        *pCfg_m2_eKey; 
  M2_BKEY_CFG        *pCfg_m2_bKey;
  M2_MKEY_CFG        *pCfg_m2_mKey_SATA;  
  M2_MKEY_CFG        *pCfg_m2_mKey_PCIE;  
  EEPROM_CFG         *pCfg_EEPROM;  
  CAMERA_CFG         *pCfg_Camera;  
  TEMPERATURE_CFG    *pCfg_Temperature;   
}TestConfigs;

TestConfigs* TestConfigsInit(void);

typedef struct TestResult_{
  LAN_RESULT         *pResult_LAN;
  WIFI_RESULT        *pResult_WIFI;
  BT_RESULT          *pResult_BT;
  SDCARD_RESULT      *pResult_SDCard;
  MSATA_RESULT       *pResult_mSATA;
  MPCIE_RESULT       *pResult_mPCIE;
  RS232_RESULT       *pResult_RS232;
  UART_RESULT        *pResult_UART;
  UART2_RESULT       *pResult_UART2;  
  UART_CSI_RESULT    *pResult_UART_CSI;
  GPIO_RESULT        *pResult_GPIO;
  FAN_RESULT         *pResult_FAN;
  EDP_RESULT         *pResult_eDP;  
  CAN_RESULT         *pResult_CAN;
  I2C_RESULT         *pResult_I2C;
  I2C_II_RESULT      *pResult_I2C_II;
  I2C_CSI_RESULT     *pResult_I2C_CSI;
  SPI_RESULT         *pResult_SPI;
  USB_RESULT         *pResult_USB_Micro;
  USB_RESULT         *pResult_USB_Type_A;
  USB_RESULT         *pResult_USB_Type_C;
  M2_EKEY_RESULT     *pResult_m2_eKey;
  M2_BKEY_RESULT     *pResult_m2_bKey;
  M2_MKEY_RESULT     *pResult_m2_mKey_SATA;
  M2_MKEY_RESULT     *pResult_m2_mKey_PCIE;
  EEPROM_RESULT      *pResult_EEPROM;
  CAMERA_RESULT      *pResult_Camera;
  TEMPERATURE_RESULT *pResult_Temerature;
}TestResults;

TestResults* TestResultsInit(void);
void setFailedResults(TestResults*, Items*);

typedef struct TestReport_
{
  char *pFileName;
  char *pGondanNumber;
  char *pPartNumber;
  char *pBoardNumber;
  bool  isOnlyBoard;
}TestReport;

TestReport* TestReportInit(void);

char* appendContent(char*, char*);

void writeInfoSparateLine(FILE*);
void writeItemSparateLine(FILE*);
void writeReportHead(FILE*);
void writeInfoContent(FILE*, int, ...);
void writeItemContent(FILE*, int, ...);
void writeResultContents(FILE*, Items*, TestConfigs*, TestResults*);
TestReport* generateReport(TestReport*, DeviceInfo*, Items*, TestConfigs*, TestResults*, bool);
void showReport(TestReport*);
void uploadReport(TestReport*);
void deleteReport();

typedef enum{
  TestMode_Debug,
  TestMode_Auto
}TestMode;

typedef struct TestCore_
{
  unsigned int   mTestTimes;
  DeviceInfo    *pDeviceInfo;
  Items         *pTestItems;
  TestConfigs   *pTestConfigs;
  TestResults   *pTestResults;
  TestReport    *pTestReport;
}TestCore;

TestCore* TestCoreInit();

void showDebugMenu(TestCore*);
TestReport* scanBoardNumber(TestReport*);
TestReport* scanGondamAndPartNumber(TestReport*);

void waitForShutdown();

void autoRunTests(TestCore*);
void configureAutoRunItems(TestCore*);

void runTest(TestCore*, char);;
void configureDebugItems(TestCore*);

void setupCfgAndRst(Items*, TestConfigs*, TestResults*);
void setCfgNRst_TX2(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_TX2_4GB(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_TX2i(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_Xavier(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_Xavier_NX(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_Nano(const ProductPortFolio, Items*, TestConfigs*, TestResults*);
void setCfgNRst_TX2_NX(const ProductPortFolio, Items*, TestConfigs*, TestResults*);


void configureTestCore(TestCore*, TestMode);

#endif
