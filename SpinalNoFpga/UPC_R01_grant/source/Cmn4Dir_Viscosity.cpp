/*
*	Name:			cmn4dir_Viscosity.cpp

*	Function:		HapticMaster is allowed to move on a horizontal plane,
					enclosed by a tunnel. Back and forth movements, servoed-
					perturbation implemented.

*	Author:			Minos Niu

*	Email:			minos.niu AT gmail.com

*	Date:			03-14-2008
*/



#include	"Utilities.h"
#include	"ExampleLib.h"
#include	<FcsHapticMaster.h>
#include	<FcsSpring.h>
#include	<FcsDamper.h>
#include	"FcsBlock.h"
#include	"FcsConstantForce.h"
#include	<fstream>
#include	<Inventor/SbLinear.h>
#include	<HapticAPI.h>
#include	<GL/glut.h>
#include	<math.h>
#include	<pthread.h>
#include	<ScopeAPI.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<time.h>
#include	<process.h>
#include	"NIDAQmx.h"
#include	"EmgDAQ.h"
#include	<ReadParadigm.h>
#include	<TriggerOutput.h>

using namespace std;

FILE*		emgLogHandle;	// EMG data file handle.
TaskHandle 	emgDaqHandle = 0;
ParaType 	gThisPara;

/* 
	Display the cursor or not
	1 - Always display; 0 - Display when moving slowly
*/
int 		gDispCursor = 1;
int 		gDispTarget = 1;
int 		gTargetCountTh = 0;
int 		gCursorCountTh = 0;
int 		gEvent = FLG_EVENT_STOP;
int 		gPractice = FLG_PRACTICE_OFF;
double		gVisualGain = 0.02; // Length:Force
int 		gPtb = 0;

int         error=0;
char        errBuff[2048]={'\0'};

string		numStr("");
string		subjectName("Out");	// Data File information
string		paraName("");

int			flag = 0;
double		ZERO_VISCOSITY[3] = {0.0, 0.0, 0.0};
double		ptbB[MAX_PTB_NUM][3] = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 
{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};

double		PosTol = 0.01;
double		onTargetTime = 1;

/* 0 = OFF; 1 = ON; 2 = Threshold */
int			cursorExpContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int			targetExpContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int			iniposExpContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

int			cursorPtbContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int			targetPtbContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};
int			iniposPtbContext[PRACTICE_NUM][EVENT_NUM] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

/*
	Thread variables
*/
pthread_t	threads[NUM_THREADS];
pthread_mutex_t 
			mutexPosition;

CFcsHapticMASTER 
			*pHapticMaster = NULL;
CFcsSpring 	*pSpring;
CFcsDamper 	*pDamper;
CFcsBlock 	*pCube1;
CFcsBlock 	*pCube2;
CFcsBlock	*pFloor, *pSlide;

double		gCurrentPos[3];
double 		gCurrentVel[3];
double 		gCurrentForce[3];
double 		gJointVel = 0.0;
double 		gJointForce = 0.0;
double 		gPkVel = -1000.0;
int 		gListening = 0; // 0 - Not listerning; 1- Listening, time demanding!!!

int			gPause = 0;	// Pause flag
int			gStartTrial = 1;

// Properties for the CFcsSpring object created in main()
double		springPos[3] = {0.0, 0.0, 0.0};
double		springDir[3] = {0.0, 0.0, 0.0};
double		springStiffness = 20000.0;
double		springDamping = 0.0;
double		springDeadband = 0.0;

// Location, Orientation And Size Parameters For The Haptic Floor Plane Object
double		floorCenter[3] = {0.0, 0.0, 0.0};
double		floorOrient[3] = {0.0, 0.0, 0.0};
double		floorSize[3] = {0.8, 0.8, 0.1};

//double DamperCoeff[3] = {63.20, 63.20, 63.20};
double		DamperCoeff[3] = {1789.0, 1789.0, 1789.0};

//Location, Orientation and Size of Haptic Cube1
double		CubeCenter1[3] = {0, 0, 0.01};
double		CubeOrient1[3] = {0.0, 0.0, 0.0};
double		CubeSize1[3] = {0.8, 0.8, 0.02};

//Location, Orientation and Size of Haptic Cube2
double		CubeCenter2[3] = {0, 0, -0.01};
double		CubeOrient2[3] = {0.0, 0.0, 0.0};
double		CubeSize2[3] = {0.8, 0.8, 0.02};

GLfloat		gXPrep = 0.0, gYPrep = 0.0, gZPrep = 0.0;

// EndEffector OpenGL Material Parameters.
GLfloat		EndEffectorAmbient[] = {0.91, 0.44, 0.00, 1.00};
GLfloat		EndEffectorDiffuse[] = {0.90, 0.38, 0.00, 1.00};

// Targets OpenGL Material Parameters.
GLfloat		TargetAmbient[2][4] = {{0.00, 0.44, 0.91, 0.50}, {0.44, 0.00, 0.91, 0.50}};
GLfloat		TargetDiffuse[] = {0.90, 0.38, 0.00, 1.00};


// Target2 OpenGL Material Parameters.
GLfloat		InitialPositionAmbient[] = {0.00, 0.44, 0.91, 0.50};
GLfloat		InitialPositionDiffuse[] = {0.90, 0.38, 0.00, 1.00};


// Peak Velocity Indicator Material Parameters.
GLfloat		PkVelAmbient[] = {0.91, 0.44, 0.00, 1.00};
GLfloat		PkVelDiffuse[] = {0.90, 0.38, 0.00, 1.00};

// Acceptable velcity zone Material Parameters.
GLfloat		VelZoneAmbient[] = {0.5, 0.5, 0.5, 0.25};
GLfloat		VelZoneDiffuse[] = {0.90, 0.38, 0.00, 1.00};

// Target Force OpenGL Material Parameters.
GLfloat		AppliedForceAmbient[] = {1.0, 0.5, 0.0, 1.0};
GLfloat		AppliedForceDiffuse[] = {0.90, 0.38, 0.00, 1.00};

// Target Force OpenGL Material Parameters.
GLfloat		ForceConeAmbient[] = {1.0, 0.5, 0.0, 1.0};
GLfloat		ForceConeDiffuse[] = {0.90, 0.38, 0.00, 1.00};

// Floor Object OpenGL Material Parameters
GLfloat		FloorAmbient[] = {1.00, 1.00, 0.00, 1.00};
GLfloat		FloorDiffuse[] = {1.00, 1.00, 0.00, 1.00};

// General OpenGl Material Parameters.
GLfloat		Specular[] = {1.00, 1.00, 1.00, 1.00};
GLfloat		Emissive[] = {0.00, 0.00, 0.00, 1.00};
GLfloat		Shininess = {128.00};

GLfloat		SpecularOff[] = {0.00, 0.00, 0.00, 0.00};
GLfloat		EmissiveOff[] = {0.50, 0.50, 0.50, 0.00};
GLfloat		ShininessOff = {0.00};

//extern	int 

void
EndEffectorMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, EndEffectorAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, EndEffectorDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

// TargetMaterial() Sets The Current OpenGl Material Paremeters. 
// Call This Function Prior To Drawing The Targets.
void
TargetMaterial(void)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, TargetAmbient[(int) gEvent == FLG_EVENT_REACH]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, TargetDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

void
MiddleTargetMaterial(void)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, TargetAmbient[1]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, TargetDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

void 
InitialPositionMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, InitialPositionAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, InitialPositionDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}


void
PkVelMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, PkVelAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, PkVelDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

void
VelZoneMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, VelZoneAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, VelZoneDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

// ObjectMaterial() Sets The Current OpenGl Material Parameters. 
// Call This Function Prior To Drawing The Floor Object.
void 
FloorMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, FloorAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, FloorDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, SpecularOff);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, ShininessOff);
}

void 
AppliedForceMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, AppliedForceAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, AppliedForceDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

void 
ForceConeMaterial()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ForceConeAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ForceConeDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Emissive);
	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);
}

//-----------------


// Countdown Timer for countdown before next trial.
void 
CountdownTimer(float seconds, double Show)
{

	LARGE_INTEGER	sample1;
	LARGE_INTEGER	sample2;
	LARGE_INTEGER	frequency;

	double		cdTime = seconds;	// For OpenGL display of cdTimer
	double		time = 0.0;
	double		actualTime = 0;

	QueryPerformanceCounter(&sample1);
	QueryPerformanceFrequency(&frequency);
	while (time < seconds)
	{
		//Setup the counting loop
		QueryPerformanceCounter(&sample2);
		actualTime = sample2.QuadPart - sample1.QuadPart;
		actualTime /= frequency.QuadPart;
		if (actualTime >= 1.0)
		{ 

			QueryPerformanceCounter(&sample1);
			time = time + actualTime;
			cdTime = int(seconds)-time;

		}
	}

}


void 
ParseCursorContext(int code, double *xc, double *yc, double *zc)
{
	//doubl<e xx = 0.0;
	//double yy = 0.0;
	//double zz = 0.0;
	double currentDistance;

	switch (code)
	{
	case 0:
		gDispCursor = 0;
		break;
	case 2:
		PthreadMutexLock(&mutexPosition);
		if (pHapticMaster != NULL)
		{
			pHapticMaster -> GetParameter(FCSPRM_VELOCITY, gCurrentVel);
			pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);
		}
		PthreadMutexUnlock(&mutexPosition);
		currentDistance = sqrt(pow(gCurrentPos[0] - gThisPara.xInit, 2) + pow(gCurrentPos[1] - gThisPara.yInit, 2));

		if (currentDistance > DISPLAY_POS_TH)
		{
			gDispCursor = 0;
			gCursorCountTh++;
		}
		else if ((currentDistance <= DISPLAY_POS_TH) && (gCursorCountTh == 0))
		{
			gDispCursor = 1;
		}
		break;
	case 1:
		gDispCursor = 1;
		break;
	}

	*xc = gCurrentPos[0];
	*yc = gCurrentPos[1];
	*zc = gCurrentPos[2];
}

void 
ParseTargetContext(int code, double *xt, double *yt, double *zt)
{
	double xx = gThisPara.xTarget;
	double yy = gThisPara.yTarget;
	double zz = gThisPara.zTarget;
	double currentDistance;

	switch (code)
	{
	case 0:
		gDispTarget = 0;
		break;
	case 1:
		gDispTarget = 1;
		break;	
	case 2:
		PthreadMutexLock(&mutexPosition);
		if (pHapticMaster != NULL)
		{
			//pHapticMaster -> GetParameter(FCSPRM_VELOCITY, gCurrentVel);
			pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);
		}
		PthreadMutexUnlock(&mutexPosition);
		//gJointVel = sqrt(pow(gCurrentVel[0], 2) + pow(gCurrentVel[1], 2));
		if (gPtb != 0)
		{
		currentDistance = sqrt(pow(gCurrentPos[0] - gThisPara.xInit, 2) + pow(gCurrentPos[1] - gThisPara.yInit, 2));
		}
		else
		{
		currentDistance = sqrt(pow(gCurrentPos[0] - gThisPara.xInit, 2) + pow(gCurrentPos[1] - gThisPara.yInit, 2));
		};

		if (currentDistance > DISPLAY_POS_TH) 
		{
			gDispTarget = 0;
			gTargetCountTh++;
		}
		else if ((currentDistance <= DISPLAY_POS_TH) && (gTargetCountTh == 0))
		{
			gDispTarget = 1;
		}
		break;
	}
	*xt = xx;
	*yt = yy;
	*zt = zz;
	return;
}



// This Function Is Called To Draw The Graphic Equivalent Of 
// The EndEffector In OpenGl.
// The EndEffector Is Drawn At The Current Position

void 
DrawEndEffector(void)
{
	double xc, yc, zc;
	EndEffectorMaterial();
	glPushMatrix();

	if (gPtb == 0)
	{
		ParseCursorContext(cursorExpContext[gPractice][gEvent], &xc, &yc, &zc);
	}
	else
	{
		ParseCursorContext(cursorPtbContext[gPractice][gEvent], &xc, &yc, &zc);
	}

	if (gDispCursor)
	{
		PthreadMutexLock(&mutexPosition);
		if (pHapticMaster != NULL)
		{
			pHapticMaster -> GetParameter(FCSPRM_FORCE, gCurrentForce);
		}
		PthreadMutexUnlock(&mutexPosition);

		glTranslatef(gCurrentForce[0] * gVisualGain + gThisPara.xInit, gCurrentForce[1] * gVisualGain + gThisPara.yInit, 0.0);
		glutSolidSphere(0.005, 20, 20);
		glPopMatrix(); 

		//PkVelMaterial();
		//glLineWidth(4);
		//glBegin(GL_LINES);
		//glVertex2f(gThisPara.xInit, gThisPara.yInit);
		////glVertex2f(gThisPara.xInit + 1.0, gThisPara.yInit);
		//glVertex2f(gCurrentForce[0] * gVisualGain + gThisPara.xInit, gCurrentForce[1] * gVisualGain + gThisPara.yInit);    // Force Line.
		//glEnd();
	}
	else
	{
		glPopMatrix();
	}
}

void
DrawVelBar(void)
{
	// Draw the velocity indicator
	VelZoneMaterial();
	glLineWidth(10);
	glBegin(GL_LINES);
	glVertex2f(-0.215, 0.25 - 0.15 * 1.0 * 0.2);    // Vertical Line.
	glVertex2f(-0.215, 0.25 + 0.15 * 1.0 * 0.2);
	glEnd();
	if (gEvent == FLG_EVENT_REACH)
	{

		PkVelMaterial();
		glLineWidth(4);
		glBegin(GL_LINES);
		glVertex2f(-0.2, 0.25 + (gPkVel - 1.0) * 0.2);    // Vertical Line.
		glVertex2f(-0.23, 0.25 + (gPkVel - 1.0) * 0.2);
		glEnd();
	}
}


void 
DrawTargets(void)
{
	double xt = 0.0;
	double yt = 0.0;
	double zt = 0.0;

	if (gPtb == 0)
	{
		ParseTargetContext(targetExpContext[gPractice][gEvent], &xt, &yt, &zt);
	}
	else
	{
		ParseTargetContext(targetPtbContext[gPractice][gEvent], &xt, &yt, &zt);
	}

	if (gDispTarget != 0)
	{
		TargetMaterial();
		glPushMatrix();
		glTranslatef(xt, yt, zt);
		glutSolidSphere(0.015, 20, 20);
		glPopMatrix();
	}	
}

void 
DrawPrepIndicator(void)
{

	// Display Sphere for target 1
	MiddleTargetMaterial();

	if ((gListening != 0) && (gXPrep > gThisPara.xInit))
	{
		glPushMatrix();
		gXPrep = gXPrep - 0.001 ;
		// Draw targets by accessing gThisPara, need synchronizing
		glTranslatef(gXPrep, gYPrep, gZPrep);

		glutSolidSphere(0.005, 20, 20);
		glPopMatrix();		
	}
}

void 
DrawInitialPosition(void)
{
	if (gEvent > FLG_EVENT_INITPOS)
		return;
	// Display Sphere for target 1
	InitialPositionMaterial();

	glPushMatrix();
	glTranslatef(gThisPara.xInit,gThisPara.yInit,gThisPara.zInit);
	glutSolidSphere(0.005, 20, 20);
	glPopMatrix();		
}

void 
DrawFloor(void)
{
	FloorMaterial();
	glScalef(0.8, 0.10, 0.08);
	glTranslatef(0.0, 0.0, 0.04);
	glutSolidCube(1.0);
}

// This Function Initializes the OpenGl Graphics Engine
void 
InitOpenGl (void)
{

	glShadeModel(GL_SMOOTH);

	glLoadIdentity();

	GLfloat		GrayLight[] = {0.75, 0.75, 0.75, 1.0};
	GLfloat		LightPosition[] = {1.0, 2.0, 1.0, 0.0};
	GLfloat		LightDirection[] = {0.0, 0.0, -1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, GrayLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, GrayLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, GrayLight);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glEnable (GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.3, 0.0);
}





// This Function Connects To The HapticMASTER And
// Initializes It.
int 
InitHapticMaster(void)
{
	int returnValue = 0;
	FCSSTATE currentState;

	// Call ConnectToHapticMASTER To Get A Pointer To A
	// CFcsHapticMASTER Object.
	pHapticMaster = ConnectToHapticMASTER ("vmd");

	// THIS IS VERY IMPORTANT!!!! WITHOUT WHICH THE ARM WILL BUMP IN THE BEGINNING
	pHapticMaster -> DeleteAll();

	// If A HapticMASTER Was Not Found At The Specified IP Address 
	// In The 'SERVERS.DB' File NULL Is Returned.
	if (pHapticMaster == NULL) 
		returnValue = -1;
	else	
	{
		// First Check The Current HapticMASTER State
		pHapticMaster  ->  GetCurrentState (currentState);
		if  (currentState != FCSSTATE_NORMAL)
		{
			// Set The HapticMASTER State To Initialised
			pHapticMaster -> SetRequestedState ( FCSSTATE_INITIALISED);

			// Wait for The HapticMASTER To Enter The Initialized State
			while (currentState != FCSSTATE_INITIALISED)
			{
				pHapticMaster -> GetCurrentState (currentState);
				Sleep(100);
			}

			// Set The HapticMASTER State To NormalForce
			pHapticMaster -> SetRequestedState(FCSSTATE_NORMAL);

			// Wait For The HapticMASTER To Enter The NormalForce State
			while (currentState != FCSSTATE_NORMAL)
			{
				pHapticMaster -> GetCurrentState(currentState);
				Sleep(100);
			}
		}

		// Call pHapticMaster -> DeleteAll() To Delete All Haptic Objects
		// Currently Active In The HapticMASTER
		pHapticMaster -> DeleteAll();

		// Set The HapticMASTER Inertia To background inertia.
		pHapticMaster -> SetParameter (FCSPRM_INERTIA, BG_INERTIA);
	}

	return returnValue;
}


//  Initializes the Scope for data logging
void 
InitScope(void)
{

	int Ret = 0;

	printf("Waiting for connection to Scope...");


	// If connection succesfull
	if(pHapticMaster != NULL)
	{
		printf("Connected Scope\n");

		// Initialize the HapticMASTER

		// Clear a possible previous list of channels to sample
		Ret = pHapticMaster -> RemoveAllScopeChannels();

		printf("Adding Scope Channels\n");

		// Add some new channels to sample
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_FORCEX);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_FORCEY);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_FORCEZ);

		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_POSX);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_POSY);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_POSZ);

		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_VELX);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_VELY);
		Ret = pHapticMaster -> AddScopeChannel(FCSPRM_VELZ);

	}
	else
	{
		printf("Scope Not Connected\n");
	}


	// First check whether the Scope mechanism is supported
	if( pHapticMaster -> SupportScope() != SCOPE_NOT_SUPPORTED)
	{

		// Tell the ScopeAPI the IP address 
		ScopeSetIpAddress1(gThisPara.IP[0], gThisPara.IP[1], gThisPara.IP[2], gThisPara.IP[3]);

		// Connect only once during the application
		// Trials with different sample data files are
		// created by starting and stopping the scope 
		// and changing the sample dataFile name inbetween
		if( ScopeConnect() == SCOPE_CONNECTED )
		{
			printf("Scope Connected... \n");


		} // if connected
		else 
		{
			printf("Scope not connected");
		}
	}
	else
		printf("Scope NOT Supported... \n");
}


// This Function Is Called By OpenGL To Redraw The Scene
// Here's Where You Put The EndEffector And Block Drawing FuntionCalls
void 
Display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix ();

	// define eyepoint in such a way that
	// drawing can be done as in lab-frame rather than sgi-frame
	// (so X towards user, Z is up)

	gluLookAt (0.0, 0.0000001, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1);

	//gluLookAt (1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1);
	glRotated(90, 0, 0, 1);
	glutPostRedisplay();

	DrawTargets();
	DrawPrepIndicator();
	DrawEndEffector();
	DrawInitialPosition();
	DrawVelBar();

	//DrawFloor();

	glPopMatrix ();
	glutSwapBuffers();

}


// The Function Is Called By OpenGL Whenever The Window Is Resized
void 
Reshape(int iWidth, int iHeight)
{
	glViewport (0, 0, (GLsizei)iWidth, (GLsizei)iHeight);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	float fAspect = (float)iWidth/iHeight;
	gluPerspective (30.0, fAspect, 0.05, 20.0);            

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}


// This Function Is Called By OpenGl WhenEver A Key Was Hit
void 
Keyboard(unsigned char ucKey, int iX, int iY)
{
	switch (ucKey) 
	{
	case 27:	// for ESC
		if (pHapticMaster != NULL)
		{
			pHapticMaster -> DeleteAll();
			pHapticMaster -> SetRequestedState (FCSSTATE_INITIALISED);
		}
		exit(0);
		break;

	case 112:	// for 'p', lower case
		if (gPause != 1)
		{
			gPause = 1;
		}
		else
			gPause = 0;
		break;

	case 32:	// for Space
		gPractice = (gPractice + 1) % 3;
		break;
	default :
		break;
	}
}

void 
TimerCB (int iTimer)
{

	// Get The Current EndEffector Position From THe HapticMASTER

	// Set The Timer For This Function Again
	glutTimerFunc (10, TimerCB, 1);
}


void 
CheckPosition (double tx, double ty, double tz)
{
	double Px, Py, Pz;

	//printf("\nChecking...\n");
	PthreadMutexLock (&mutexPosition);
	if (pHapticMaster != NULL)
	{
		pHapticMaster -> GetParameter(FCSPRM_POSX, Px);
		pHapticMaster -> GetParameter(FCSPRM_POSY, Py);
		pHapticMaster -> GetParameter(FCSPRM_POSZ, Pz);
	}
	PthreadMutexUnlock (&mutexPosition);



	while ((abs(tx-Px) > 0.001) || 
		(abs(ty-Py) > 0.001) || 
		(abs(tz-Pz) > 0.001))
	{
		PthreadMutexLock (&mutexPosition);

		if (pHapticMaster != NULL){
			pHapticMaster -> GetParameter(FCSPRM_POSX, Px);
			pHapticMaster -> GetParameter(FCSPRM_POSY, Py);
			pHapticMaster -> GetParameter(FCSPRM_POSZ, Pz);}
		PthreadMutexUnlock (&mutexPosition);

	}
	//printf("Checked!!!\n");

}


/* Testing if the target has been reached */
void 
CheckReachTarget(double x, double y, double z, double time)
{
	double xyDistance = 0.0;	// Distance between currentPos and target
	static double	
		reachTargetTimer = 0.0,	// Time elapsed since slowed below REACH_TARGET_VEL
		reachTargetOnset = 0.0;	// Time onset when firstly slowed below REACH_TARGET_VEL
	static int
		reachTargetFlag = 0; // 0: not slowing down; 1: slowing down not reached; 2: slowing down and reached

	if (gEvent == FLG_EVENT_REACH)
	{
		return;
	}

	PthreadMutexLock (&mutexPosition);
	if (pHapticMaster) 
	{
		pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);
		pHapticMaster -> GetParameter(FCSPRM_VELOCITY, gCurrentVel);// Reading velodity information
	}
	PthreadMutexUnlock (&mutexPosition);

	// Once reached the target, the OnTarget will be 1 till the end of current trial
	xyDistance = sqrt(pow(gCurrentPos[0]-x,2)+pow(gCurrentPos[1]-y,2));
	gJointVel = sqrt(pow(gCurrentVel[0], 2) + pow(gCurrentVel[1], 2));

	if (reachTargetFlag == 2)
	{
		reachTargetFlag = 0;
		reachTargetTimer = 0.0;
		return;
	}
	if (gJointVel > REACH_TARGET_VEL)
	{
		reachTargetFlag = 0;
		reachTargetTimer = 0.0;
		return;
	}	
	if ((reachTargetFlag == 0) && (gJointVel <= REACH_TARGET_VEL))
	{
		reachTargetFlag = 1;
		reachTargetOnset = time;
		reachTargetTimer = 0.0;
		return;
	}
	if ((reachTargetFlag == 1) 
		&& (reachTargetTimer < REACH_TARGET_DELAY)) //0.3s
	{
		reachTargetTimer += time - reachTargetOnset;
		return;
	}	
	if ((reachTargetFlag == 1) 
		&& (xyDistance <= REACH_TARGET_DIST) && (gEvent < FLG_EVENT_REACH)) // 0.2
	{
		reachTargetFlag = 2;
		gEvent = FLG_EVENT_REACH; // Move the Target
		//	printf("Target Reached!\n");
		return;
	}				
}

void 
Servo3D(float xf, float yf, float zf, float duration, float rate)
{

	LARGE_INTEGER sample1;
	LARGE_INTEGER sample2;
	LARGE_INTEGER frequency;

	//period = SpringMoveSpeed; // 200Hz sampling frequency
	//float duration = 1.0;

	double	elapsedTime = 0.0,
		lastTime = 0.0;

	double tau;
	double dCoef[3] = {0.0, 0.0, 0.0};
	double xi, yi, zi; 

	if (pFloor != NULL)
		pFloor -> Disable();
	if (pSlide != NULL)
		pSlide -> Disable();

	QueryPerformanceCounter (&sample1);
	QueryPerformanceFrequency (&frequency);


	dCoef[0] = 1789.0;  dCoef[1] = 1789.0; dCoef[2] = 1789.0;
	// Enable the damper
	if (pDamper != NULL)
		pDamper -> SetBaseParameters(dCoef);
	// Getting the current spring position


	PthreadMutexLock (&mutexPosition);
	if (pHapticMaster) 
		pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);

	PthreadMutexUnlock (&mutexPosition);
	pSpring -> SetParameter(FCSPRM_POSITION, gCurrentPos);

	if (pSpring != NULL)
		pSpring -> Enable();


	xi = gCurrentPos[0];
	yi = gCurrentPos[1];
	zi = gCurrentPos[2];

	do 
	{

		//Setup the sampling loop
		QueryPerformanceCounter(&sample2);
		elapsedTime = sample2.QuadPart - sample1.QuadPart;
		elapsedTime /= frequency.QuadPart;

		//QueryPerformanceCounter(&sample1);
		if ((elapsedTime - lastTime) < 1 / rate)
		{
			continue;
		}				

		//Move Along a Minimum Jerk Trajectory
		tau = elapsedTime / duration;
		//springPos[0] = xi + (xi - xf) * (-tau);
		//	springPos[1] = yi + (yi - yf) * (-tau);
		//	springPos[2] = zi + (zi - zf) * (-tau);
		gCurrentPos[0] = xi + (xi - xf) * ((15.0 * pow(tau, 4.0)) 
			- (6.0 * pow(tau, 5.0)) - (10.0 * pow(tau, 3.0)));
		gCurrentPos[1] = yi + (yi - yf) * ((15.0 * pow(tau, 4.0)) 
			- (6.0 * pow(tau, 5.0)) - (10.0 * pow(tau, 3.0)));
		gCurrentPos[2] = zi + (zi - zf) * ((15.0 * pow(tau, 4.0)) 
			- (6.0 * pow(tau, 5.0)) - (10.0 * pow(tau, 3.0)));
		//printf("%f %f %f\n",springPos[0],springPos[1],springPos[2]);
		//PthreadMutexLock (&mutexPosition);
		pSpring -> SetParameter(FCSPRM_POSITION, gCurrentPos);
		//PthreadMutexUnlock (&mutexPosition);

		lastTime = elapsedTime;
	} 
	while (elapsedTime < duration);

}

void 
FreeMove3D(int iCurrentTrial, double xi, double yi, double zi, 
		   double xf, double yf, double zf, double elasticInitPosition,
		   double duration, double rate)
{

	LARGE_INTEGER sample1, sample2, frequency;
	double elapsedTime = 0.0, lastTime = 0.0;
	int ptbFlag = FLG_BELOW_DELAY;
	double dCoef[3] = {0.0, 0.0, 0.0};

	gCursorCountTh = 0;
	gTargetCountTh = 0;

	if (gThisPara.sequence[iCurrentTrial - 1] != 0)
		gListening = 1;

	QueryPerformanceCounter(&sample1);
	QueryPerformanceFrequency(&frequency);

	if (gThisPara.sequence[iCurrentTrial - 1] != 0) 
		printf("\nPertType = %d\n", gThisPara.sequence[iCurrentTrial - 1]);


	do //while  ((elapsedTime < duration) && (gEvent < FLG_EVENT_REACH));
	{
		PthreadMutexLock(&mutexPosition);
		if (pHapticMaster)
		{
			pHapticMaster -> GetParameter(FCSPRM_FORCE, gCurrentForce);
			pHapticMaster -> GetParameter(FCSPRM_VELOCITY, gCurrentVel);
			pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);
		}
		PthreadMutexUnlock(&mutexPosition);

		gJointVel = sqrt(pow(gCurrentVel[0],2)+pow(gCurrentVel[1],2));

		if (fabs(gCurrentVel[0]) > gPkVel)
		{
			gPkVel = fabs(gCurrentVel[0]);
		}

		gJointForce = gCurrentForce[0]*gCurrentForce[0] + gCurrentForce[1]*gCurrentForce[1];
		if ((gListening == 1) 
			&& (gCurrentVel[0] < -gThisPara.trigThreshold)
			&& (gCurrentForce[0] < 0))
			//			&& (gJointForce >= gThisPara.trigThreshold))
			//			&& (ptbFlag == FLG_ABOVE_DELAY_BELOW_THRESHOLD))
		{
		}

		if (gEvent < FLG_EVENT_REACH)
		{
			//Calculate the time for Loop 1
			QueryPerformanceCounter(&sample2);
			elapsedTime = sample2.QuadPart - sample1.QuadPart;
			elapsedTime /= frequency.QuadPart;

			if (elapsedTime > duration)
			{
				gEvent = FLG_EVENT_REACH;
				//Beep(823, 500);
				continue;
			}


			if ((elapsedTime - lastTime) < 1 / rate)
			{
				continue;
			}				

			lastTime = elapsedTime;
			if (elapsedTime > 1.0)
				CheckReachTarget(xf, yf, zf, elapsedTime);
		}
	} 
	while  ((elapsedTime < duration) && (gEvent < FLG_EVENT_REACH));

	//CountdownTimer(1.0,0);

	dCoef[0] = 1789.0;  
	dCoef[1] = 1789.0; 
	dCoef[2] = 1789.0;

	// Enable the damper
	if (pDamper != NULL)
		pDamper -> SetBaseParameters(dCoef);

	//printf("Damping increased!!!!!!!!!!!!!!!\n");

	if (pHapticMaster != NULL)
	{
		pHapticMaster -> GetParameter(FCSPRM_POSITION, gCurrentPos);
		pHapticMaster -> SetParameter(FCSPRM_INERTIA, BG_INERTIA);
	}

	// Enable the spring
	if (pSpring != NULL)
	{
		pSpring -> SetParameter(FCSPRM_POSITION, gCurrentPos);
		pSpring -> Enable();
	}
}


void
ReadPtbPara(void)
{
	FILE* hpPerturbHandle;
	string perturbFile("");
	for (int ii = 0; ii < gThisPara.ptbTypeNum; ii++)
	{
		ostringstream oss;
		oss << (ii + 1);
		numStr = oss.str();
		perturbFile = "C:/HapticMASTER/People/Mark/Protocol/"
			+ paraName + "_" + numStr + ".ptb";
		cout << perturbFile << endl;

		fopen_s(&hpPerturbHandle, ToChar(perturbFile),"r");

		if (hpPerturbHandle == NULL)
		{
			printf("\nError reading .ptb file!\n");
			break;
		}
		else
			printf("\nPerturbation file found!\n");
		
		fscanf_s(hpPerturbHandle, "%lf\n", &ptbB[ii]);

		// Close protocol file
		if (hpPerturbHandle != NULL)
		{
			fclose(hpPerturbHandle);
		}
	}
}



// This Fucntion performs the Experimental Protocol
void* 
HapticProtocol(void*)
{

	float duration = 1.0;
	char	finishExp = 'N';
	char	keepData = 'Y';

	string taskFile(""), sequenceFile(""), perturbFile(""), tempEmgFile(""), 
		dataFile(""), emgFile(""), rawSequence(""), tempDatFile("");

	double dCoef[3];
	int iCurrentTrial;

	do //while (toupper(finishExp) != 'Y');
	{
		if (pHapticMaster != NULL)
		{
			floorCenter[2] = gThisPara.zInit - 0.05;
			pFloor = pHapticMaster -> CreateBlock(floorCenter, floorOrient, floorSize, 20000.0, 20000.0);
		}

		cout << "\n xInit = " << gThisPara.xInit << gThisPara.yInit << endl;
		printf("\nStart from which trial:");
		scanf_s( "%d", &gStartTrial);
		printf ("\n%d trials are to be conducted.\n", 
				gThisPara.trialNum/* - gStartTrial + 1*/);

		ReadPtbPara();
		ReadDispPara();

		iCurrentTrial = gStartTrial;
		do //while (iCurrentTrial <= gThisPara.trialNum); 
		{
			gPkVel = 0.0;
			gEvent = FLG_EVENT_STOP;
			gPtb = gThisPara.sequence[iCurrentTrial - 1];

			printf("\nTrial No. = %d ", iCurrentTrial);
			if (gPtb != 0)
			{
				printf("Perturbation %d pending...", gPtb);
			}
			
			ostringstream oss;
			oss << iCurrentTrial;
			numStr = oss.str();

			tempDatFile = "C:/HapticMASTER/People/Mark/Data/temp_" +
				numStr + ".dat";
			tempEmgFile = "C:/HapticMASTER/People/Mark/Data/temp_" +
				numStr + ".emg";

			FILE *fp1;
			FILE *fp2;
			fopen_s(&fp1, "tempDatFile","r");
			fopen_s(&fp2, "tempEmgFile","r");

			if( fp1 || fp2 ) 
			{
				// exists
				fclose(fp1);
				fclose(fp2);
				printf("\nData file exists! Continue? (y/N)");
				char ignoreOverwrite;
				do
				{
					cin >> ignoreOverwrite;
				}
				while (toupper(ignoreOverwrite) == 'Y');
			} 

			ScopeSetDataFileName(ToChar(tempDatFile));
			fopen_s(&emgLogHandle, ToChar(tempEmgFile), "w");	//unified temp filename for EMG data

			gEvent = FLG_EVENT_INITPOS;
			printf("\n\nTarget code = %d", targetPtbContext[gPractice][gEvent]);
			Servo3D(gThisPara.xInit, gThisPara.yInit, gThisPara.zInit, 2.0, 100); // Servoing to the initposition
			
			//Prepare Scope
			if ( ScopeSetSampleRate(SCOPE_RATE_2500HZ) != SCOPE_NO_ERROR)
				printf("Error setting SYNC rate to 2500 Hz\n");

			if ( ScopeSetSyncRate(SCOPE_RATE_2500HZ) != SCOPE_NO_ERROR)
				printf("Error setting Syncpulse rate to 1250 Hz\n");

			// In the new scope version, set the Fixed sample count to
			// the exact nr of samples, instead of adding one
			//if (ScopeSetFixedSampleCount(500) != SCOPE_NO_ERROR)
			//printf("Error setting Samples");

			if ( ScopeConfigure() != SCOPE_NO_ERROR)
				printf("Error configuring rate\n");

			// Trial starts; prepIndicator moves; HM handle moves
			gListening = -1;

			//Start of trial
			if (ScopeStart() != SCOPE_NO_ERROR)
				printf("Scope NOT Started\n");


			// Move the handle
			gEvent = FLG_EVENT_PREP; // Move the Target
			CountdownTimer(1.0, 0);
			if (pFloor != NULL)
				pFloor -> Enable();
			if (pSpring != NULL)
				pSpring -> Disable();
			if (pHapticMaster != NULL)	// Background inertia + controlled inertia
			{
				pHapticMaster -> SetParameter (FCSPRM_INERTIA, BG_INERTIA + gThisPara.loadCoef[0]);
			}

			if (pDamper != NULL)
			{
				dCoef[0] = 0.0 + gThisPara.loadCoef[1];  // Some damping + controlled damping
				dCoef[1] = 0.0 + gThisPara.loadCoef[1]; 
				dCoef[2] = 0.0 + gThisPara.loadCoef[1];
				pDamper -> SetBaseParameters(dCoef);
			}

			Beep(700, 100);
			if (gThisPara.sequence[iCurrentTrial - 1] != 0) 
			{
				gEvent = FLG_EVENT_PTB;
				if (pDamper != NULL)
				{
					pDamper -> SetBaseParameters(ptbB[gPtb - 1]);
				}

				FreeMove3D(iCurrentTrial, gThisPara.xInit, gThisPara.yInit, 
					gThisPara.zInit, gThisPara.xTarget, gThisPara.yTarget,
					gThisPara.zTarget, gThisPara.elasticOrigin, 3.0, 
					SERVO_SAMPLING_RATE);	
				if (pDamper != NULL)
				{
					pDamper -> SetBaseParameters(ZERO_VISCOSITY);
				}

				//pDamper -> SetBaseParameters(ZERO_VISCOSITY);
			}
			else
			{
				gEvent = FLG_EVENT_FREEMOVE;
				FreeMove3D(iCurrentTrial, gThisPara.xInit, gThisPara.yInit, 
					gThisPara.zInit, gThisPara.xTarget, gThisPara.yTarget,
					gThisPara.zTarget, gThisPara.elasticOrigin, 3.0, 
					SERVO_SAMPLING_RATE);		
			}


			//printf("FreeMove3D ended\n");
			printf("\nPkVel = %3.2lf\n", gPkVel);

			gListening = 0;

			// Stop the sampling
			if ( ScopeStop() != SCOPE_NO_ERROR )
				printf("Scope NOT Stopped\n");

			//Check if the user wants a rest
			while(gPause == 1)
			{}
			iCurrentTrial++;
		} 
		while (iCurrentTrial <= gThisPara.trialNum); // End of trials

		while (getchar() != '\n');

		printf ("\nKeep and Name the Data? (Y/N): ");
		cin >> keepData;

		if (toupper(keepData) != 'N')
		{
			// Input Subject Name
			printf ("Enter an Identifier (Initials and Condition Names) :");
			cin >> subjectName;
		}
		else
		{
			cout << "Warning: Data will be Deleted! (Y/N):";
			cin >> keepData;
			if (toupper(keepData) == 'N')
				keepData = 'Y';
			else
				keepData = 'N';
		}

		for (int ii = 0; ii < gThisPara.trialNum; ii++)
		{
			ostringstream oss;

			oss << ii + 1;
			numStr = oss.str();

			dataFile ="C:\\HapticMASTER\\People\\Mark\\Data\\" + 
				subjectName + "_" + numStr + ".dat";	

			emgFile = "C:\\HapticMASTER\\People\\Mark\\Data\\" + 
				subjectName + "_" + numStr + ".emg";

			tempDatFile = "C:\\HapticMASTER\\People\\Mark\\Data\\temp_" + 
				numStr + ".dat";

			tempEmgFile = "C:\\HapticMASTER\\People\\Mark\\Data\\temp_" + 
				numStr + ".emg";

			if (toupper(keepData) != 'N')
			{
				rename(ToChar(tempDatFile), ToChar(dataFile));
				rename(ToChar(tempEmgFile), ToChar(emgFile));
			}
			else
			{
				remove(ToChar(tempDatFile));
				remove(ToChar(tempEmgFile));
			}
		}
		printf ("\nFinish the Experiment(Y/n)? ");
		cin >> finishExp;
	} while (toupper(finishExp) != 'Y');
	printf ("\nSwitch to the 3D Window, Hit ESC to Quit!");
	return 0;	
}

// Main Function
int 
main(int argc, char** argv)
{
	int trajectory;
	double tempSpringPos[3];

	pthread_mutex_init (&mutexPosition, NULL);
	ReadParadigm(gThisPara);


	// Call The Initialize HapticMASTER Function
	InitHapticMaster();

	// Call The Initialize HapticMASTER Function
	InitScope();


	if (pHapticMaster != NULL)
	{
		pHapticMaster -> GetParameter(FCSPRM_POSITION, tempSpringPos);// Reading location information
		pSpring = pHapticMaster -> CreateSpring(tempSpringPos, springStiffness,
			springDamping, springDeadband); // Setting the spring at the current location
		pDamper = pHapticMaster -> CreateDamper(DamperCoeff);
	}

	if (pSpring != NULL)
		pSpring -> Enable();
	if (pDamper != NULL)
		pDamper -> Enable();
	if (pFloor != NULL)
		pFloor -> Enable();
	if (pSlide != NULL)
		pSlide -> Enable();

	// OpenGL Initialization Calls
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600);
	glutCreateWindow ("Fast Reaching Experiment");

	InitOpenGl();

	// More OpenGL Initialization Calls
	glutReshapeFunc (Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc (Keyboard);
	glutTimerFunc (10, TimerCB, 1);

	trajectory = pthread_create(&threads[0], NULL, HapticProtocol, (void *)gCurrentPos);

	glutMainLoop();
	if (pHapticMaster != NULL)
	{
		// When all the trials are finished, close the scope
		ScopeClose();
	}

	if (pSpring != NULL)
	{
		pHapticMaster -> DeleteSpring(pSpring);
		delete pSpring;
	}

	if (pDamper != NULL)
	{
		pHapticMaster -> DeleteDamper(pDamper);
		delete pDamper;
	}

	if (pFloor != NULL)
	{
		pHapticMaster -> DeleteBlock(pFloor);
		delete pFloor;
	}
	if (pSlide != NULL)
	{
		pHapticMaster -> DeleteBlock(pSlide);
		delete pSlide;
	}
	return 0; 
}

