`timescale 1ns / 1ps
`default_nettype none
//////////////////////////////////////////////////////////////////////////////////
// Creator: C. Minos Niu
// 
// Module Name:    
// Project Name: 
// Target Devices: XEM6010 - OpalKelly
// Design properties: xc6slx150-2fgg484
// Description: 
//
// Dependencies: 
//
// Revision: 
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module one_joint_robot_all_in1_xem6010(
	input  wire [7:0]  hi_in,
	output wire [1:0]  hi_out,
	inout  wire [15:0] hi_inout,
	inout  wire        hi_aa,

	output wire        i2c_sda,
	output wire        i2c_scl,
	output wire        hi_muxsel,
	input  wire        clk1,
	input  wire        clk2,
	
	output wire [7:0]  led,
    output wire pin0,   
    output wire pin1,
    output wire pin2,
    output wire spike_out1,  // corss-board spike output
    input wire  spike_in1  // cross-board spike input
	 );
//	input wire SCK_r,	    //pin_jp1_41    SCK_r
//   input wire SSEL_r,	    //pin_jp1_42    SSEL_r
//   input wire DATA_0_r,  //pin_jp1_43    Data_bic_r
//	//input wire DATA_1_r,  //pin_jp1_44    Data_tri_r
//   output wire  SCK_s,	    //pin_jp2_41    SCK_s
//   output wire  SSEL_s, 	     //pin_jp2_42	  SSEL_s
//   output wire  DATA_0_s 	//pin_jp2_43   DATA_bic_s
//   //output wire  DATA_1_s	   //pin_jp2_43 DATA_tri_s
//   );
//   
//    
//	 // Mapping to UCF files
//	 wire DATA_tricepsfr_Ia_r,DATA_triforce_r, DATA_trilen_s, DATA_biclen_s;
//	 assign DATA_tricepsfr_Ia_r = DATA_0_r;
//	 //assign DATA_triforce_r = DATA_1_r;
//	 assign DATA_0_s = DATA_trilen_s;
//	 //assign DATA_1_s = DATA_biclen_s;
//		
    parameter NN = 6;
    // *** Dump all the declarations here:
    wire         ti_clk;
    wire [30:0]  ok1;
    wire [16:0]  ok2;   
    wire [15:0]  ep00wire, ep01wire, ep02wire, ep03wire, ep04wire, ep05wire, ep06wire, ep07wire, ep08wire, ep50trig, ep20wire, ep21wire, ep22wire, ep23wire;
    wire [15:0]  ep24wire, ep25wire, ep26wire, ep27wire, ep28wire, ep29wire, ep30wire, ep31wire;
    wire reset_global, reset_sim;
    wire        is_pipe_being_written, is_lce_valid;
    
    wire [15:0] hex_from_py;
    
    reg [31:0] delay_cnt_max;
    
    reg [15:0] rawspikes;
    wire pipe_out_read;
 
    // *** Target interface bus:
    assign i2c_sda = 1'bz;
    assign i2c_scl = 1'bz;
    assign hi_muxsel = 1'b0;

  // *** Buttons, physical on XEM3010, software on XEM3050 & XEM6010
    // *** Reset & Enable signals
    assign reset_global = ep00wire[0];
    assign reset_sim = ep00wire[1];

    //assign enable_sim = is_waveform_valid;
    wire    [31:0]  IEEE_1, IEEE_0;
	assign IEEE_1 = 32'h3F800000;
	assign IEEE_0 = 32'd0;


    // *** Triggered input from Python
    always @(posedge ep50trig[0] or posedge reset_global)
    begin
        if (reset_global)
            delay_cnt_max <= 18'd9;
        else
            delay_cnt_max <= {ep02wire, ep01wire};  //firing rate
    end
    
    
    reg [31:0] f_pps_coef_Ia;
    always @(posedge ep50trig[1] or posedge reset_global)
    begin
        if (reset_global)
            f_pps_coef_Ia <= 32'h3F66_6666;
        else
            f_pps_coef_Ia <= {ep02wire, ep01wire};  //firing rate
    end       
//    
//    reg [31:0] f_pps_coef_II;
//    always @(posedge ep50trig[2] or posedge reset_global)
//    begin
//        if (reset_global)
//            f_pps_coef_II <= 32'h3F66_6666;
//        else
//            f_pps_coef_II <= {ep02wire, ep01wire};  //firing rate
//    end           
//    



    reg [31:0] tau;
    always @(posedge ep50trig[2] or posedge reset_global)
    begin
        if (reset_global)
            tau <= 32'h3CF5_C28F; // 0.03 second
        else
            tau <= {ep02wire, ep01wire};  
    end      
    
//
//    reg [31:0] gain;
//    always @(posedge ep50trig[3] or posedge reset_global)
//    begin
//        if (reset_global)
//            gain <= 32'd0;
//        else
//            gain <= {ep02wire, ep01wire};  //firing rate
//    end        
    
             
    /***  Cortical synapse gain  ***/
    reg signed [31:0] i_gain_syn_CN_to_MN; 
    always @(posedge ep50trig[3] or posedge reset_global)
    begin
        if (reset_global)
            i_gain_syn_CN_to_MN <= 32'd1; // gamma_sta reset to 80
        else
            i_gain_syn_CN_to_MN <= {ep02wire, ep01wire};  
    end 
    
    reg [31:0] f_gamma_dyn;
    always @(posedge ep50trig[4] or posedge reset_global)
    begin
        if (reset_global)
            f_gamma_dyn <= 32'h42A0_0000; // gamma_dyn reset to 80
        else
            f_gamma_dyn <= {ep02wire, ep01wire};  
    end  
    
    reg [31:0] f_gamma_sta;
    always @(posedge ep50trig[5] or posedge reset_global)
    begin
        if (reset_global)
            f_gamma_sta <= 32'h42A0_0000; // gamma_sta reset to 80
        else
            f_gamma_sta <= {ep02wire, ep01wire};  
    end  
    
    reg [31:0] i_gain_syn_SN_to_MN;
    always @(posedge ep50trig[6] or posedge reset_global)
    begin
        if (reset_global)
            i_gain_syn_SN_to_MN <= 32'd1; // gamma_sta reset to 80
        else
            i_gain_syn_SN_to_MN <= {ep02wire, ep01wire};  
    end
	 
//    reg signed [31:0] i_gain_mu2_MN;
//    always @(posedge ep50trig[7] or posedge reset_global)
//    begin
//        if (reset_global)
//            i_gain_mu2_MN <= 32'd1; // gamma_sta reset to 80
//        else
//            i_gain_mu2_MN <= {ep02wire, ep01wire};  
//    end  
//	 
	 reg signed [31:0] trigger_input;
    always @(posedge ep50trig[7] or posedge reset_global)
    begin
        if (reset_global)
            trigger_input <= 32'd1; // gamma_sta reset to 80
        else
            trigger_input <= {ep02wire, ep01wire};  
    end  
    
//    /***  MotorCortex direct drive ***/
//    reg [31:0] i_M1_CN2_drive; 
//    always @(posedge ep50trig[8] or posedge reset_global)
//    begin
//        if (reset_global)
//            i_M1_CN2_drive <= 32'h0000_0000; //0.0
//        else
//            i_M1_CN2_drive <= {ep02wire, ep01wire}; 
//    end   
    
    reg [31:0] i_gain_syn_CN2_to_MN;
    always @(posedge ep50trig[9] or posedge reset_global)
    begin
        if (reset_global)
            i_gain_syn_CN2_to_MN <= 32'd1; 
        else
            i_gain_syn_CN2_to_MN <= {ep02wire, ep01wire};  
    end
    
//    reg signed [31:0] i_gain_mu3_MN;
//    always @(posedge ep50trig[8] or posedge reset_global)
//    begin
//        if (reset_global)
//            i_gain_mu3_MN <= 32'd1; // gamma_sta reset to 80
//        else
//            i_gain_mu3_MN <= {ep02wire, ep01wire};  
//    end  
//
//    reg [31:0] f_len_bic_pxi_F0; // _pxi = from PXI system in BBDL
//    always @(posedge ep50trig[8] or posedge reset_global)
//    begin
//        if (reset_global)
//            f_len_bic_pxi_F0 <= 32'h3F66_6666; //0.9
//        else
//            f_len_bic_pxi_F0 <= {ep02wire, ep01wire}; 
//    end   
//
//    reg [31:0] f_velocity; // _pxi = from PXI system in BBDL
//    always @(posedge ep50trig[9] or posedge reset_global)
//    begin
//        if (reset_global)
//            f_velocity <= 32'h0000_0000; //0.0
//        else
//            f_velocity <= {ep02wire, ep01wire}; 
//    end   

    /**** Update  pos and vel at the same time ****/
    reg [31:0] f_len_pxi_F0; // _pxi = from PXI system in BBDL
    reg [31:0] f_velocity_F0; // _pxi = from PXI system in BBDL
    reg [31:0] i_M1_CN_drive;
    reg [31:0] i_M1_CN2_drive;     
    
    always @(posedge ep50trig[10] or posedge reset_global)
    begin
        if (reset_global) begin
            f_len_pxi_F0 <= 32'h3F66_6666; //0.9
            f_velocity_F0 <= 32'h0000_0000; //0.0       
            i_M1_CN_drive <= 32'd0;             
            i_M1_CN2_drive <= 32'd0;
        end
        else begin
            f_len_pxi_F0 <= {ep02wire, ep01wire}; 
            f_velocity_F0 <= {ep04wire, ep03wire};             
            i_M1_CN_drive <= {ep06wire, ep05wire};             
            i_M1_CN2_drive <= {ep08wire, ep07wire}; 
        end
    end   

    /***  II sensory spindle  ***/
    reg [31:0] f_pps_coef_II; 
    always @(posedge ep50trig[11] or posedge reset_global)
    begin
        if (reset_global)
            f_pps_coef_II <= 32'h3F66_6666;
        else
            f_pps_coef_II <= {ep02wire, ep01wire}; 
    end   
     
    /***  MotorCortex synapse gain  ***/
    reg signed [31:0] i_gain_syn_SN_to_CN; 
    always @(posedge ep50trig[12] or posedge reset_global)
    begin
        if (reset_global)
            i_gain_syn_SN_to_CN <= 32'd1; // gamma_sta reset to 80
        else
            i_gain_syn_SN_to_CN <= {ep02wire, ep01wire};  
    end
    

//    
//    reg [31:0] delay_cnt_max;
//    always @(posedge ep50trig[7] or posedge reset_global)
//    begin
//        if (reset_global)
//            delay_cnt_max <= delay_cnt_max;
//        else
//            delay_cnt_max <= {ep02wire, ep01wire};  //firing rate
//    end        
//    

    reg [31:0] BDAMP_1, BDAMP_2, BDAMP_chain, GI, GII;
    always @(posedge ep50trig[15] or posedge reset_global)
    begin
        if (reset_global)
            BDAMP_1 <= 32'h3E71_4120; // bag 1 BDAMP reset to 0.2356
        else
            BDAMP_1 <= {ep02wire, ep01wire};  //firing rate
    end
    always @(posedge ep50trig[14] or posedge reset_global)
    begin
        if (reset_global)
            BDAMP_2 <= 32'h3D14_4674; // bag 2 BDAMP reset to 0.0362
        else
            BDAMP_2 <= {ep02wire, ep01wire};  //firing rate
    end    
    always @(posedge ep50trig[13] or posedge reset_global)
    begin
        if (reset_global)
            BDAMP_chain <= 32'h3C58_44D0; // chain BDAMP reset to 0.0132 
        else
            BDAMP_chain <= {ep02wire, ep01wire};  //firing rate
    end
    

    // *** Deriving clocks from on-board clk1:
    wire neuron_clk, sim_clk, spindle_clk;
//    wire [NN+2:0] neuronCounter;
    wire [31:0] i_neuronCounter; 

    gen_clk useful_clocks
    (   .rawclk(clk1), 
        .half_cnt(delay_cnt_max), 
        .clk_out1(neuron_clk), 
        .clk_out2(sim_clk), 
        .clk_out3(spindle_clk),
        .int_neuron_cnt_out(i_neuronCounter) );
                
    
    // *** Generating waveform to stimulate the spindle
    wire    [31:0] f_pos_elbow, f_rawfr_Ia;
    wire    [31:0] f_len_bic, f_len_tri;
    waveform_from_pipe_bram_2s    generator(
                                .reset(reset_sim),
                                .pipe_clk(ti_clk),
                                .pipe_in_write(is_pipe_being_written),
                                .pipe_in_data(hex_from_py),
                                .pop_clk(sim_clk),
                                .wave(f_len_bic)
    );  
    
// // Get biceps muscle length from joint angle
    reg [31:0] f_len_pxi; // _pxi = from PXI system in BBDL
    reg [31:0] f_velocity;
    always @(posedge sim_clk or posedge reset_sim)
	 begin
	   if (reset_sim)
		begin
		  f_len_pxi <= 32'h3f80_0000;
          f_velocity <= 32'h0;
		end else begin
		  f_len_pxi <= f_len_pxi_F0;
          f_velocity <= f_velocity_F0;
		end
	 end

    // *** Spindle: f_muscle_len => f_rawfr_Ia
    wire [31:0] f_fr_Ia, x_0, x_1, f_fr_II;
    spindle bic_bag1_bag2_chain
    (	.gamma_dyn(f_gamma_dyn), // 32'h42A0_0000
        .gamma_sta(f_gamma_sta),
        .lce(f_len_pxi),
        .clk(spindle_clk),
        .reset(reset_sim),
        .out0(x_0),
        .out1(x_1),
        .out2(f_fr_II),
        .out3(f_fr_Ia),
        .BDAMP_1(BDAMP_1),
        .BDAMP_2(BDAMP_2),
        .BDAMP_chain(BDAMP_chain)
		);

//    wire [31:0] f_tricepsfr_Ia, x_0_tri, x_1_tri, f_tricepsfr_II;
//    spindle tri_bag1_bag2_chain
//    (	.gamma_dyn(f_gamma_dyn), // 32'h42A0_0000
//        .gamma_sta(f_gamma_sta),
//        .lce(f_len_tri_pxi),
//        .clk(spindle_clk),
//        .reset(reset_sim),
//        .out0(x_0_tri),
//        .out1(x_1_tri),
//        .out2(f_tricepsfr_II),
//        .out3(f_tricepsfr_Ia),
//        .BDAMP_1(BDAMP_1),
//        .BDAMP_2(BDAMP_2),
//        .BDAMP_chain(BDAMP_chain)
//		);

     ////********************Ia sensory ***************************************///
	wire signed [31:0] i_current_out;
	wire SN_spk;
    wire [15:0] spkid_SN;
	 wire [31:0] neuron_clk_temp;
    
    neuron_pool #(.NN(NN)) SN_pool_Ia
    (   .f_rawfr_Ia(f_fr_Ia),     //
        .f_pps_coef_Ia(f_pps_coef_Ia), //
        .half_cnt(delay_cnt_max),
        .rawclk(clk1),
        .ti_clk(ti_clk),
        .reset_global(reset_sim),
        //.i_gain(i_gain_mu1_SN),
//        .neuronCounter(neuronCounter),
        .spike(),
        .each_spike(SN_spk),
        .spkid(spkid_SN),
		//.out3(neuron_clk_temp),
		.i_current_out(i_current_out),
        .population()
        );
          
    wire [31:0]  i_SN_spkcnt;
    spike_counter  sync_counter
     (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(SN_spk),
                        .spike_count(i_SN_spkcnt) );
          
    ////********************II sensory ***************************************///
    wire signed [31:0] i_current_out_II;
	wire SN_spk_II;
    wire [15:0] spkid_SN_II;
	 wire [31:0] neuron_clk_temp_II;
    
    neuron_pool #(.NN(NN)) SN_pool_II
    (   .f_rawfr_Ia(f_fr_II),     //
        .f_pps_coef_Ia(f_pps_coef_II), //
        .half_cnt(delay_cnt_max),
        .rawclk(clk1),
        .ti_clk(ti_clk),
        .reset_global(reset_sim),
        //.i_gain(i_gain_mu1_SN),
//        .neuronCounter(neuronCounter),
        .spike(),
        .each_spike(SN_spk_II),
        .spkid(spkid_SN_II),
		  .out3(neuron_clk_temp_II),
		  .i_current_out(i_current_out_II),
          .population()
          );

    wire [31:0]  i_SN_spkcnt_II;
    spike_counter  sync_counter_II
     (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(SN_spk_II),
                        .spike_count(i_SN_spkcnt_II) );

//
//	wire    [31:0] i_SN_spkcnt;
//	wire    dummy_slow;  
//	spike_counter	spike_cnt_SN 
//	(		.spike(SN_spk),
//			.int_cnt_out(i_SN_spkcnt),
//			.slow_clk(sim_clk),
//			.fast_clk(clk1),
//			.reset(reset_sim),
//			.clear_out(dummy_slow));
//			

    
    
    	
//    //******** combining transcortical spikes & short latency spikes ***********// DO NOT 'OR' it, Add currents together with two synapses. 
//	wire spike_SL_LL_combined;
//    assign spike_SL_LL_combined = SN_bic_spk | spike_in1;  



	//******** Synapse 1**********/
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
    wire [31:0] I_synapse_SN_to_MN;
    wire [31:0] i_EPSC_SN_to_MN_preamp;
    wire each_spike_SN_to_MN;
    
    synapse synapse_SN_to_MN(
                .clk(neuron_clk),
                .reset(reset_sim),
                .spike_in(SN_spk),
                .postsynaptic_spike_in(each_spike_SN_to_MN),
                .I_out(I_synapse_SN_to_MN),  // updates once per population (scaling factor 1024) 
                .each_I(i_EPSC_SN_to_MN_preamp) // updates on each synapse
    );
    
    
   wire [31:0] i_EPSC_SN_to_MN;
   unsigned_mult32 syn1_gain(.out(i_EPSC_SN_to_MN), .a(i_EPSC_SN_to_MN_preamp), .b(i_gain_syn_SN_to_MN));
   
   

//******** Synapse II to MN **********/
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
    wire [31:0] I_synapse_SN_to_MN_II;
    wire [31:0] i_EPSC_SN_to_MN_preamp_II;
    wire each_spike_SN_to_MN_II;
    
    synapse synapse_SN_to_MN_II(
                .clk(neuron_clk),
                .reset(reset_sim),
                .spike_in(SN_spk_II),
                .postsynaptic_spike_in(each_spike_SN_to_MN_II),
                .I_out(I_synapse_SN_to_MN_II),  // updates once per population (scaling factor 1024) 
                .each_I(i_EPSC_SN_to_MN_preamp_II) // updates on each synapse
    );
    
    
   wire [31:0] i_EPSC_SN_to_MN_II;
   unsigned_mult32 syn1_gain_II(.out(i_EPSC_SN_to_MN_II), .a(i_EPSC_SN_to_MN_preamp_II), .b(i_gain_syn_SN_to_MN));
   
   

    
    /*************************************************************************************************************************/    
    /**************************************** M1 direct drive (voluntary command) ***************************/
    /*************************************************************************************************************************/
 
    //****** SN to CN Synapse gain ********/
    wire [31:0] i_EPSC_SN_to_CN_preamp;
    wire [31:0] i_EPSC_SN_to_CN;
   unsigned_mult32 syn2_gain(.out(i_EPSC_SN_to_CN), .a(i_EPSC_SN_to_MN_preamp), .b(i_gain_syn_SN_to_CN));
      
   
   //***   Addition of two currents ******//
   wire [31:0] i_drive_to_CN_F0;
   assign i_drive_to_CN_F0 = i_EPSC_SN_to_CN + i_M1_CN_drive;   
   
   
    
     //********* izneuron *************//
	wire CN_spk;
	
//    izneuron neuron_pool_CN(
//                .clk(neuron_clk),
//                .reset(reset_sim),
//                .I_in(i_drive_to_CN),
//                .spike(),
//                .each_spike(CN_spk)
//    );
    
     wire [31:0] v_neuron0_CN;
    wire each_spike_neuron0_CN, spike_neuron0_CN;
    wire [127:0] population_neuron0_CN;
    izneuron_th_control neuron_pool_CN(
        .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
        .reset(reset_global),           // reset to initial conditions
        .I_in(  i_drive_to_CN ),          // input current from synapse
        .th_scaled(32'd30720),            // default 30mv threshold scaled x1024
        .v_out(v_neuron0_CN),               // membrane potential
        .spike(spike_neuron0_CN),           // spike sample
        .each_spike(CN_spk), // raw spikes
        .population(population_neuron0_CN)  // spikes of population per 1ms simulation time
    );   


    
    //***************************************** Synapse 2***************************************/
    // Since this synapse is exactly identical to SN_to_MN synapse, we are going to share the output of the synapse. -Eric 20130620
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
//    wire [31:0] I_synapse_CN1;
//    wire each_spike;
//    
//    synapse SN_to_CN(
//                .clk(neuron_clk),
//                .reset(reset_sim),
//                .spike_in(SN_spk),
//                .postsynaptic_spike_in(each_spike),
//                .I_out(I_synapse_CN1),  // updates once per population (scaling factor 1024) 
//                .each_I(i_EPSC_SN_to_CN_preamp) // updates on each synapse
//    );
    

    
    //***************************************** Synapse 5***************************************/
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
    wire [31:0] I_synapse_CN2;
    wire each_spike_SN_to_CN2;
    
    synapse SN_to_CN2(
                .clk(neuron_clk),
                .reset(reset_sim),
                .spike_in(SN_spk),
                .postsynaptic_spike_in(each_spike_SN_to_CN2),
                .I_out(I_synapse_CN2),  // updates once per population (scaling factor 1024) 
                .each_I(i_EPSC_SN_to_CN2_preamp) // updates on each synapse
    );
    
    /****** SN to CN2 Synapse gain ********/
    wire [31:0] i_EPSC_SN_to_CN2_preamp;
    wire [31:0] i_EPSC_SN_to_CN2;
    unsigned_mult32 syn520_gain(.out(i_EPSC_SN_to_CN2), .a(i_EPSC_SN_to_CN2_preamp), .b(i_gain_syn_SN_to_CN)); // sharing same gain in CN's

   //***   Addition of two currents ******//
   wire [31:0] i_drive_to_CN2_F0;
   assign i_drive_to_CN2_F0 = i_EPSC_SN_to_CN2 + i_M1_CN2_drive;   

        
   //********* izneuron *************/
	wire CN2_spk;
//    izneuron neuron_pool_CN2(
//                .clk(neuron_clk),
//                .reset(reset_sim),
//                .I_in(i_drive_to_CN2),
//                .spike(),
//                .each_spike(CN2_spk)
//    );
//	
    wire [31:0] v_neuron0_CN2;
    wire each_spike_neuron0_CN2, spike_neuron0_CN2;
    wire [127:0] population_neuron0_CN2;
    izneuron_th_control neuron_pool_CN2(
        .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
        .reset(reset_global),           // reset to initial conditions
        .I_in(  i_drive_to_CN2 ),          // input current from synapse
        .th_scaled(32'd30720),            // default 30mv threshold scaled x1024
        .v_out(v_neuron0_CN2),               // membrane potential
        .spike(spike_neuron0_CN2),           // spike sample
        .each_spike(CN2_spk), // raw spikes
        .population(population_neuron0_CN2)  // spikes of population per 1ms simulation time
    );   





//    
//    /********* CN1 izneuron *************/
//	wire CN1_bic_spk;
//	
//    izneuron neuron_pool_CN1_bic(
//                .clk(neuron_clk),
//                .reset(reset_sim),
//                .I_in(I_synapse_CN1_gainAdjusted32),
//                .spike(),
//                .each_spike(CN1_bic_spk)
//    );
    
	//******** Synapse 3**********/
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
    wire [31:0] I_synapse_CN_to_MN;
    wire [31:0] i_EPSC_CN_to_MN_preamp;
    wire each_spike_CN_to_MN;
    
    synapse CN_to_MN(
                .clk(neuron_clk),
                .reset(reset_sim),
                .spike_in(CN_spk),
                .postsynaptic_spike_in(each_spike_CN_to_MN),
                .I_out(I_synapse_CN_to_MN),  // updates once per population (scaling factor 1024) 
                .each_I(i_EPSC_CN_to_MN_preamp) // updates on each synapse
    );

   
   wire [31:0] i_drive_to_MN_F0;   
    //****** Syanpse gain ********//
    wire [31:0] i_EPSC_CN_to_MN;
   unsigned_mult32 syn524_gain(.out(i_EPSC_CN_to_MN), .a(i_EPSC_CN_to_MN_preamp), .b(i_gain_syn_CN_to_MN));
   
   
       
    //******** Synapse 4**********/
	//** input: spikes
    //** output: current (each_I_synapse: updates at neuron_clk)
    
    wire [31:0] I_synapse_CN2_to_MN;
    wire [31:0] i_EPSC_CN2_to_MN_preamp;
    wire each_spike_CN2_to_MN;
    
    synapse CN2_to_MN(
                .clk(neuron_clk),
                .reset(reset_sim),
                .spike_in(CN2_spk),
                .postsynaptic_spike_in(each_spike_CN2_to_MN),
                .I_out(I_synapse_CN2_to_MN),  // updates once per population (scaling factor 1024) 
                .each_I(i_EPSC_CN2_to_MN_preamp) // updates on each synapse
    );
    //****** Syanpse gain ********//
    wire [31:0] i_EPSC_CN2_to_MN;
   unsigned_mult32 syn551_gain(.out(i_EPSC_CN2_to_MN), .a(i_EPSC_CN2_to_MN_preamp), .b(i_gain_syn_CN2_to_MN));

    
 
   assign i_drive_to_MN_F0 =i_EPSC_SN_to_MN + i_EPSC_SN_to_MN_II + i_EPSC_CN_to_MN + i_EPSC_CN2_to_MN;
   //add Spinal_n_Transcortical_Current(.x(each_I_synapse_SPINAL), .y(each_I_synapse_CN_END), .out(each_I_synapse));  


    
    reg [31:0] i_drive_to_MN; // _pxi = from PXI system in BBDL
    reg [31:0] i_drive_to_CN;
    reg [31:0] i_drive_to_CN2;

    // latching 
    always @(posedge neuron_clk or posedge reset_sim)
	 begin
	   if (reset_sim)
		begin
		  i_drive_to_MN <= 32'h0;
          i_drive_to_CN <= 32'h0;
          i_drive_to_CN2 <= 32'h0;

		end else begin
		  i_drive_to_MN <= i_drive_to_MN_F0;
          i_drive_to_CN <= i_drive_to_CN_F0; 
          i_drive_to_CN2 <= i_drive_to_CN2_F0;

		end
      end
    
    //randomize current input before entering MN.
    
      wire [31:0] rand_out_MN1;
       rng rng_MN1(
                .clk1(neuron_clk),
                .clk2(neuron_clk),
                .reset(reset_sim),
                .out(rand_out_MN1)
        ); 
      wire [31:0]  i_rng_drive_to_MN1;
     assign i_rng_drive_to_MN1= {i_drive_to_MN[31:10] , rand_out_MN1[9:0]};
        
     
         
      wire [31:0] rand_out_MN3;
       rng rng_MN3(
                .clk1(neuron_clk),
                .clk2(neuron_clk),
                .reset(reset_sim),
                .out(rand_out_MN3)
        ); 
      wire [31:0] i_rng_drive_to_MN3;
     assign i_rng_drive_to_MN3= {i_drive_to_MN[31:10] , rand_out_MN3[9:0]};
     
         
      wire [31:0] rand_out_MN5;
       rng rng_MN5(
                .clk1(neuron_clk),
                .clk2(neuron_clk),
                .reset(reset_sim),
                .out(rand_out_MN5)
        ); 
      wire [31:0] i_rng_drive_to_MN5;
     assign i_rng_drive_to_MN5= {i_drive_to_MN[31:10] , rand_out_MN5[9:0]};
     
     
     
    
   //********* izneuron *************//

	
//    izneuron neuron_pool_MN(
//                .clk(neuron_clk),
//                .reset(reset_sim),
//                .I_in(i_drive_to_MN),
//                .spike(),
//                .each_spike(MN_spk)
//    );

    //MN's for size principle
    //MN1
    wire MN1_spk;
    wire [31:0] v_neuron0_MN1;
    wire each_spike_neuron0_MN, spike_neuron0_MN1;
    wire [127:0] population_neuron0_MN1;
    izneuron_th_control izneuronMN1(
        .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
        .reset(reset_sim),           // reset to initial conditions
        .I_in(  ((i_rng_drive_to_MN1)*75)>>4 ),          // input current from synapse
        .th_scaled(32'd30720),            // default 30mv threshold scaled x1024
        .v_out(v_neuron0_MN1),               // membrane potential
        .spike(spike_neuron0_MN1),           // spike sample
        .each_spike(MN1_spk), // raw spikes
        .population(population_neuron0_MN1)  // spikes of population per 1ms simulation time
    );
    
    
    //MN3 
	wire MN3_spk;
    wire [31:0] v_neuron0_MN3;
    wire each_spike_neuron0_MN3, spike_neuron0_MN3;
    wire [127:0] population_neuron0_MN3;
    izneuron_th_control izneuronMN3(
        .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
        .reset(reset_sim),           // reset to initial conditions
        .I_in( ((i_rng_drive_to_MN3)*30)>>4 ),          // input current from synapse
        .th_scaled(32'd30720),            // default 30mv threshold scaled x1024
        .v_out(v_neuron0_MN3),               // membrane potential
        .spike(spike_neuron0_MN3),           // spike sample
        .each_spike(MN3_spk), // raw spikes
        .population(population_neuron0_MN3)  // spikes of population per 1ms simulation time
    );
    
     //MN5
	wire MN5_spk;
    wire [31:0] v_neuron0_MN5;
    wire each_spike_neuron0_MN5, spike_neuron0_MN5;
    wire [127:0] population_neuron0_MN5;
    izneuron_th_control izneuronMN5(
        .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
        .reset(reset_sim),           // reset to initial conditions
        .I_in( ((i_rng_drive_to_MN5)*13)>>4 ),          // input current from synapse
        .th_scaled(32'd30720),            // default 30mv threshold scaled x1024
        .v_out(v_neuron0_MN5),               // membrane potential
        .spike(spike_neuron0_MN5),           // spike sample
        .each_spike(MN5_spk), // raw spikes
        .population(population_neuron0_MN5)  // spikes of population per 1ms simulation time
    );
	
 
     wire [31:0]  i_MN1_spkcnt;
    spike_counter  sync_counter_MN1
     (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(MN1_spk),
                        .spike_count(i_MN1_spkcnt) );
                        
     wire [31:0]  i_MN3_spkcnt;
    spike_counter  sync_counter_MN3
     (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(MN3_spk),
                        .spike_count(i_MN3_spkcnt) );
                        
    wire [31:0]  i_MN5_spkcnt;
    spike_counter  sync_counter_MN5
     (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(MN5_spk),
                        .spike_count(i_MN5_spkcnt) );

   wire [31:0] i_total_spike_count_sync_MNs;
    //assign total_spike_count_sync = spike_count_neuron_sync_MN1 + spike_count_neuron_sync_MN2+ spike_count_neuron_sync_MN3+ spike_count_neuron_sync_MN4 + spike_count_neuron_sync_MN5 + spike_count_neuron_sync_MN6 + spike_count_neuron_sync_MN7;

    assign i_total_spike_count_sync_MNs = ((i_MN1_spkcnt*32'd13)>>4) + 
                                        ((i_MN3_spkcnt*32'd30)>>4) + 
                                        ((i_MN5_spkcnt*32'd75)>>4);

//    
//	wire    [31:0] i_MN_spkcnt;
//	wire    dummy_slow_MN;  
//	spikecnt	spike_cnt_MN 
//	(		.spike(MN_spk),
//			.int_cnt_out(i_MN_spkcnt),
//			.slow_clk(sim_clk),
//			.fast_clk(clk1),
//			.reset(reset_sim),
//			.clear_out(dummy_slow_MN));
//    

    
//            
//	wire    [31:0] i_combined_spkcnt;
//	wire    dummy_slow_combined;  
//	spikecnt	spike_cnt_combined 
//	(		.spike(spike_SL_LL_combined),
//			.int_cnt_out(i_combined_spkcnt),
//			.slow_clk(sim_clk),
//			.fast_clk(clk1),
//			.reset(reset_sim),
//			.clear_out(dummy_slow_combined));
			

	
	
	
 // *** Shadmehr muscle: spike_count_out => f_active_state => f_total_force
    wire    [31:0]  f_actstate, f_MN_spkcnt; 
	 wire    [31:0]   f_force;
    wire    [31:0] IEEE_1p57, IEEE_2p77;
    assign IEEE_1p57 = 32'h3FC8F5C3; 
    assign IEEE_2p77 = 32'h403147AE;    
   // sub get_bic_len(.x(IEEE_2p77), .y(trigger_input?  f_len_bic_pxi: f_len_bic), .out(f_muscleInput_len_bic));  

    shadmehr_muscle muscle_foo
    (   .i_spike_cnt(i_total_spike_count_sync_MNs),
//        .pos(trigger_input?  f_len_bic_pxi: f_len_bic),  // muscle length
        .f_pos(f_len_pxi),  // muscle length
        //pos(32'h3F8147AE),  // muscle length 1.01
        //.vel(current_vel),
        .f_vel(f_velocity),
        .clk(sim_clk),
        .reset(reset_sim),
        .f_total_force_out(f_force),
        //.f_current_A(f_actstate),
        //.f_current_fp_spikes(f_MN_spkcnt), 
		  .f_tau(tau)
    );   
    
    
    /*** fuglevand twitch model for force ***/
//    wire    [31:0]  f_current_h, f_current_fp_spikes;
//     shadmehr_active_force fuglevand_foo
//    (   .i_spikes(i_MN_spkcnt),
//        .f_active_force_out(f_force),
//        .f_fp_spikes_out(f_MN_spkcnt),
//        .clk(sim_clk), 
//        .reset(reset_sim),
//        .f_tau(tau)
//     );   
    wire 	[31:0]	f_weightout;
    


//
//    wire [31:0] f_emg;
//    emg_parameter emg_parater_foo
//    (   .f_total_emg_out(f_emg), 
//        .i_spike_cnt(i_total_spike_count_sync_MNs), 
//        .b1_F0(32'h3A9E55C1),      //0.001208 (b1 default)
//        .b2_F0(32'hBAA6DACB),       //-0.001273 (b2 default)
//        .a1_F0(32'hC00F3B64),       //- 2.238 (a1 default)
//        .a2_F0(32'h3FD5C28F),       //1.67 (a2 default)
//        .a3_F0(32'hBED49518),       // - 0.4152 (a3 default)
//        .clk(sim_clk), 
//        .reset(reset_sim) ); 



	 
 // ** LEDs
    assign led[0] = ~reset_global;
    assign led[1] = ~reset_sim;
    assign led[2] = ~spike_in1;
    assign led[3] = ~SN_spk;
    assign led[4] = ~CN_spk;
	assign led[5] = ~CN2_spk;  // SN_bic_spk + spike_in1
//    assign led[5] = ~MN_tri_spike;
    assign led[6] = ~MN1_spk; // slow clock
    //assign led[5] = ~spike;
    //assign led[5] = ~button1_response;
    //assign led[6] = ~button2_response;
    //assign led[6] = ~reset_sim;
    assign led[7] = ~sim_clk; //fast clock
    //assign led[6] = ~execute; // When execute==1, led lits      
    // *** Buttons, physical on XEM3010, software on XEM3050 & XEM6010
    assign reset_global = ep00wire[0];
    assign reset_sim = ep00wire[1];
    
    // *** Endpoint connections:
    assign pin0 = neuron_clk_temp[0];   
    assign pin1 = sim_clk;
    assign pin2 = neuron_clk;
	 
    assign spike_out1 = SN_spk;
    //assign spike_longlatency = spike_in1;

  // Instantiate the okHost and connect endpoints.
    // Host interface
    // *** Endpoint connections:
  
    okHost okHI(
        .hi_in(hi_in), .hi_out(hi_out), .hi_inout   (hi_inout), .hi_aa(hi_aa), .ti_clk(ti_clk),
        .ok1(ok1), .ok2(ok2));
        
    parameter NUM_OK_IO = 24;

    wire [NUM_OK_IO*17 - 1: 0]  ok2x;
    okWireOR # (.N(NUM_OK_IO)) wireOR (ok2, ok2x);
    okWireIn     wi00 (.ok1(ok1),                           .ep_addr(8'h00), .ep_dataout(ep00wire));
    okWireIn     wi01 (.ok1(ok1),                           .ep_addr(8'h01), .ep_dataout(ep01wire));
    okWireIn     wi02 (.ok1(ok1),                           .ep_addr(8'h02), .ep_dataout(ep02wire));
    okWireIn     wi03 (.ok1(ok1),                           .ep_addr(8'h03), .ep_dataout(ep03wire));
    okWireIn     wi04 (.ok1(ok1),                           .ep_addr(8'h04), .ep_dataout(ep04wire));
    okWireIn     wi05 (.ok1(ok1),                           .ep_addr(8'h05), .ep_dataout(ep05wire));
    okWireIn     wi06 (.ok1(ok1),                           .ep_addr(8'h06), .ep_dataout(ep06wire));
    okWireIn     wi07 (.ok1(ok1),                           .ep_addr(8'h07), .ep_dataout(ep07wire));
    okWireIn     wi08 (.ok1(ok1),                           .ep_addr(8'h08), .ep_dataout(ep08wire));


    okWireOut    wo20 (.ep_datain(f_len_pxi[15:0]), .ok1(ok1), .ok2(ok2x[  0*17 +: 17 ]), .ep_addr(8'h20) );
    okWireOut    wo21 (.ep_datain(f_len_pxi[31:16]), .ok1(ok1), .ok2(ok2x[  1*17 +: 17 ]), .ep_addr(8'h21) );
    okWireOut    wo22 (.ep_datain(f_fr_Ia[15:0]), .ok1(ok1), .ok2(ok2x[  2*17 +: 17 ]), .ep_addr(8'h22) );
    okWireOut    wo23 (.ep_datain(f_fr_Ia[31:16]), .ok1(ok1), .ok2(ok2x[  3*17 +: 17 ]), .ep_addr(8'h23) );
    okWireOut    wo24 (.ep_datain(f_fr_II[15:0]), .ok1(ok1), .ok2(ok2x[  4*17 +: 17 ]), .ep_addr(8'h24) );
    okWireOut    wo25 (.ep_datain(f_fr_II[31:16]), .ok1(ok1), .ok2(ok2x[  5*17 +: 17 ]), .ep_addr(8'h25) );
    okWireOut    wo26 (.ep_datain(i_total_spike_count_sync_MNs[15:0]), .ok1(ok1), .ok2(ok2x[  6*17 +: 17 ]), .ep_addr(8'h26) );
    okWireOut    wo27 (.ep_datain(i_total_spike_count_sync_MNs[31:16]), .ok1(ok1), .ok2(ok2x[  7*17 +: 17 ]), .ep_addr(8'h27) );
    okWireOut    wo28 (.ep_datain(i_EPSC_CN_to_MN[15:0]),  .ok1(ok1), .ok2(ok2x[ 8*17 +: 17 ]), .ep_addr(8'h28) );
    okWireOut    wo29 (.ep_datain(i_EPSC_CN_to_MN[31:16]), .ok1(ok1), .ok2(ok2x[ 9*17 +: 17 ]), .ep_addr(8'h29) );
    okWireOut    wo2A (.ep_datain(i_MN1_spkcnt[15:0]),  .ok1(ok1), .ok2(ok2x[ 10*17 +: 17 ]), .ep_addr(8'h2A) );
    okWireOut    wo2B (.ep_datain(i_MN1_spkcnt[31:16]), .ok1(ok1), .ok2(ok2x[ 11*17 +: 17 ]), .ep_addr(8'h2B) );
    okWireOut    wo2C (.ep_datain(f_force[15:0]),  .ok1(ok1), .ok2(ok2x[ 12*17 +: 17 ]), .ep_addr(8'h2C) );
    okWireOut    wo2D (.ep_datain(f_force[31:16]), .ok1(ok1), .ok2(ok2x[ 13*17 +: 17 ]), .ep_addr(8'h2D) );   
    okWireOut    wo2E (.ep_datain(population_neuron0_MN1[15:0]),  .ok1(ok1), .ok2(ok2x[ 14*17 +: 17 ]), .ep_addr(8'h2E) );
    okWireOut    wo2F (.ep_datain(population_neuron0_MN1[31:16]), .ok1(ok1), .ok2(ok2x[ 15*17 +: 17 ]), .ep_addr(8'h2F) ); 
//    okWireOut    wo30 (.ep_datain(I_synapse_SN_to_MN[15:0]),  .ok1(ok1), .ok2(ok2x[ 16*17 +: 17 ]), .ep_addr(8'h30) );
//    okWireOut    wo31 (.ep_datain(I_synapse_SN_to_MN[31:16]), .ok1(ok1), .ok2(ok2x[ 17*17 +: 17 ]), .ep_addr(8'h31) );  
//    okWireOut    wo32 (.ep_datain(i_SN_spkcnt_II[15:0]),  .ok1(ok1), .ok2(ok2x[ 18*17 +: 17 ]), .ep_addr(8'h32) );
//    okWireOut    wo33 (.ep_datain(i_SN_spkcnt_II[31:16]), .ok1(ok1), .ok2(ok2x[ 19*17 +: 17 ]), .ep_addr(8'h33) );       
    
    //ep_ready = 1 (always ready to receive)
    okBTPipeIn   ep80 (.ok1(ok1), .ok2(ok2x[ 16*17 +: 17 ]), .ep_addr(8'h80), .ep_write(is_pipe_being_written), .ep_blockstrobe(), .ep_dataout(hex_from_py), .ep_ready(1'b1));
    //okBTPipeOut  epA0 (.ok1(ok1), .ok2(ok2x[ 5*17 +: 17 ]), .ep_addr(8'ha0), .ep_read(pipe_out_read),  .ep_blockstrobe(), .ep_datain(response_nerf), .ep_ready(pipe_out_valid));
    //okBTPipeOut  epA0 (.ok1(ok1), .ok2(ok2x[ 11*17 +: 17 ]), .ep_addr(8'ha1), .ep_read(pipe_out_read),  .ep_blockstrobe(), .ep_datain(rawspikes), .ep_ready(1'b1));

    okTriggerIn ep50 (.ok1(ok1),  .ep_addr(8'h50), .ep_clk(clk1), .ep_trigger(ep50trig));
endmodule




module unsigned_mult32 (out, a, b);
	output 	[31:0]	out;
	input 	[31:0] 	a;
	input 	[31:0] 	b;
    wire    [63:0]   temp_out; 

	assign temp_out = a * b;
	assign out = temp_out[31:0];
endmodule
