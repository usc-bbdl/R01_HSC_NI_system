/*

 *
 * Author: C. Minos Niu
 *
 *           
 *
 */

/*  File    : stspace.c
 *  Abstract:
 *
 *      Example mex file S-function for state-space system.
 *
 *      Implements a set of state-space equations.
 *      You can turn this into a new block by using the
 *      S-function block and Mask facility.
 *
 *      This example MEX-file performs the same function
 *      as the built-in State-space block. This is an
 *      example of a MEX-file where the number of inputs,
 *      outputs, and states is dependent on the parameters
 *      passed in from the workspace.
 *
 *      Syntax  [sys, x0] = stspace(t,x,u,flag,A,B,C,D,X0)
 *
 *      For more details about S-functions, see simulink/src/sfuntmpl_doc.c
 *
 *  Copyright 1990-2009 The MathWorks, Inc.
 *  $Revision: 1.1.6.1 $
 */

#define S_FUNCTION_NAME izh
#define S_FUNCTION_LEVEL 2

#define SAMPLING_RATE 1024.0

#include "simstruc.h"
#include "math.h"

#define U(element) (*uPtrs[element])  /* Pointer to Input Port0 */

#define MAGICNUM_IDX 0
#define MAGICNUM_PARAM(S) ssGetSFcnParam(S,MAGICNUM_IDX)
 
#define X0_IDX 1
#define X0_PARAM(S) ssGetSFcnParam(S,X0_IDX)
 
#define NPARAMS 2 // {MAGICNUM_PARAM, X0_PARAM}

#define NSTATES   2 // {u, v} in Izhi model. mxGetN(X0_PARAM(S))
#define NMAGICNUM 6 // {a, b, c, d} in Izh model + {threthold control}
#define NINPUTS   1 // {I} in Izhi model. mxGetN(B_PARAM(S))
#define NOUTPUTS  2 // {u, spike} in Izhi model. mxGetM(C_PARAM(S))

#define IS_PARAM_DOUBLE(pVal) (mxIsNumeric(pVal) && !mxIsLogical(pVal) &&\
!mxIsEmpty(pVal) && !mxIsSparse(pVal) && !mxIsComplex(pVal) && mxIsDouble(pVal))

#define OK_EMPTY_DOUBLE_PARAM(pVal) (mxIsNumeric(pVal) && !mxIsLogical(pVal) &&\
!mxIsSparse(pVal) && !mxIsComplex(pVal) && mxIsDouble(pVal)) 

/*====================*
 * S-function methods *
 *====================*/
 
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
  /* Function: mdlCheckParameters =============================================
   * Abstract:
   *    Validate our parameters to verify they are okay.
   */
  static void mdlCheckParameters(SimStruct *S)
  {
      /* Check 1st parameter: MAGICNUM-Matrix */
      {
          if ( mxGetNumberOfElements(MAGICNUM_PARAM(S)) != NMAGICNUM || 
                  !IS_PARAM_DOUBLE(MAGICNUM_PARAM(S)) )
          {
              ssSetErrorStatus(S,"1st parameter to S-function "
                               "\"A-Matrix\" must be square and double");
              return;
          }
      }

      /* Check 2nd parameter: X0 */
      {
          if ( ((mxGetM(X0_PARAM(S)) != 0) && 
               (mxGetNumberOfElements(X0_PARAM(S)) != NSTATES)) || !OK_EMPTY_DOUBLE_PARAM(X0_PARAM(S)) ) {
              ssSetErrorStatus(S,"2nd parameter to S-function "
                               "\"X0-Matrix\" is not dimensioned "
                               "correctly");
              return;
          }
      }
  }
#endif /* MDL_CHECK_PARAMETERS */
 


/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, NPARAMS);  /* Number of expected parameters */
#if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) {
            return;
        }
    } else {
        return; /* Parameter mismatch will be reported by Simulink */
    }
#endif

    {
        int iParam = 0;
        int nParam = ssGetNumSFcnParams(S);

        for ( iParam = 0; iParam < nParam; iParam++ )
        {
            ssSetSFcnParamTunable( S, iParam, SS_PRM_SIM_ONLY_TUNABLE );
        }
    }

    ssSetNumContStates(S, NSTATES);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    ssSetInputPortWidth(S, 0, NINPUTS);
    ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, NOUTPUTS)) return;
    //ssSetOutputPortWidth(S, 0, NOUTPUTS);
    {
        ssSetOutputPortWidth(S, 0, 1);
        ssSetOutputPortWidth(S, 1, 1);
    }
    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    /* Take care when specifying exception free code - see sfuntmpl_doc.c */
    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    S-function is comprised of only continuous sample time elements
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
    
}



#define MDL_INITIALIZE_CONDITIONS
/* Function: mdlInitializeConditions ========================================
 * Abstract:
 *    If the initial condition parameter (X0) is not an empty matrix,
 *    then use it to set up the initial conditions, otherwise,
 *    set the initial conditions to all 0.0
 */
static void mdlInitializeConditions(SimStruct *S)
{
    real_T *x0 = ssGetContStates(S);
    int_T  i, nStates;
 
    nStates = ssGetNumContStates(S);
    if (mxGetM(X0_PARAM(S)) != 0) {
        const real_T *pr = mxGetPr(X0_PARAM(S));

        for (i = 0; i < nStates; i++) {
            *x0++ = *pr++;
        }
    } else {
        for (i = 0; i < nStates; i++) {
            *x0++ = 0.0;
        }
    }
}



/* Function: mdlOutputs =======================================================
 * Abstract:
 *      y = Cx + Du
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    real_T            *y       = ssGetOutputPortRealSignal(S,0);
    real_T            *x       = ssGetContStates(S);
    InputRealPtrsType uPtrs    = ssGetInputPortRealSignalPtrs(S,0);
    const real_T      *apr     = mxGetPr(MAGICNUM_PARAM(S));
    int_T             nStates  = ssGetNumContStates(S);
    int_T             nInputs  = ssGetInputPortWidth(S,0);
    int_T             nOutputs = ssGetOutputPortWidth(S,0);
    int_T             i, j;
    real_T            accum;
 
    UNUSED_ARG(tid); /* not used in single tasking mode */
  // SetRandomSeed(0);  
  // Firing threshold randomly distributed 25.0 ~ 35.0
    double TH_RANGE = 10.0;
    //double TH = 30.0 - TH_RANGE + ( 2.0 * TH_RANGE * rand() / ( RAND_MAX + 1.0 ) );
    //double TH = 10.0;
    real_T TH = apr[4];
    real_T EPSP_WEIGHT = apr[5];
    //*exportTH = TH;

    real_T  v = x[0];
    real_T  u = x[1];
    
    real_T  C = apr[2];
    real_T  D = apr[3];
    
    if (v >= TH) // if spikes    
    {
      x[0] = C;
      x[1] = u + D;
      y[0] = C;             
      y[1] = 10.0 / (pow(EPSP_WEIGHT, 1.6)) * SAMPLING_RATE; // 1 = spiking
      //y[1] = 1.0 * SAMPLING_RATE; // 1 = spiking

    }
    else
    {
        y[0] = x[0];
        y[1] = 0.0;
    };

// 
//     /* Matrix Multiply: y = Cx + Du */
//     for (i = 0; i < nOutputs; i++) {
//         accum = 0.0;
// 
//         /* Cx */
//         for (j = 0; j < nStates; j++) {
//             accum += cpr[i + nOutputs*j] * x[j];
//         }
// 
//         /* Du */
//         for (j = 0; j < nInputs; j++) {
//             accum += dpr[i + nOutputs*j] * U(j);
//         }
// 
//         y[i] = accum;
//     }
}



#define MDL_DERIVATIVES
/* Function: mdlDerivatives =================================================
 * Abstract:
 *      xdot = Ax + Bu
 */
static void mdlDerivatives(SimStruct *S)
{
    real_T            *dx     = ssGetdX(S);
    real_T            *x      = ssGetContStates(S);
    InputRealPtrsType uPtrs   = ssGetInputPortRealSignalPtrs(S,0);
    const real_T      *apr    = mxGetPr(MAGICNUM_PARAM(S));
    int_T             nStates = ssGetNumContStates(S);
    int_T             nInputs  = ssGetInputPortWidth(S,0);
    int_T i, j;
    real_T accum;
 
    /* Matrix Multiply: dx = Ax + Bu */
    real_T EPSP_WEIGHT = apr[5];
    
    real_T  v = x[0];
    real_T  u = x[1];
    
    real_T  A = apr[0];
    real_T  B = apr[1];

    dx[0] = 1000 * (0.04 * v*v + 5.0 * v + 140.0 - u + U(0) * EPSP_WEIGHT); // U(0) = 1st input, i.e. I;
    dx[1] = 1000 * A * (B * v - u) ; // neuron[1] = u; See iZhikevich model

//     for (i = 0; i < nStates; i++) {
//         accum = 0.0;
//  
//         /* Ax */
//         for (j = 0; j < nStates; j++) {
//             accum += apr[i + nStates*j] * x[j];
//         }
//  
//         /* Bu */
//         for (j = 0; j < nInputs; j++) {
//             accum += bpr[i + nStates*j] * U(j);
//         }
//  
//         dx[i] = accum;
//     }
}
 
 
 
/* Function: mdlTerminate =====================================================
 * Abstract:
 *    No termination needed, but we are required to have this routine.
 */
static void mdlTerminate(SimStruct *S)
{
    UNUSED_ARG(S); /* unused input argument */
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif

