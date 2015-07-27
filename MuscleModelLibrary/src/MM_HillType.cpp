#include "..\include\MM_HillType.h"
#include <math.h>

using namespace std;

namespace MM
{
    MM_HillType::MM_HillType()
    {
        //Inicializing values of time
        gettimeofday(&t_1, NULL);
        gettimeofday(&t_0, NULL);

        //Actual time inicialized
        act_time = 0.0;

        tval_1 = 0.0;
        tval_0 = 0.0;

        //HT_MM_Sim auxiliaries
        T_i = 0.0;
        T_i1 = 0.0;
        spike_i1 = 0.0;
        spike_i2 = 0.0;
        h_i1 = 0.0;
        h_i2 = 0.0;

    }

    MM_HillType::~MM_HillType()
    {
        //dtor
    }
    //Sim function using computer clock for the timing
    double MM_HillType::HT_MM_Sim(double spike,double x, double d_x)
    {
        //Updating time
        t_1 = t_0;
        gettimeofday(&t_0, NULL);
        double delta_t = (t_0.tv_sec - t_1.tv_sec) + 0.000001 * (t_0.tv_usec - t_1.tv_usec);
        act_time += delta_t;

        double h = gen_h_diff_eq(spike);
        //Get A value:
        double A = h * s(x);
        double dT_i = d_force(T_i, x, d_x, 0.0);
        T_i = T_i1 + dT_i * delta_t;
        T_i1 = T_i;
        return T_i;
    }

    //Sim function with time in double
    /*
     * In python code (source of this code, the function is called plotForce)
     */
    double MM_HillType::HT_MM_Sim(double spike,double x, double d_x,double t)
    {
        //Updating time
        tval_1 = tval_0;
        tval_0 = t;
        double delta_t = tval_0 - tval_1;
        act_time = act_time + delta_t;

        double h = gen_h_diff_eq(spike);
        //Get A value:
        double A = h * s(x);
        double dT_i = d_force(T_i, x, d_x, A);
        T_i = T_i1 + dT_i * delta_t;
        T_i1 = T_i;
        return T_i;
    }

    double MM_HillType::s(double x){
        double weighted = 0.0;

        if (x < 0.5){
            weighted = 0.0;
        }
        else if (x < 1.0){
            weighted = -4.0*x*x + 8.0*x - 3.0;
        }
        else if (x < 2.0){
            weighted = -x*x + 2.0*x;
        }
        else{
            weighted = 0.0;
        }
        return weighted;
    }

    double MM_HillType::d_force(double T_0, double lce, double vel, double A){
        const double L0 = 1.0;
        const double Kse = 136.0;
        const double Kpe = 75.0;
        const double b = 50.0;

        double dT_0;

        if (lce > L0){
            dT_0 = Kse / b * (Kpe * (lce - L0) + b * vel - (1 + Kpe/Kse)*T_0 + A);
        }
        else{
            dT_0 = Kse / b * (b * vel - (1 + Kpe/Kse)*T_0 + A);
        }

        //Alternative code:
        /*double x0 = 1.0;
        double x1 = lce;
        double x2 = vel;
        double Kse_x_Kpe_o_b = Kse / b * Kpe;
        double Kse_o_b_m_one_p_Kpe_o_Kse = Kse / b * (1 + Kpe / Kse);
        double Kse_o_b = Kse / b;
        dT_0 = Kse_x_Kpe_o_b * (x1 - x0) + Kse * x2 - Kse_o_b_m_one_p_Kpe_o_Kse * T_0 + Kse_o_b * A;
        */
        return dT_0;
    }

    double MM_HillType::gen_h_diff_eq (double spike){
        //double h = active_state(spike, spike_i1, spike_i2, h_i1, h_i2);
        double h = active_state_fuglevand(spike_i1, spike_i2, h_i1, h_i2);
        spike_i2 = spike_i1;
        spike_i1 = spike;
        h_i2 = h_i1;
        h_i1 = h;

        return h;

    }

    double MM_HillType::active_state(double x_i0, double x_i1, double x_i2, double y_i1, double y_i2){
        double b0 = 2299.0;
        double b1 = -2289.0;
        double b2 = 0.0;
        //double a0 = 1.0;
        double a1 = -1.942;
        double a2 = 0.943;

        double t0 = b0*x_i0;
        double t1 = b1*x_i1;
        double t2 = b2*x_i2;
        double t3 = a1*y_i1;
        double t4 = a2*y_i2;

        double y_i0 = t0 + t1 + t2 - t3 - t4;
        return y_i0;
    }

    double MM_HillType::active_state_fuglevand(double x_i1, double x_i2, double y_i1, double y_i2){
        double T_twitch = 0.001;
        double euler_e = 2.718;
        double P = 1;
        //double neg1 = -1;
        double neg2 = -2;
        double tau = 0.03;
        double A_twitch = exp(-T_twitch / tau);

        //double b0 = 0.0;
        double b1 = P * euler_e * T_twitch * A_twitch;
        double b2 = 0.0;
        //double a0 = 1.0;
        double a1 = neg2 * A_twitch;
        double a2 = A_twitch * A_twitch;

        double t1 = b1 * x_i1;
        double t2 = b2 * x_i2;
        double t3 = a1 * y_i1;
        double t4 = a2 * y_i2;

        double y_i = t1 + t2 - t3 - t4 ;

        return y_i;
    }
}
