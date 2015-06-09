/*
 ============================================================================
 Name        : spindle.c
 Author      : Ivan Trujillo-Priego // ivan7256[]hotmail
 Version     :
 Copyright   : Your copyright notice
 Description : Spindle model
 ============================================================================
 */
//#include <ansi_c.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int
Doer(double *stateMatrix, int bufferInd, int bufferLength, int numDataColumns,
    double samplFreq, double *motorVoltages, double *param, double *auxVar);

void
Spindle(double *neuron_state, double *neuron_input);

int
main(int argc, char *argv[])
{
  double *stateMatrix = (double *) calloc(500 * 67, sizeof(double));
  const int numMotorsTotal = 20;
  int bufferInd = 0;
  int bufferLength = 0;
  int numDataColumns = 67;
  double *motorVoltages = (double *) calloc(20, sizeof(double));
  double *param = calloc(2, sizeof(double));
  double *auxVar = calloc(4, sizeof(double));
  double samplFreq = 1024;

  int i;
  const double KSR = 10.4649;
  const double KPR = 0.1127;
  const double LSR0 = 0.04;
  const double LPR0 = 0.76;
  const double F0ACT = 0.0289;
  const double LCEIN=1.0;
  //double fib;
  //const double SAMPLING_RATE = 1024.0;
  const double F = 4; // in Hz, continuous freq
  const double T = 1.0 / samplFreq; // in seconds
  const double PERIODS = 4;
  const double BIAS = 1.0;
  const double AMP = 0.2;
  double lce[1024];
  int n;
  double max_n;
  double w;



  w = F * 2.0 * M_PI * T;
  max_n = 2.0 * M_PI * PERIODS / w;

  for (n = 0; n < max_n; n++)
    {
      lce[n] = AMP * sin(w * n) + BIAS;
      //printf("%.6f\n", lce);
    }

  auxVar[0] = 0.0; // xo = firing rate
  auxVar[1] = (LPR0*KPR+LCEIN*KSR-LSR0*KSR-F0ACT)/(KSR+KPR); // x1 = Lpr
  auxVar[2] = 0.0; // x2 = Lpr /dt
  auxVar[3] = 0.0;
  param[0] = 80.0;

  for (i = 0; i < max_n; i++)
    {
      param[1] = lce[i];
      stateMatrix[0] = (double) i / samplFreq;
      Doer(stateMatrix, bufferInd, bufferLength, numDataColumns, samplFreq,
          motorVoltages, param, auxVar);
      printf("%.6f\n", auxVar[3]);
     }

  free(stateMatrix);
  free(motorVoltages);
  free(param);
  free(auxVar);
  return 0;
}

int
Doer(double *stateMatrix, int bufferInd, int bufferLength, int numDataColumns,
    double samplFreq, double *motorVoltages, double *param, double *auxVar)
{

  motorVoltages[1] = 0.0;

  //double neuron_state[3], neuron_input[3];
  double *neuron_state = calloc(4, sizeof(double)); // State variables that keep updating by themselves
  double *neuron_input = calloc(3, sizeof(double)); // Sim parameters that should be pre-defined w/o updating

  neuron_input[0] = param[0]; // gamma dynamic for bag1
  neuron_input[1] = param[1]; // Lce the muscle stretch
  neuron_input[2] = 1.0 / samplFreq; // dt


  memcpy(neuron_state, auxVar, 4 * sizeof(double)); // dump the previous states to neuron_state[]

  Spindle(neuron_state, neuron_input); // do one step of the integration
  memcpy(auxVar, neuron_state, 4 * sizeof(double)); // save the current states in auxVar

  motorVoltages[1] = neuron_state[3];

  return 0;

}

void
Spindle(double *neuron_state, double *neuron_input)
{


  const double KSR = 10.4649;
  const double KPR = 0.1127;
  const double BDAMP=0.2356;
  const double GI = 20000;
  const double LSR0 = 0.04;
  const double M = 0.0002;
  const double freq = 60.0;
  double gd = neuron_input[0];
  double lce = neuron_input[1];
  double DT = neuron_input[2];

  double dx0;
  double dx1;
  double dx2;
  //double fib;

  double mingd;
  double CSS;

  double x0 = neuron_state[0];
  double x1 = neuron_state[1];

  double x2 = neuron_state[2];
  double fib = neuron_state[3];

  double xx0, xx1, xx2;



//  for (i = 0; i < 1024 ; i++ )
  //  {
  mingd = pow(gd, 2) / (pow(gd, 2) + pow(freq, 2));
  dx0 = (mingd - x0) / 0.149; // Tao: 0.149
  dx1 = x2;
  if (-1000.0 * x2 > 100.0)
    CSS = -1.0;
  else if (-1000.0 * x2 < -100.0)
    CSS = 1.0;
  else
    {
      CSS = (2.0 / (1.0 + exp(-1000.0 * x2))) - 1.0;
    }
  //printf("%.6f\n", lce);
  dx2 = (1 / M) * (KSR * lce - (KSR + KPR) * x1 - CSS * (BDAMP * x0) * (fabs(x2)) - 0.4);
  xx0 = x0 + dx0 * DT;
  xx1 = x1 + dx1 * DT;
  xx2 = x2 + dx2 * DT;

  fib = fmax(GI * (lce - xx1 - LSR0), 0.0);

  neuron_state[0] = xx0;
  neuron_state[1] = xx1;
  neuron_state[2] = xx2;
  neuron_state[3] = fib;
  //printf("%.6f\n", xx0);
  //printf("%.6f\n", xx2);

}


