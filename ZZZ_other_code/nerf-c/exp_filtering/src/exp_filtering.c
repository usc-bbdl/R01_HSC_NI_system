/*
 ============================================================================
 Name        : exp_filtering.c
 Author      : Ivan Trujillo-Priego // ivan7256[]hotmail
 Version     :
 Copyright   : Your copyright notice
 Description : Exponential filtering
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(void)
{
  srand48(time(NULL));

  double alfa = 0.4;
  //double T = 0.001; // Sampling interval
  //double eat = exp(-alfa * T);
  double y[1000];
  //double x0;
  double x;
  double yy = 0.0;
  int i;


  for (i = 0; i < 50; i++)
    {
      y[i] = yy;
      if (drand48() > 0.8)
        {
          x = 1;
          printf("%.6f\t", x);
        }
      else
        {
          x = 0;
          printf("%.6f\t", x);
        }
      yy = alfa * y[i] + (1 - alfa) * x;
      printf("%.6f\n\t ", yy);
    }



//
//  for (i = 0; i < 46; i++)
//    {
//      y[i] = drand48();
//      if (y[i] > 0.8)
//        {
//          y[i] = 1;
//          printf("%.6f\t", y[i]);
//        }
//      else
//        {
//          y[i] = 0;
//          printf("%.6f\t", y[i]);
//        }
//      x1 = alfa*y[i] + (1-alfa)*x1;
//      printf("%.6f\n\t ", x1);
//    }

//  for (i = 0; i < 46; i++)
//    {
//      y[i] = yy;
//      if (drand48() > 0.8)
//        {
//          x = 1;
//          printf("%.6f\t", x);
//        }
//      else
//        {
//          x = 0;
//          printf("%.6f\t", x);
//        }
//      yy = eat*y[i] + x;
//      printf("%.6f\n\t ", yy);
//    }
  return 0;

}
