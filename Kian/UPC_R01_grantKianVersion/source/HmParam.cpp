#include "HmParam.h"

CHapticMaster::CHapticMaster(char *pName)
{

	pHapticMaster = NULL;
	bConnected = false;
	bInitDone = false;
	strcpy(Name, pName);

	dForce[0] = 0.0;
	dForce[1] = 0.0;
	dForce[2] = 0.0;

	dPos[0] = 0.0;
	dPos[1] = 0.0;
	dPos[2] = 0.0;

	dVel[0] = 0.0;
	dVel[1] = 0.0;
	dVel[2] = 0.0;

	dEeForce[0] = 0.0;
	dEeForce[1] = 0.0;
	dEeForce[2] = 0.0;

}

CHapticMaster::~CHapticMaster()
{
	if(pHapticMaster != NULL)
		pHapticMaster->SetRequestedState ( FCSSTATE_INITIALISED );
}


int CHapticMaster::SetForceGetPV()
{
	return pHapticMaster->SetForceGetPV(dForce, dPos, dVel);
}

int CHapticMaster::GetEeForce()
{
	return pHapticMaster->GetParameter(FCSPRM_FORCE, dEeForce);
}


