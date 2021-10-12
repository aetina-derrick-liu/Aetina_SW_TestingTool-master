#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <crypt.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include "common.h"

int main(int argc, char **argv){
  char szMsg[MAX_PATH] = {0}, szCMD[MAX_PATH] = {0}, szErrMsg[MAX_PATH] = {0};
	bool bIsStop = false, bMenuIsShow = false, bIsAuto = false;
    
	TestCore *test_core = TestCoreInit();
	
	//initial
	system("clear");
	
	//showDeviceInfo();

	for (int i=0;i<argc;i++){
		if (strcmp(argv[i], "-a") == 0){
			bIsAuto = true;
		}
	}

	// debug mode you can execute the single funtion that you want.
	if(bIsAuto){
		//create the report.
		configureTestCore(test_core, TestMode_Auto);
		autoRunTests(test_core);
	}
	else
	{	
		char ch;
		configureTestCore(test_core, TestMode_Debug);
		while (!bIsStop){
			if(!bMenuIsShow){
				showDebugMenu(test_core);
				bMenuIsShow = true;
			}
			
			ch = getchar();
			if (ch != '\n'){
				if (ch != 'z'){
					runTest(test_core, ch);
					bMenuIsShow = false;
				}
				else{
					bIsStop = true;
				}
			}
		}
	}

	free(test_core);
	return 0;
}
