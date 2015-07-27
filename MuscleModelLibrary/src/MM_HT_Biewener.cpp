#include "..\include\MM_HT_Biewener.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace MM;
namespace MM {
    MM_HT_Biewener::MM_HT_Biewener()
    {
        //Inicializing values of time
        gettimeofday(&t_1, NULL);
        gettimeofday(&t_0, NULL);

        tval_0 = 0.0;
        tval_1 = 0.0;

        x_0 = 0.0;
        x_1 = 0.0;
        vel = 0.0;

        act_time = 0.0;
        inta1 = 0.0;
        inta2 = 0.0;
        inta3 = 0.0;
        a1_1 = 0.0;
        a1_0 = 0.0;
        a2_1 = 0.0;
        a2_0 = 0.0;
        a3_1 = 0.0;
        a3_0 = 0.0;

        actS = 0.0;
        actF = 0.0;
    }

    MM_HT_Biewener::~MM_HT_Biewener()
    {
        //dtor
    }

    double MM_HT_Biewener::forcelength_active(double length){
        double active;
        double W = length * 1.253;
        active = ((-878.25* W * W) + (2200 * (length * 1.254)) - 1192) / 186.24;
        return active;
    }

    double MM_HT_Biewener::forcelength_pasive(double length){
        double pasive;
        double W = -1.3 + (3.8 * (length * 1.253));
        pasive = exp(W)/186.24;
        return pasive;
    }

    double MM_HT_Biewener::forcevelocity(double vel, double v0, double k){
        double force;

        if (vel <= 0){
            force = (1- (vel/v0))/(1+(vel/(v0*k)));
        }else{
            force = 1.5 - 0.5 * ((1+(vel/v0))/(1-((7.56*vel)/(v0 * k))));
        }
        return force;
    }

    double MM_HT_Biewener::fiber_force(double a, double fa, double fv){
        double F_f;
        F_f = a * fa * fv;
        return F_f;
    }

    double MM_HT_Biewener::total_active_force(double Ff_slow, double Ff_fast){
        double Ff = Ff_slow + Ff_fast;
        return Ff;
    }

    double MM_HT_Biewener::total_muscle_force(double Ff, double Fp, double theta){
        //Calculate c for normalized force!!!
        double c = 30;
        double Fm = (c * (Ff + Fp))* cos(theta);
        return Fm;
    }

    double MM_HT_Biewener::activation(double EMGoff, double tau1, double tau2, double tau3, double beta1, double beta2, double beta3, double delta){
        //Constant to normalize the values
        double c = 1.0;
        //Calculating values
        inta1 = inta1 + (((EMGoff * (1/tau1))-((((EMGoff * (1-beta1)) + beta1)*(1/tau1))*a1_1)) * delta);
        a1_0 = inta1;
        inta2 = inta2 + (((a1_1 * (1/tau2))-((((a1_1 * (1-beta2)) + beta2)*(1/tau2))*a2_1)) * delta);
        a2_0 = inta2;
        inta3 = inta3 + (((a2_1 * (1/tau3)* c)-((((a2_1 * (1-beta3)) + beta3)*(1/tau3))*a3_1)) * delta);
        a3_0 = inta3;

        //Updating values
        a1_1 = a1_0;
        a2_1 = a2_0;
        a3_1 = a3_0;
        //cout << a3_0 <<  endl;
        return a3_0;
    }

    double MM_HT_Biewener::HT_MM_Sim(double EMG, double length){
        //Updating time
        t_1 = t_0;
        gettimeofday(&t_0, NULL);
        double delta_t = (t_0.tv_sec - t_1.tv_sec) + 0.000001 * (t_0.tv_usec - t_1.tv_usec);
        act_time += delta_t;

        pair <double, double> actual(act_time, EMG);

        double EMGoffS = EMG;
        double EMGoffF = EMG;

        timeLineSlow.push(actual);
        timeLineFast.push(actual);

        //For slow
        /*if(act_time - 0.0065 <= 0){
            EMGoffS = 0.0;
        }else{
            ts_0 = timeLineSlow.front();
            EMGoffS = ts_0.second;
            timeLineSlow.pop();
        }*/

        //For fast
        /*if(act_time - 0.0015 <= 0){
            EMGoffF = 0.0;
        }else{
            tf_0 = timeLineFast.front();
            EMGoffF = tf_0.second;
            timeLineFast.pop();
        }*/

        //Calculate velocity
        x_1 = x_0;
        x_0 = length;
        vel = (x_0 - x_1) /delta_t;


        //Compute EMG for slow

        //Eq 4.
        double Fa_l = forcelength_active(x_0);

        //Eq 1. Compute activation
        double tau1s = 0.03406;
        double tau2s = 0.03627;
        double tau3s = 0.03782;
        double beta1s = 0.73;
        double beta2s = 0.74;
        double beta3s = 0.92;
        //Delay slow : 6.5 ms = 0.0065 s
        double act_s = activation(EMGoffS, tau1s, tau2s, tau3s, beta1s, beta2s, beta3s, delta_t);
        // Eq 6 and 7.
        double v_0_s = -3.59;
        double k_s = 0.18;
        double Fv_v_s = forcevelocity(vel, v_0_s, k_s);
        //Eq. 3
        double Ff_s = fiber_force(act_s, Fa_l, Fv_v_s);

        //Compute EMG for fast
        //Eq 1. Compute activation
        double tau1f = 0.01814;
        double tau2f = 0.02091;
        double tau3f = 0.02075;
        double beta1f = 0.9;
        double beta2f = 0.99;
        double beta3f = 0.98;
        //Delay fast: 1.5 ms = 0.0015 s
        double act_f = activation(EMGoffF, tau1f, tau2f, tau3f, beta1f, beta2f, beta3f, delta_t);
        // Eq 6 and 7.
        double v_0_f = -2.74;
        double k_f = 0.29;
        double Fv_v_f = forcevelocity(vel, v_0_f, k_f);
        //Eq. 3
        double Ff_f = fiber_force(act_f, Fa_l, Fv_v_f);

        //Adding forces
        //Eq 8.
        double Total_Fa = total_active_force(Ff_s, Ff_f);
        //Eq 5.
        double Fp_l = forcelength_pasive(x_0);
        //Eq 2.
        double theta = 0.0;
        double Total_muscle_F = total_muscle_force(Total_Fa,Fp_l,theta);
        return Total_muscle_F;
    }

    double MM_HT_Biewener::HT_MM_Sim(double EMG, double length, double t){
        //Updating time
        tval_1 = tval_0;
        tval_0 = t;

        double delta_t = 0.0;
        delta_t = tval_0 - tval_1;
        act_time = act_time + delta_t;

        //Calculate velocity
        x_1 = x_0;
        x_0 = length;
        vel = (x_0 - x_1) /(delta_t + 0.0000001);

        pair <double, double> actual(act_time, EMG);

        double EMGoffS = EMG;
        double EMGoffF = EMG;

        //timeLineSlow.push(actual);
        //timeLineFast.push(actual);

        //For slow
        /*if(act_time - 0.0065 < 0){
            EMGoffS = 0.0;
        }else{
            ts_0 = timeLineSlow.front();
            EMGoffS = ts_0.second;
            timeLineSlow.pop();
        }*/

        //For fast
        /*if(act_time - 0.0015 < 0){
            EMGoffF = 0.0;
        }else{
            tf_0 = timeLineFast.front();
            EMGoffF = tf_0.second;
            timeLineFast.pop();
        }*/
        //cout << EMGoffS << " , " << EMGoffF << " , "<< act_time<<endl;
        //Compute EMG for slow

        //Eq 4.
        double Fa_l = forcelength_active(x_0);

        //Eq 1. Compute activation
        double tau1s = 0.03406;
        double tau2s = 0.03627;
        double tau3s = 0.03782;
        double beta1s = 0.73;
        double beta2s = 0.74;
        double beta3s = 0.92;
        //Delay slow : 6.5 ms = 0.0065 s
        double act_s = activation(EMGoffS, tau1s, tau2s, tau3s, beta1s, beta2s, beta3s, delta_t);
        actS = act_s;
        // Eq 6 and 7.
        double v_0_s = -3.59;
        double k_s = 0.18;
        double Fv_v_s = forcevelocity(vel, v_0_s, k_s);
        //Eq. 3
        double Ff_s = fiber_force(act_s, Fa_l, Fv_v_s);

        //Compute EMG for fast
        //Eq 1. Compute activation
        double tau1f = 0.01814;
        double tau2f = 0.02091;
        double tau3f = 0.02075;
        double beta1f = 0.9;
        double beta2f = 0.99;
        double beta3f = 0.98;
        //Delay fast: 1.5 ms = 0.0015 s
        double act_f = activation(EMGoffF, tau1f, tau2f, tau3f, beta1f, beta2f, beta3f, delta_t);
        actF = act_f;
        // Eq 6 and 7.
        double v_0_f = -2.74;
        double k_f = 0.29;
        double Fv_v_f = forcevelocity(vel, v_0_f, k_f);
        //Eq. 3
        double Ff_f = fiber_force(act_f, Fa_l, Fv_v_f);

        //Adding forces
        //Eq 8.
        double Total_Fa = total_active_force(Ff_s, Ff_f);
        eq8 = Total_Fa;
        //Eq 5.
        double Fp_l = forcelength_pasive(x_0);
        eq5 = Fp_l;
        //Eq 2.
        double theta = 7.0 * 3.1416 / 180;
        double Total_muscle_F = total_muscle_force(Total_Fa,Fp_l,theta);
        return Total_muscle_F;
    }

}
