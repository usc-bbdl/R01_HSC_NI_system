
using namespace std;

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <AntTweakBar.h>
#include "okFrontPanelDLL.h"
/*
    Name:			reflex_single_muscle    
    Author:			C. Minos Niu    
    Email:			minos.niu AT sangerlab.net
*/

#include	<math.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<time.h>
#include	"NIDAQmx.h"
#include	"PxiDAQ.h"
#include	"Utilities.h"
#include	"glut.h"   // The GL Utility Toolkit (Glut) Header
#include	"OGLGraph.h"

//#pragma comment(lib, "lua51.lib")

// *** Global variables
float                   gAuxvar [NUM_AUXVAR*NUM_MOTOR];
pthread_t               gThreads[NUM_THREADS];
pthread_mutex_t         gMutex;  
TaskHandle              gEnableHandle, gForceReadTaskHandle, gAOTaskHandle, gEncoderHandle[NUM_MOTOR];
float                   gLenOrig[NUM_MOTOR], gLenScale[NUM_MOTOR], gMuscleLce[NUM_MOTOR], gMuscleVel[NUM_MOTOR];
bool                    gResetSim=false, gEmgSoundOn = false, gIsRecording=false, gResetGlobal=false, gIsP2pMoving=false;
float                   gP2pIndex = 0.0f, gDeltaLen = 0.0f;
LARGE_INTEGER           gInitTick, gCurrentTick, gClkFrequency;
FILE                    *gDataFile, *gConfigFile;
int                     gCurrMotorState = MOTOR_STATE_INIT;
double                  gEncoderCount[NUM_MOTOR];
float64                 gMotorCmd[NUM_MOTOR]={0.0, 0.0};
const float gGain = 2.5 / 2000.0;

float gMusB = 50.0f; 
float gMusKse = 136.0f;//136.0f;
float gMusKpe = 75.0f;//120.0;

bool gAlterDamping = false; //Damping flag
time_t randSeedTime;


float                   gCtrlFromFPGA[NUM_MUSCLE];
int                     gMuscleEMG[NUM_FPGA], gMNCount[NUM_FPGA];

OGLGraph*               gMyGraph;
char                    gLceLabel1[60];
char                    gLceLabel2[60];
char                    gTimeStamp[100];
char                    gTimeStampDropbox[100];
char                    gStateLabel[5][30] = { "MOTOR_STATE_INIT",
                                               "MOTOR_STATE_WINDING_UP",
                                               "MOTOR_STATE_OPEN_LOOP",
                                               "MOTOR_STATE_CLOSED_LOOP",                            
                                               "MOTOR_STATE_SHUTTING_DOWN"};
//IPP
Ipp32f *taps0;
Ipp32f *taps1;
Ipp32f *dly0;
Ipp32f *dly1;
Ipp32f *tapsVel0IIR, *tapsVel1IIR;
Ipp32f *dlysVel0IIR, *dlysVel1IIR;

//IppsFIRState_32f *pFIRState0, *pFIRState1;
IppsIIRState_32f *pIIRStateVel0, *pIIRStateVel1;
IppsIIRState_32f *pIIRState0, *pIIRState1;

// Sinewave
int gWave[1024];

//Descending command
int gM1Voluntary = 0;
int gM1Dystonia = 0;

//AntTweakBar
TwBar *gBar; // Pointer to the tweak bar

//Lua UDP
lua_State *L;
int statusLua;

//Fpga Data Logging
float32 gLenBic;
float32 gEmgBic;
float32 gLenTri;
float32 gEmgTri;
float gForceBic;
float gForceTri;
int gSpikeCountBic;
int gSpikeCountTri;
float32 gSpindleIaBic;
float32 gSpindleIIBic;
float32 gSpindleIaTri;
float32 gSpindleIITri;


void ledIndicator ( float w, float h );

void init ( GLvoid )     // Create Some Everyday Functions
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.f);				// Black Background
    //glClearDepth(1.0f);								// Depth Buffer Setup
    gMyGraph = OGLGraph::Instance();
    gMyGraph->setup( 500, 100, 10, 20, 2, 2, 1, 200 );
}
void outputText(int x, int y, char *string)
{
  int len, i;
  glRasterPos2f(x, y);
  len = (int) strlen(string);
  for (i = 0; i < len; i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
  }
}
void display ( void )   // Create The Display Function
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) 800 / (float) 600, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // This is a dummy function. Replace with custom input/data
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float value;
    value = 5*sin( 5*time ) + 10.f;

    
    //gMyGraph->update( 10.0 * gAuxvar[0] );
    //gMyGraph->update( gMuscleVel[0] );
    gMyGraph->update( gCtrlFromFPGA[0] );

    gMyGraph->draw();
    
    
    if(MOTOR_STATE_WINDING_UP)
        glColor3f(1.0f,0.0f,0.0f);
    else
        glColor3f(0.0f,1.0f,0.0f);
    ledIndicator( 10.0f,80.0f );


    if(!gIsRecording)
        glColor3f(1.0f,0.0f,0.0f);
    else
        glColor3f(0.0f,1.0f,0.0f);
    ledIndicator( 50.0f,80.0f );
    glColor3f(1.0f,1.0f,1.0f);

    // Draw tweak bars
    TwDraw();
//    sprintf_s(gLceLabel1,"%f    %.2f   %.2f", gMuscleLce[0], gMuscleVel[0], gCtrlFromFPGA[0]);
    sprintf_s(gLceLabel1,"%f    %f    %f    %f",  -gAuxvar[2], gMuscleVel[0], gCtrlFromFPGA[0], gMusB);
    outputText(10,95,gLceLabel1);
//    sprintf_s(gLceLabel2,"%f    %.2f   %.2f", gMuscleLce[1], gMuscleVel[1],gCtrlFromFPGA[NUM_FPGA - 1]);
    sprintf_s(gLceLabel2,"%f    %f    %f",  -gAuxvar[2+NUM_AUXVAR], gMuscleVel[1], gCtrlFromFPGA[1]);
    //sprintf_s(gLceLabel2,"%f    %d   %.2f", gMuscleLce[1], gM1Dystonia, gCtrlFromFPGA[NUM_FPGA - 1]);
    outputText(10,85,gLceLabel2);
    //printf("\n\t%f\t%f", gMuscleVel[0], gMuscleVel[1]);
    
    //sprintf_s(gStateLabel,"%.2f    %.2f   %f",gAuxvar[0], gMuscleLce, gCtrlFromFPGA[0]);
    outputText(300,95,gStateLabel[gCurrMotorState]);

    

    glutSwapBuffers ( );
}

void ledIndicator ( float w, float h )   // Create The Reshape Function (the viewport)
{
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(w,h,0.0f);
        glVertex3f(w,h+10.0f,0.0f);
        glVertex3f(w+10.0,h+10.0f,0.0f);
        glVertex3f(w+10.0f,h,0.0f);
    glEnd();
}
void reshape ( int w, int h )   // Create The Reshape Function (the viewport)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) w / (float) h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

    // Send the new window size to AntTweakBar
    TwWindowSize(w, h);
}

int ProceedFSM(int *state);
int InitMotor(int *state);

int ProceedFSM(int *state)
{
    switch(*state)
    {
        case MOTOR_STATE_INIT:
            EnableMotors(&gEnableHandle);
            *state = MOTOR_STATE_WINDING_UP;
            break;
        case MOTOR_STATE_WINDING_UP:
            *state = MOTOR_STATE_OPEN_LOOP;
            break;
        case MOTOR_STATE_OPEN_LOOP:
            *state = MOTOR_STATE_CLOSED_LOOP;
            break;
        case MOTOR_STATE_CLOSED_LOOP:
            DisableMotors(&gEnableHandle);
            *state = MOTOR_STATE_SHUTTING_DOWN;
            break;
        case MOTOR_STATE_SHUTTING_DOWN:
            *state = MOTOR_STATE_SHUTTING_DOWN;
            break;
        default:
            *state = MOTOR_STATE_INIT;
    }
    return 0;
}

int InitMotor(int *state)
{
    DisableMotors(&gEnableHandle);
    *state = MOTOR_STATE_INIT;
    return 0;
}

int ShutdownMotor(int *state);
int ShutdownMotor(int *state)
{
    DisableMotors(&gEnableHandle);
    *state = MOTOR_STATE_SHUTTING_DOWN;
    return 0;
}

void report_errors(lua_State *L, int status)
{
  if ( status!=0 ) {
    std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1); // remove error message
  }
}

void sendLuaUdp()
{
    if ( statusLua == 0 )
	{
      // execute Lua program
      statusLua = lua_pcall(L, 0, LUA_MULTRET, 0);
    }
}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
    switch ( key ) 
    {
    case 27:        // When Escape Is Pressed...

        exit(0);   // Exit The Program
        break;        
    //case 32:        // SpaceBar 
    //    //ShutdownMotor(&gCurrMotorState);
    //    break;  
    case 'G':       //Proceed in FSM
    case 'g':
        ProceedFSM(&gCurrMotorState);
        break;
    case '1':       
        gMusB = 0.0f; 
        break;
    case '2':       
        gMusB = 6.0f; 
        break;
    case '3':       
        gMusB = 12.0f; 
        break;
    case '4':       
        gMusB = 25.0f; 
        break;
    case '5':       
        gMusB = 50.0f; 
        break;
    case '6':       
        gMusB = 100.0f; 
        break;
    case '7':       
        gMusB = 200.0f; 
        break;
    

    case 'M':       //EMG SOUND
    case 'm':
        if(!gEmgSoundOn)
        {
            gEmgSoundOn=true;
        }
        else
            gEmgSoundOn=false;
        break;

    case 'R':       //Winding up
    case 'r':
        if(!gIsRecording)
        {
            gIsRecording=true;
            sendLuaUdp();
        }
        else
            gIsRecording=false;
        break;
    //case '0':       //Reset SIM
    //    if(!gResetSim)
    //        gResetSim=true;
    //    else
    //        gResetSim=false;
    //    SendButton(gXemMuscleBic, (int) gResetSim, "BUTTON_RESET_SIM");
    //    SendButton(gXemMuscleTri, (int) gResetSim, "BUTTON_RESET_SIM");
    //    break;
    case '9':       //Reset GLOBAL
        if(!gResetGlobal)
            gResetGlobal=true;
        else
            gResetGlobal=false;
       //SendButton(gXemMuscleBic, (int) gResetGlobal, "BUTTON_RESET_GLOBAL");
       //SendButton(gXemMuscleTri, (int) gResetGlobal, "BUTTON_RESET_GLOBAL");
       //SendButton(gXemSpindleBic, (int) gResetGlobal, "BUTTON_RESET_GLOBAL");
       //SendButton(gXemSpindleTri, (int) gResetGlobal, "BUTTON_RESET_GLOBAL");
        break;   
    case 'p':       //Minos: workaround for p2p movement
    case 'P':
        sendLuaUdp();
        break;

        //gIsP2pMoving = true;
        //break;
    case 'z':       //Rezero
    case 'Z':
        gLenOrig[0]=gAuxvar[2] + gEncoderCount[0];
        gLenOrig[1]=gAuxvar[2+NUM_AUXVAR] + gEncoderCount[1];
        break;
    case 'E':         
    case 'e':     
        InitMotor(&gCurrMotorState);
        break;  
    default:        
        break;
    }
    TwEventKeyboardGLUT(NULL,NULL,NULL);
}

void idle(void)
{
    glutPostRedisplay();
}


float d_force(float T_0, float x1, float x2, float A)
{
    // Take state variables
    // Return derivatives
    
    float x0 = 1.0f;
    
    float Kse_x_Kpe_o_b;
    float Kse_o_b_m_one_p_Kpe_o_Kse;
    float Kse_o_b;
    float dT_0; 

    float rate_change_x = x2; // slope;
    
    
    x0 = 1.0;
    Kse_x_Kpe_o_b = gMusKse / gMusB * gMusKpe; // 204
    Kse_o_b_m_one_p_Kpe_o_Kse = gMusKse / gMusB * (1 + gMusKpe / gMusKse); // 4.22
    Kse_o_b = gMusKse / gMusB; // 2.72
        
    dT_0 = Kse_x_Kpe_o_b *(x1 - x0) + gMusKse * x2 - Kse_o_b_m_one_p_Kpe_o_Kse * T_0 + Kse_x_Kpe_o_b * A;
        
    return dT_0;
}

void* ControlLoopBic(void*)
{
    
	int32       error=0;
	char        errBuff[2048]={'\0'};
        
    long iLoop = 0;
    while (1)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }

        if ((MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;
		//printf("\n\t%f",dataEncoder[0]); 
        iLoop++;
        
        gCtrlFromFPGA[0] += d_force(gCtrlFromFPGA[0], gMuscleLce[0], gMuscleVel[0], 0.5f)/1000.0f; 
        gCtrlFromFPGA[0] = gCtrlFromFPGA[0]/1.01;
        gCtrlFromFPGA[0] = (gCtrlFromFPGA[0]>= 0.0) ? gCtrlFromFPGA[0] : 0.0;
        


        
        //Sleep(1);
        if(_kbhit()) break;
    } 
	return 0;
}

void* ControlLoopTri(void*)
{
    
	int32       error=0;
	char        errBuff[2048]={'\0'};
        
    long iLoop = 0;
    while (1)
    {
        if(GetAsyncKeyState(VK_SPACE))
        { 
            ShutdownMotor(&gCurrMotorState);
        }

        if ((MOTOR_STATE_CLOSED_LOOP != gCurrMotorState) && (MOTOR_STATE_OPEN_LOOP != gCurrMotorState)) continue;
		//printf("\n\t%f",dataEncoder[0]); 
        iLoop++;
/*        if (10000 <= iLoop)
        {
            gM1Dystonia = 5000;
            gM1Voluntary = gWave[iLoop % 1024];
        }    */    




        gCtrlFromFPGA[1] += d_force(gCtrlFromFPGA[1], gMuscleLce[1], gMuscleVel[1], 0.5f)/1000.0f; 
        gCtrlFromFPGA[1] = gCtrlFromFPGA[1]/1.01;

        gCtrlFromFPGA[1] = (gCtrlFromFPGA[1]>= 0.0) ? gCtrlFromFPGA[1] : 0.0;
        


        //Sleep(1);
        if(_kbhit()) break;
    } 
	return 0;
}

void SaveConfigCache()
{
    FILE *ConfigCacheFile;
    ConfigCacheFile= fopen("ConfigPXI.cache","w");
    fprintf(ConfigCacheFile,"%lf\t%lf",gLenScale[0],gLenScale[1]);
    fclose(ConfigCacheFile);
}


FileContainer *gSwapFiles;

void InitProgram()
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf_s(gTimeStamp,"%4d%02d%02d%02d%02d.txt",timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);
    sprintf_s(gTimeStampDropbox,"C:\\Users\\PXI_BBDL\\Dropbox\\DataPxi\\Pxi%4d%02d%02d%02d%02d.txt",timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);

    gAlterDamping = false;
    srand((unsigned) time(&randSeedTime));

   
        

    gSwapFiles = new FileContainer;

    //IPP_VEL_IIR
    dlysVel0IIR = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    dlysVel1IIR = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    tapsVel0IIR  = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    tapsVel1IIR  = ippsMalloc_32f(2 * (lenFilterVel_IIR + 1));
    
    ippsZero_32f(dlysVel0IIR, 2 * (lenFilterVel_IIR + 1) );
    ippsZero_32f(dlysVel1IIR, 2 * (lenFilterVel_IIR + 1) );

    tapsVel0IIR[0] =  0.0078; // for Lowpass filter velocity
    tapsVel0IIR[1] =  0.0156;
    tapsVel0IIR[2] =  0.0078;
    tapsVel0IIR[3] =  1.0000;
    tapsVel0IIR[4] = -1.7347;
    tapsVel0IIR[5] =  0.7660;

    tapsVel1IIR[0] =  0.0078;
    tapsVel1IIR[1] =  0.0156;
    tapsVel1IIR[2] =  0.0078;
    tapsVel1IIR[3] =  1.0000;
    tapsVel1IIR[4] = -1.7347;
    tapsVel1IIR[5] =  0.7660;

    ippsIIRInitAlloc_32f(&pIIRStateVel0, tapsVel0IIR, lenFilterVel_IIR, dlysVel0IIR);
    ippsIIRInitAlloc_32f(&pIIRStateVel1, tapsVel1IIR, lenFilterVel_IIR, dlysVel1IIR);


    //IPP_FR
    taps0 = ippsMalloc_32f(lenFilter);
    taps1 = ippsMalloc_32f(lenFilter);
    dly0  = ippsMalloc_32f(lenFilter);
    dly1  = ippsMalloc_32f(lenFilter);
    
    float   P   =   1.0;
    float   e   =   2.7183;
    float   T   =   0.001;
    float   tau   =   0.090; // rising time of muscle twitch in seconds
    float   a   =   exp(-T / tau);
    float   pefat = P * e * T * a / tau;
    

    taps0[0] =   0.0000000 * pefat;
    taps0[1] =   1.00 * pefat;
    taps0[2] =   0.0000000 * pefat;
    taps0[3] =   1.00 * pefat;
    taps0[4] =  -2 * a * pefat;
    taps0[5] =   a * a * pefat;
                          
    taps1[0] =   0.0000000 * pefat;
    taps1[1] =   1.00 * pefat;
    taps1[2] =   0.0000000 * pefat;
    taps1[3] =   1.00 * pefat;
    taps1[4] =  -2 * a * pefat;
    taps1[5] =   a * a * pefat;


    ippsZero_32f(dly0,lenFilter);
    ippsZero_32f(dly1,lenFilter);
        
    //ippsFIRInitAlloc_32f( &pFIRState0, taps0, lenFilter, dly0 );
    //ippsFIRInitAlloc_32f( &pFIRState1, taps1, lenFilter, dly1 );
    ippsIIRInitAlloc_32f( &pIIRState0, taps0, lenFilter, dly0 );
    ippsIIRInitAlloc_32f( &pIIRState1, taps1, lenFilter, dly1 );

    // *** Get a sine wave
    SineGen(gWave);
    //{
    //    float F = 1.0f; // in Hz
    //    float BIAS = 20000.0f;
    //    float AMP = 10000.0f;
    //    float PHASE = 0.0f;
    //    float SAMPLING_RATE = 1024.0f;
    //    float dt = 1.0f / SAMPLING_RATE; // Sampling interval in seconds
    //    float periods = 1.0;

    //    auto w = F * 2 * PI * dt;
    //    int max_n = floor(periods * SAMPLING_RATE / F);
    //    printf("max_n = %d\n", max_n);
    //    for (int i = 0; i < max_n; i++)
    //    {
    //        gWave[i] = (int) (AMP * sin(w * i + PHASE) + BIAS);
    //    }
    //}

    gCtrlFromFPGA[0] = 0.0f;
    gCtrlFromFPGA[1] = 0.0f;

    //WARNING: DON'T CHANGE THE SEQUENCE BELOW
    StartReadPos(&gEncoderHandle[0], &gEncoderHandle[1]);


    StartSignalLoop(&gAOTaskHandle, &gForceReadTaskHandle); 
    InitMotor(&gCurrMotorState);

}


//#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
 
#define SERVER "192.168.0.2"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8899   //The port on which to listen for incoming data

struct sockaddr_in si_other;
int sock, slen;
char buf[BUFLEN];
char message[BUFLEN];
WSADATA wsa;
 
int initUdpEmg()
{
    slen=sizeof(si_other);
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("\n\nInitialised.\n\n");
     
    //create socket
    if ( (sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
    return 0;
}

char strSendUdp[100] = "wakawaka";

int updateUdpEmg(float32 inputVal)
{
    sprintf_s(strSendUdp, "%.4f", inputVal);

    //send the message
    if (sendto(sock, strSendUdp, strlen(strSendUdp) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
         
    //receive a reply and print it
    ////clear the buffer by filling null, it might have previously received data
    //memset(buf,'\0', BUFLEN);
    ////try to receive some data, this is a blocking call
    //if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
    //{
    //    printf("recvfrom() failed with error code : %d" , WSAGetLastError());
    //    exit(EXIT_FAILURE);
    //}
    //     
    //puts(buf);
 
 
    return 0;
}

int closeUdpEmg()
{
    closesocket(sock);
    WSACleanup();
    return 0;
}


inline void LogData( void)
{   
    // Approximately 100 Hz Recording
    double actualTime;
    QueryPerformanceCounter(&gCurrentTick);
    actualTime = gCurrentTick.QuadPart - gInitTick.QuadPart;
    actualTime /= gClkFrequency.QuadPart;
    //if (gEmgSoundOn)
    //{ 
    //    updateUdpEmg(gEmgBic);
    //    //printf("\t%.3lf",gEmgBic );																	

    //}
    if (gIsRecording)
    {   
        fprintf(gDataFile,"%.3lf\t",actualTime );																	

        fprintf(gDataFile,"%f\t%f\t", gCtrlFromFPGA[0], gCtrlFromFPGA[1]);			
        fprintf(gDataFile,"%f\t%f\t", gMuscleLce[0], gMuscleLce[1]);
        fprintf(gDataFile,"%f\t%f\t", gMuscleVel[0], gMuscleVel[1]);
        fprintf(gDataFile,"%f\t%f\t", gAuxvar[2], gAuxvar[6]);
        fprintf(gDataFile,"%f\t%f\t%f\t", gMusKse, gMusKpe, gMusB);
        
        //fprintf(gDataFile,"%f\t%f\t%f\t%f\t%f\t%f\t", gSpindleIaBic, gSpindleIaTri, gSpindleIIBic, gSpindleIITri, gMusDamp, -gAuxvar[2]);			
        //gSpikeCountBic, gSpikeCountTri);			
        fprintf(gDataFile,"\n");
        
    }    
        
        //printf("\n%lf",gEmgBic);
        
        
//updateUdpEmg(gEmgBic);
        //updateUdpEmg(3.555);
    
}


void* NoTimerCB (void *)
{
    while (1)
    {
        LogData();
        //if (gIsP2pMoving) // Update the neutral joint angle using mini-jerk
        //{
        //    float   af = 0.6f, d = 200.0f;

        //    gP2pIndex += 1.0f;

        //    gDeltaLen = min(af, af*(10.0f*pow(gP2pIndex/d, 3) - 15.0f*pow(gP2pIndex/d, 4) + 6.0f*pow(gP2pIndex/d, 5)));

        //    
        //}

        Sleep(1);
    }
}
void ExitProgram();

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
    gLenOrig[0]=0.0;
    gLenOrig[1]=0.0;
    
    gM1Voluntary= 0.0;
    gM1Dystonia= 0.0;
    //gLenScale=0.0001;
    
    //Lua Init
    L = lua_open();
	luaL_openlibs(L);
	luaopen_base(L);
	luaopen_table(L);
    luaopen_string(L);
    luaopen_math(L);
    char filename[40] = "..\\source\\sendUdp.lua";
	std::cerr << "-- Loading file: " << filename << std::endl;
	statusLua = luaL_loadfile(L, filename);


    FILE *ConfigFile;
    ConfigFile= fopen("ConfigPXI.txt","r");

    fscanf(ConfigFile,"%f\t%f",&gLenScale[0],&gLenScale[1]);

    fclose(ConfigFile);
    glutInit( &argc, argv ); // Erm Just Write It =)
    init();

    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
    glutInitWindowSize( 800, 600); // If glutFullScreen wasn't called this is the window size
    glutCreateWindow( "OpenGL Graph Component" ); // Window Title (argv[0] for current directory as title)
    glutDisplayFunc( display );  // Matching Earlier Functions To Their Counterparts
    glutReshapeFunc( reshape );

    TwInit(TW_OPENGL, NULL);

    glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
    glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
    glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
    glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
    glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
    // TODO: Code the TimerCB() to log data
    //printf("\n\t%f",gMuscleLce); 
    //***** RELOCATE THIS -->   
    //glutTimerFunc(3, TimerCB, 1);

    // send the ''glutGetModifers'' function pointer to AntTweakBar
    TwGLUTModifiersFunc(glutGetModifiers);

    glutKeyboardFunc( keyboard );
    glutIdleFunc(idle);

    initUdpEmg();


    // Make sure to pair InitProgram() with ExitProgram()
    // Resources need to be released  
    InitProgram();
   
    atexit(ExitProgram);


    // gAuxvar = {current force 0, current force 1, current pos 0, current pos 1};

    pthread_mutex_init (&gMutex, NULL);
    int logdata_handle = pthread_create(&gThreads[0], NULL, NoTimerCB,	NULL);
    int ctrl_handle_bic = pthread_create(&gThreads[1], NULL, ControlLoopBic,	(void *)gAuxvar);
    int ctrl_handle_tri = pthread_create(&gThreads[2], NULL, ControlLoopTri,	(void *)gAuxvar);
   
    gBar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='This is our interface for the T5 Project BBDL-SangerLab.' "); // Message added to the help bar.
    TwDefine(" TweakBar size='400 200' color='96 216 224' "); // change default tweak bar size and color

    // Add 'g_Zoom' to 'bar': this is a modifable (RW) variable of type TW_TYPE_FLOAT. Its key shortcuts are [z] and [Z].
    TwAddVarRW(gBar, "Gain", TW_TYPE_FLOAT, &gLenScale, 
               " min=0.0000 max=0.0002 step=0.000001 keyIncr=l keyDecr=L help='Scale the object (1=original size).' ");
    //TwAddVarRW(gBar, "M1Voluntary", TW_TYPE_INT32, &gM1Voluntary, 
    //           " min=0.00 max=500000.00 step=40000 ");
    TwAddVarRW(gBar, "M1Dystonia", TW_TYPE_INT32, &gM1Dystonia, 
               " min=0.00 max=500000.00 step=20000 ");

    glutMainLoop( );          // Initialize The Main Loop  

    return 0;

}


void ExitProgram() 
{
    //pthread_join(gThreads[0], NULL);
    SaveConfigCache();
    DisableMotors(&gEnableHandle);    
    StopSignalLoop(&gAOTaskHandle, &gForceReadTaskHandle);
    StopPositionRead(&gEncoderHandle[0],&gEncoderHandle[1]);

    report_errors(L, statusLua);
    lua_close(L);
    //IPP
    //ippsFIRFree_32f(pFIRState0);
    //ippsFIRFree_32f(pFIRState1);
    
    
    closeUdpEmg();

    ippsFree(taps0);
    ippsFree(taps1);
    ippsFree(dly0);
    ippsFree(dly1);


    ippsIIRFree_32f(pIIRState0);
    ippsIIRFree_32f(pIIRState1);
    

    //IPP_VEL_IIR

    ippsFree(tapsVel0IIR);
    ippsFree(tapsVel1IIR);
    ippsFree(dlysVel0IIR);
    ippsFree(dlysVel1IIR);

    ippsIIRFree_32f(pIIRStateVel0);
    ippsIIRFree_32f(pIIRStateVel1);
 
    
    TwDeleteBar(gBar);

    TwTerminate();    
    delete gSwapFiles;


}
