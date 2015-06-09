#include	<stdio.h>
#include	<time.h>
#include	"Utilities.h"
#include    "PxiDAQ.h"
#include    <math.h>


#define		DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
//#define     USING_SMOOTH
#define     USING_IPP

int const ORDER_LOWPASS = 2;
        
void myMakeSrc_32f(Ipp32f** pSig, int len)
{
	*pSig = ippsMalloc_32f(len);

	ippsVectorJaehne_32f( *pSig, len, 1 );
}       

TimeData::TimeData() :
    lce00(1.0), lce01(1.0), lce02(1.0), h1(1.0), h2(1.0),
    lce10(1.0), lce11(1.0), lce12(1.0)
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&tick0);
    QueryPerformanceCounter(&tick1);
    QueryPerformanceCounter(&tick2);
}
TimeData     gtime_data;    

int StartSignalLoop(TaskHandle *rawAOHandle,  TaskHandle *rawForceHandle)
{
    TaskHandle AOHandle = *rawAOHandle;
    TaskHandle ForceReadTaskHandle = *rawForceHandle;
	int32       error=0;
	char        errBuff[2048]={'\0'};



    //IPP
    taps0 = ippsMalloc_32f(lenFilter);
    taps1 = ippsMalloc_32f(lenFilter);
    dly0  = ippsMalloc_32f(lenFilter);
    dly1  = ippsMalloc_32f(lenFilter);

    /*ippsSet_32f(1.0f, taps0, lenFilter);
    ippsSet_32f(1.0f, taps1, lenFilter);*/

    //taps0[0] = 0.0284f;
    //taps0[1] = 0.1427f;
    //taps0[2] = 0.3289f;
    //taps0[3] = 0.3289f;
    //taps0[4] = 0.1427f;
    //taps0[5] = 0.0284f;

    //taps1[0] = 0.0284f;
    //taps1[1] = 0.1427f;
    //taps1[2] = 0.3289f;
    //taps1[3] = 0.3289f;
    //taps1[4] = 0.1427f;
    //taps1[5] = 0.0284f;

    taps0[0] =  0.0078;
    taps0[1] =  0.0156;
    taps0[2] =  0.0078;
    taps0[3] =  1.0000;
    taps0[4] = -1.7347;
    taps0[5] =  0.7660;

    taps1[0] =  0.0078;
    taps1[1] =  0.0156;
    taps1[2] =  0.0078;
    taps1[3] =  1.0000;
    taps1[4] = -1.7347;
    taps1[5] =  0.7660;


    ippsZero_32f(dly0,lenFilter);
    ippsZero_32f(dly1,lenFilter);
        
    //ippsFIRInitAlloc_32f( &pFIRState0, taps0, lenFilter, dly0 );
    //ippsFIRInitAlloc_32f( &pFIRState1, taps1, lenFilter, dly1 );
    ippsIIRInitAlloc_32f( &pIIRState0, taps0, lenFilter, dly0 );
    ippsIIRInitAlloc_32f( &pIIRState1, taps1, lenFilter, dly1 );

    gDataFile = fopen(gTimeStamp,"a");

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&ForceReadTaskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(ForceReadTaskHandle,"PXI1Slot5/ai1","force0", DAQmx_Val_NRSE ,-5.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(ForceReadTaskHandle,"PXI1Slot5/ai9","force1", DAQmx_Val_NRSE ,-5.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(ForceReadTaskHandle,"",1000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1));
    
    DAQmxErrChk (DAQmxCreateTask("",&AOHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(AOHandle,"PXI1Slot2/ao10","motor0", 0.0,MAX_VOLT,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(AOHandle,"PXI1Slot2/ao11","motor1", 0.0,MAX_VOLT,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(AOHandle,"",1000.0,DAQmx_Val_Rising,DAQmx_Val_ContSamps,1));
	
    
    //DAQmxErrChk (DAQmxRegisterSignalEvent(ForceReadTaskHandle,DAQmx_Val_SampleClock, 0, UpdatePxiData ,NULL));

    DAQmxErrChk (DAQmxRegisterSignalEvent(ForceReadTaskHandle,DAQmx_Val_SampleClock, 0, UpdatePxiData , (void *) &gtime_data));
	DAQmxErrChk (DAQmxRegisterDoneEvent(ForceReadTaskHandle,0,DoneCallback,NULL));    
    
    QueryPerformanceCounter(&gInitTick);
	QueryPerformanceFrequency(&gClkFrequency);


	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
    if (0 != ForceReadTaskHandle) // Some error occurred
    {    
        // Sequence SENSITIVE: Start the tasks at the very end
        *rawForceHandle = ForceReadTaskHandle;
        *rawAOHandle = AOHandle;

        DAQmxErrChk (DAQmxStartTask(ForceReadTaskHandle));
        DAQmxErrChk (DAQmxStartTask(AOHandle));
        
    }
    else
    {
        //printf("Ready for EMG recording!\n");
        //getchar();
        *rawForceHandle = 0;
        *rawAOHandle = 0;
    }
	return 0;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	
    if( DAQmxFailed(error) )
		printf("StartSignalLoop Error: %s\n",errBuff);
	return 0;
}

int StopSignalLoop(TaskHandle *rawAOHandle, TaskHandle *rawForceHandle)
{
    TaskHandle AOHandle = *rawAOHandle;
    TaskHandle ForceReadTaskHandle = *rawForceHandle;
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    const float64     ZERO_VOLTS[NUM_MOTOR]={0.0, 0.0};
    
    fclose(gDataFile);

    //IPP
    //ippsFIRFree_32f(pFIRState0);
    //ippsFIRFree_32f(pFIRState1);
    
    ippsIIRFree_32f(pIIRState0);
    ippsIIRFree_32f(pIIRState1);
    
    ippsFree(taps0);
    ippsFree(taps1);
    ippsFree(dly0);
    ippsFree(dly1);

    DAQmxErrChk (DAQmxWriteAnalogF64(AOHandle, 1, TRUE, 10.0, DAQmx_Val_GroupByChannel, ZERO_VOLTS, NULL, NULL));
    
	//printf( "\nStopping EMG ...\n" );
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( ForceReadTaskHandle!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(ForceReadTaskHandle);
        DAQmxStopTask(AOHandle);
		DAQmxClearTask(ForceReadTaskHandle);
		DAQmxClearTask(AOHandle);
        *rawForceHandle = 0;
        *rawAOHandle = 0;
	}
    else
    {
        *rawForceHandle = ForceReadTaskHandle;
        *rawAOHandle = AOHandle;
    }
Error:
	if( DAQmxFailed(error) )
		printf("StopSignalLoop Error: %s\n",errBuff);
	//fclose(emgLogHandle);
	//printf("\nStopped EMG !\n");
	return 0;
}

#ifdef USING_SMOOTH
const int FILT_DIF = 30;
double difSmooth[NUM_MOTOR][FILT_DIF];
double difMean[NUM_MOTOR];
#endif


int32 CVICALLBACK UpdatePxiData(TaskHandle taskHandleDAQmx, int32 signalID, void *callbackData)
{
	int32   error=0;
	float64 loadcell_data[200]={0.0};
    float64 encoder_data[NUM_MOTOR][10]={{0.0}, {0.0}};
	int32   numRead;
	uInt32  i=0;
	char    buff[512], *buffPtr;
	char    errBuff[2048]={'\0'};
	char	*timeStr;
	time_t	currTime;
    float64 AOdata[NUM_MOTOR];


    //float32 lce0, lce1, lce2;
    //float32 t0, t1, t2;
    //LARGE_INTEGER tick0, tick1, tick2, frequency;


    //memset(AOdata,0,NUM_MOTOR*sizeof(float64));


	if( taskHandleDAQmx ) {
		//time (&currTime);
		//timeStr = ctime(&currTime);
		//timeStr[strlen(timeStr)-1]='\0';  // Remove trailing newline.

		/*********************************************/
		// DAQmx Read Code
		/*********************************************/

        

        DAQmxErrChk (DAQmxReadAnalogF64(taskHandleDAQmx,1,10.0,DAQmx_Val_GroupByScanNumber, loadcell_data, 2*CHANNEL_NUM,&numRead,NULL));
        double motor_cmd[NUM_MOTOR];
        switch(gCurrMotorState)
        {
        case MOTOR_STATE_INIT:
            motor_cmd[0] = ZERO_MOTOR_VOLTAGE;
            motor_cmd[1] = ZERO_MOTOR_VOLTAGE;
            break;
        case MOTOR_STATE_WINDING_UP:
            motor_cmd[0] = SAFE_MOTOR_VOLTAGE;
            motor_cmd[1] = SAFE_MOTOR_VOLTAGE;
            break;
        case MOTOR_STATE_OPEN_LOOP:
            motor_cmd[0] = 5.0; //5.0 * SAFE_MOTOR_VOLTAGE;  
            motor_cmd[1] = 5.0; //5.0 * SAFE_MOTOR_VOLTAGE;
            break;
        case MOTOR_STATE_CLOSED_LOOP:
            motor_cmd[0] = SAFE_MOTOR_VOLTAGE + gCtrlFromFPGA[0];
            motor_cmd[1] = SAFE_MOTOR_VOLTAGE + gCtrlFromFPGA[1];
            //motor_cmd[0] = gCtrlFromFPGA[0];
            //motor_cmd[1] = gCtrlFromFPGA[1];
            break;
        case MOTOR_STATE_SHUTTING_DOWN:
            motor_cmd[0] = ZERO_MOTOR_VOLTAGE;
            motor_cmd[1] = ZERO_MOTOR_VOLTAGE;
            break;
        default:
            motor_cmd[0] = ZERO_MOTOR_VOLTAGE;
            motor_cmd[1] = ZERO_MOTOR_VOLTAGE;
        }

        //AOdata[0] = (motor_cmd > MAX_VOLT) ? MAX_VOLT : motor_cmd;
        AOdata[0] = (motor_cmd[0] > MAX_VOLT) ? MAX_VOLT : ( (motor_cmd[0] < ZERO_MOTOR_VOLTAGE ) ? ZERO_MOTOR_VOLTAGE : motor_cmd[0]) ;
        AOdata[1] = (motor_cmd[1] > MAX_VOLT) ? MAX_VOLT : ( (motor_cmd[1] < ZERO_MOTOR_VOLTAGE ) ? ZERO_MOTOR_VOLTAGE : motor_cmd[1]) ;
        //printf("\nAO handle = %d \n", gAOTaskHandle);
        DAQmxErrChk (DAQmxWriteAnalogF64(gAOTaskHandle, 1, TRUE, 10.0, DAQmx_Val_GroupByChannel, AOdata, NULL, NULL));

		if( numRead ) {
//			printf("f1 %.4lf :: f2 %.4lf \n", data[0], data[1]);
			gAuxvar[0+0*NUM_AUXVAR] = (float32) loadcell_data[0];
            gAuxvar[0+1*NUM_AUXVAR] = (float32) loadcell_data[1];
			//gAuxvar[1] = (float32) loadcell_data[1];  //???
			//gAuxvar[1+NUM_AUXVAR] = (float32) loadcell_data[3];
		}

        DAQmxErrChk (DAQmxReadCounterF64(gEncoderHandle[0],1,10.0,encoder_data[0],NUM_MOTOR*1,&numRead,0));        
		if( numRead ) {
			gAuxvar[2] = (float) encoder_data[0][0];
		}
        DAQmxErrChk (DAQmxReadCounterF64(gEncoderHandle[1],1,10.0,encoder_data[1],NUM_MOTOR*1,&numRead,0));        
		if( numRead ) {
            gAuxvar[2+NUM_AUXVAR] = (float32) encoder_data[1][0];
		}
         
        //gMuscleLce = -gLenScale * (-gAuxvar[2] + gLenOrig) + 1.0;
        
        //gMuscleLce = gLenScale * (-gAuxvar[2] + gLenOrig) + 1.2;
        //gMuscleLce[0] = gAuxvar[2];
        //gMuscleLce[1] = gAuxvar[2+NUM_AUXVAR];

        //gEncoderCount[0] = (motor_cmd[0] - 0.9) * (815.0 - 0.00) / 4.1; 
        //gEncoderCount[1] = (motor_cmd[1] - 0.9) * (675.0 - 0.86) / 4.1;

        
        gEncoderCount[0] = 0.0; 
        gEncoderCount[1] = 0.0;
        
        gMuscleLce[0] = -gLenScale[0] * (-gAuxvar[2] - gEncoderCount[0] + gLenOrig[0]) + 1.0;
        gMuscleLce[1] = -gLenScale[1] * (-gAuxvar[2+NUM_AUXVAR] - gEncoderCount[1] + gLenOrig[1]) + 1.0;
        //float32 residuleMuscleLce = (2.02 - gMuscleLce[0] - gMuscleLce[1]) / 2.0;
        //gMuscleLce[0] += residuleMuscleLce;
        //gMuscleLce[1] += residuleMuscleLce;
        if ((0.0 == gMuscleLce[0]) || (0.0 == gMuscleLce[1]))
        {
            gMuscleLce[0] = 1.01;
            gMuscleLce[1] = 1.01;
        }

            
    	//QueryPerformanceFrequency(&frequency);

        TimeData *td = (TimeData*) callbackData;

        QueryPerformanceCounter(&(td->tick0));

        td->lce00 = -gAuxvar[2];
        td->lce10 = -gAuxvar[2+NUM_AUXVAR];
        
        td->h1 = 1.0 * (td->tick0.QuadPart - td->tick1.QuadPart) / td->frequency.QuadPart;
        td->h2 = 1.0 * (td->tick1.QuadPart - td->tick2.QuadPart) / td->frequency.QuadPart;


        // Calculate how many encoderCounts per second
        //float dEncoderCounts0 =   (td->lce00 * 3.0 - td->lce01 * 4.0 + td->lce02) / (td->h1 + td->h2);
        //float dEncoderCounts1 =   (td->lce10 * 3.0 - td->lce11 * 4.0 + td->lce12) / (td->h1 + td->h2);
        float dEncoderCounts0 =   (td->lce00 - td->lce01 );
        float dEncoderCounts1 =   (td->lce10 - td->lce11 );


        // Convert encoderTicks/sec to L0/sec
        float muscleVel0;
        float muscleVel1;

#ifdef  USING_IPP
        //ippsFIROne_32f(dEncoderCounts0, &muscleVel0, pFIRState0);
        //ippsFIROne_32f(dEncoderCounts1, &muscleVel1, pFIRState1);
        ippsIIROne_32f(dEncoderCounts0, &muscleVel0, pIIRState0);
        ippsIIROne_32f(dEncoderCounts1, &muscleVel1, pIIRState1);
        muscleVel0*=(-gLenScale[0]) * 1000.0f;
        muscleVel1*=(-gLenScale[1]) * 1000.0f;
#else
        muscleVel0 = dEncoderCounts0*(-gLenScale[0]);
        muscleVel1 = dEncoderCounts1*(-gLenScale[1]);
#endif
        // ensure muscleVel > 0
        //gMuscleVel[0] = (rawVel0 > 0.0) ? rawVel0 : 0.0;
        //gMuscleVel[1] = (rawVel1 > 0.0) ? rawVel1 : 0.0;
        // ensure muscleVel > 0

#ifdef USING_SMOOTH
        difMean[0]=0.0;
        difMean[NUM_MOTOR-1]=0.0;

        //Shift this and add to the end of it
        for(i=0;i<FILT_DIF-1;i++)
        {
	        difSmooth[0][i]=difSmooth[0][i+1];
	        difSmooth[NUM_MOTOR-1][i]=difSmooth[NUM_MOTOR-1][i+1];
        }

        difSmooth[0][FILT_DIF-1]=muscleVel0;
        difSmooth[NUM_MOTOR-1][FILT_DIF-1]=muscleVel1;

        for(i=0;i<FILT_DIF;i++)
        {
	        difMean[0]+=difSmooth[0][i];
	        difMean[NUM_MOTOR-1]+=difSmooth[NUM_MOTOR-1][i];
        }

        difMean[0]/=(double)FILT_DIF;
        difMean[NUM_MOTOR-1]/=(double)FILT_DIF;
        muscleVel0 = -gLenScale[0]*difMean[0];
        muscleVel1 = -gLenScale[1]*difMean[1];
#endif
    
        gMuscleVel[0] = (muscleVel0 > 0.0) ? muscleVel0: 0.0;
        gMuscleVel[1] = (muscleVel1 > 0.0) ? muscleVel1: 0.0;



        
        td->lce02 = td->lce01;
        td->lce01 = td->lce00;
        td->lce12 = td->lce11;
        td->lce11 = td->lce10;

        
        td->tick2 = td->tick1;
        td->tick1 = td->tick0;

	}
    

	return 0;

Error:
	if( DAQmxFailed(error) )
	{
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxStopTask(taskHandleDAQmx);
		DAQmxStopTask(gAOTaskHandle);
		DAQmxClearTask(taskHandleDAQmx);
		DAQmxClearTask(gAOTaskHandle);
		printf("UpdateData Error: %s\n",errBuff);
	}
	return 0;
}


int32 CVICALLBACK DoneCallback(TaskHandle taskHandleDAQmx, int32 status, void *callbackData)
{
	int32   error=0;
	char    errBuff[2048]={'\0'};

	// Check to see if an error stopped the task.
	DAQmxErrChk (status);

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(taskHandleDAQmx);
		printf("DAQmx Error: %s\n",errBuff);
	}
	return 0;
}

int EnableMotors(TaskHandle *rawHandle)
{
    TaskHandle  motorTaskHandle = *rawHandle;
	int32       error=0;
	char        errBuff[2048]={'\0'};
    uInt32      dataEnable=0xffffffff;
    uInt32      dataDisable=0x00000000;
    int32		written;

	DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorTaskHandle,"PXI1Slot2/port0","enable07",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
   	DAQmxErrChk (DAQmxWriteDigitalU32(motorTaskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,&written,NULL));

	*rawHandle = motorTaskHandle;

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	
    if( DAQmxFailed(error) )
		printf("EnableMotor Error: %s\n",errBuff);
	return 0;
}

int DisableMotors(TaskHandle *rawHandle)
{
    TaskHandle motorTaskHandle = *rawHandle;

	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    
	if( motorTaskHandle!=0 ) {
        DAQmxErrChk (DAQmxWriteDigitalU32(motorTaskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,&written,NULL));
	    //printf( "\nStopping Enable ...\n" );
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        *rawHandle = 0;
	}  
    else
    {
        *rawHandle = motorTaskHandle;
    }
    return 0;

Error:
	if( DAQmxFailed(error) )
		printf("DisableMotor Error: %s\n",errBuff);
	//fclose(emgLogHandle);
	//printf("\nStopped EMG !\n");
	return 0;
}


int StartReadPos(TaskHandle *rawHandle0,TaskHandle *rawHandle1)
{
	TaskHandle  encoderTaskHandle[NUM_MOTOR] ;
    encoderTaskHandle[0] = *rawHandle0;
    encoderTaskHandle[1] = *rawHandle1;
	int32       error=0;
	char        errBuff[2048]={'\0'};
    uInt32      dataEnable=0xffffffff;
    uInt32      dataDisable=0x00000000;

    int32		written;
    DAQmxCreateTask ("",&encoderTaskHandle[0]);
    DAQmxCreateTask ("",&encoderTaskHandle[1]);
    //printf("IN");
    //DAQmxLoadTask ("EncoderSlot3Ctr3",&encoderTaskHandle);
	if( encoderTaskHandle[0]!=0 ) {
        DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[0],"PXI1Slot3/ctr3","",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,24,0.0,""));
        //DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle,"PXI1Slot3/ctr4","",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,24,0.0,""));

	    DAQmxErrChk (DAQmxStartTask(encoderTaskHandle[0]));
    }

    if( encoderTaskHandle[1]!=0 ) {
        DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[1],"PXI1Slot3/ctr4","",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,24,0.0,""));

	    DAQmxErrChk (DAQmxStartTask(encoderTaskHandle[1]));
    }
    //printf("IN");

	*rawHandle0 = encoderTaskHandle[0];
	*rawHandle1 = encoderTaskHandle[1];

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	
    if( DAQmxFailed(error) )
		printf("EnableEncoder Error: %s\n",errBuff);
	return 0;
}

int StopPositionRead(TaskHandle *rawHandle0,TaskHandle *rawHandle1)
{
    TaskHandle encoderTaskHandle[NUM_MOTOR]; 
    encoderTaskHandle[0] = *rawHandle0;
    encoderTaskHandle[1] = *rawHandle1;

	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;

	if( encoderTaskHandle[0]!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(encoderTaskHandle[0]);
		DAQmxClearTask(encoderTaskHandle[0]);
        *rawHandle0 = 0;
    }
    else 
    {
        *rawHandle0 = encoderTaskHandle[0];
    }

    if( encoderTaskHandle[1]!=0 ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(encoderTaskHandle[1]);
		DAQmxClearTask(encoderTaskHandle[1]);
        *rawHandle1 = 0;
    }
    else 
    {
        *rawHandle1 = encoderTaskHandle[1];
    }
    return 0;

Error:
	if( DAQmxFailed(error) )
		printf("DisableEncoder Error: %s\n",errBuff);
	//fclose(emgLogHandle);
	//printf("\nStopped EMG !\n");
	return 0;
}
