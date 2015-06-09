
`default_nettype none
`timescale 1ns / 1ps

// rack_mid_node1_xem6010.v
// Generated on Tue Mar 12 15:55:02 -0700 2013

    module rack_CN_simple_S1M1_xem6010(
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
	    
	    // Neuron array inputs
          input wire spikein1,  
          input wire spikein2,
          input wire spikein3,
          input wire spikein4,
          input wire spikein5,
          input wire spikein6,
          input wire spikein7,
          input wire spikein8,
          input wire spikein9,
          input wire spikein10,
          input wire spikein11,
          input wire spikein12,
          input wire spikein13,
          input wire spikein14,
      
          // Neuron array outputs
          output wire spikeout1, 
          output wire spikeout2,
          output wire spikeout3,
          output wire spikeout4,
          output wire spikeout5,
          output wire spikeout6,
          output wire spikeout7,
          output wire spikeout8,
          output wire spikeout9,
          output wire spikeout10,
          output wire spikeout11,
          output wire spikeout12,
          output wire spikeout13,
          output wire spikeout14
       );
       
        parameter NN = 8;
		
        // *** Dump all the declarations here:
        wire         ti_clk;
        wire [30:0]  ok1;
        wire [16:0]  ok2;   
        wire reset_global, reset_sim;
        wire is_from_trigger;

        // *** Target interface bus:
        assign i2c_sda = 1'bz;
        assign i2c_scl = 1'bz;
        assign hi_muxsel = 1'b0;
        
        
        wire [31:0] threshold30mv;  // iz neuron threshold 
        assign threshold30mv = 32'd30;
    

      
/////////////////////// BEGIN WIRE DEFINITIONS ////////////////////////////

        // Synapse synapse0 Wire Definitions     (Ia)    
        wire [31:0] I_synapse0;   // sample of the synaptic current (updates once per 1ms simulation time)
        wire [31:0] each_I_synapse0;  // raw synaptic currents
        
         // Synapse synapse0 Wire Definitions (II)   
        wire [31:0] I_synapse0__II;   // sample of the synaptic current (updates once per 1ms simulation time)
        wire [31:0] each_I_synapse0_II;  // raw synaptic currents
        
                // Synapse synapse1 Wire Definitions        
        wire [31:0] I_synapse1;   // sample of the synaptic current (updates once per 1ms simulation time)
        wire [31:0] each_I_synapse1;  // raw synaptic currents

        
        // triggered input f_Ia_gani
        reg [31:0] f_Ia_gain;
        reg [31:0] f_II_gain;
        
        // Triggered Input triggered_input0 Wire Definitions
        reg [31:0] triggered_input0;    // Triggered input sent from USB (ltp)       
        

        // Triggered Input triggered_input1 Wire Definitions
        reg [31:0] triggered_input1;    // Triggered input sent from USB (ltd)       
        

        // Triggered Input triggered_input2 Wire Definitions
        reg [31:0] triggered_input2;    // Triggered input sent from USB (p_delta)       
        

        // Triggered Input triggered_input3 Wire Definitions
        reg [31:0] triggered_input3;    // Triggered input sent from USB (syn_gain)    
   
        
        reg [31:0] f_overflow_proportion;
        // Triggered Input triggered_input4 Wire Definitions
        reg [31:0] triggered_input4;    // Triggered input sent from USB (clk_divider)       
        

        // Spike Counter spike_counter0 Wire Definitions
        wire [31:0] spike_count_neuron0;
        

    // FPGA Input/Output Rack Wire Definitions
    // these are in the top module input/output list
    

        // Output and OpalKelly Interface Wire Definitions
        
        wire [20*17-1:0] ok2x;
        wire [15:0] ep00wire, ep01wire, ep02wire;
        wire [15:0] ep50trig;
        
        wire pipe_in_write;
        wire [15:0] pipe_in_data;
        

        // Clock Generator clk_gen0 Wire Definitions
        wire neuron_clk;  // neuron clock (128 cycles per 1ms simulation time) 
        wire sim_clk;     // simulation clock (1 cycle per 1ms simulation time)
        wire spindle_clk; // spindle clock (3 cycles per 1ms simulation time)
        

        // Neuron neuron0 Wire Definitions
        wire [31:0] v_neuron0;   // membrane potential
        wire spike_neuron0;      // spike sample for visualization only
        wire each_spike_neuron0; // raw spike signals
        wire each_spike_neuron1; // raw spike signals
        wire [127:0] population_neuron0; // spike raster for entire population        
        
/////////////////////// END WIRE DEFINITIONS //////////////////////////////

/////////////////////// BEGIN INSTANCE DEFINITIONS ////////////////////////


        wire [31:0] f_I_synapse_Ia;
        synapse_simple synapse_simple_from_Ia(
            .clk(sim_clk),
            .reset(reset_sim),
            .spike_in(spikein1),
            .f_I_out(f_I_synapse_Ia)
        );
        
      
         wire [31:0] f_I_synapse_II;
        synapse_simple synapse_simple_from_II(
            .clk(sim_clk),
            .reset(reset_sim),
            .spike_in(spikein5),
            .f_I_out(f_I_synapse_II)
        );
        
        
   
   
 
        wire [31:0] f_Ia_weighted;
        mult Ia_to_CN(.x(f_I_synapse_Ia), .y(f_Ia_gain), .out(f_Ia_weighted));
        
        wire [31:0] f_II_weighted;
        mult II_to_CN(.x(f_I_synapse_II), .y(f_II_gain), .out(f_II_weighted));


        
        //*********** add currents from two synapse (Ia, II)  *********
        wire [31:0] f_I_synapse_both;
        add addCurrentsFrom_Ia_and_II(.x(f_Ia_weighted), .y(f_II_weighted), .out(f_I_synapse_both));
        
         
        //*********** add currents from extra cortical input 1(M1)  *********
        //gain control for extraCN synapse output
//        wire [31:0] f_gain_controlled_I_synapse_extraCN1;
//        mult mult_synapse_simple0_extraCN1(.x(f_I_synapse_M1extra1), .y(f_extraCN_syn_gain), .out(f_gain_controlled_I_synapse_extraCN1));
//        
//        wire [31:0]  i_I_from_CN1extra;
//        floor   float_to_int_CN1extra(
//            .in(f_I_synapse_M1extra1),
//            .out(i_I_from_CN1extra)
//        );

//        wire [31:0] f_SNsCN_M1extra1;
//        add addCurrentsFrom_extra1(.x(f_I_synapse_both), .y(f_gain_controlled_I_synapse_extraCN1), .out(f_SNsCN_M1extra1));
//        
          //*********** add currents from extra cortical input 2(M1)  *********
          
           //gain control for CN synapse output
//        wire [31:0] f_gain_controlled_I_synapse_extraCN2;
//        mult mult_synapse_simple0_extraCN2(.x(f_I_synapse_M1extra2), .y(f_extraCN_syn_gain), .out(f_gain_controlled_I_synapse_extraCN2));

//        wire [31:0]  i_I_from_CN2extra;
//        floor   float_to_int_CN2extra(
//            .in(f_I_synapse_M1extra2),
//            .out(i_I_from_CN2extra)
//        );
        
        wire [31:0] i_I_from_CN1extra_buttonScaled;
        unsigned_mult32  scale_CN1extra(.out(i_I_from_CN1extra_buttonScaled), .a(i_CN1_extra_drive), .b(32'd1)); //i_stuffed_scaler
        
        
//         assign i_rng_CN1_extra_drive= {i_I_from_CN1extra_buttonScaled[31:4] , CN1_rand_out[3:0]};  // with randomness
         assign i_rng_CN1_extra_drive= i_I_from_CN1extra_buttonScaled;    // no randomness
//       
//       assign i_I_from_CN2extra_buttonScaled = i_I_from_CN2extra * i_scaler;
        
        
        
//        
//        wire [31:0] f_extraInputs;
//        add addCurrentsFrom_extra2(.x(f_gain_controlled_I_synapse_extraCN1), .y(f_gain_controlled_I_synapse_extraCN2), .out( f_extraInputs));
//        
     
//        //*********** add currents from four synapses *********
//        wire [31:0] f_I_synapse_cortical;
//        add addCurrentsFrom_IaII_and_extras(.x(f_I_synapse_both), .y(f_SNsCN_M1extra1_2), .out(f_I_synapse_cortical));
        
        
        
//        //*********** add currents from extra cortical input 1(M1)  *********
//        wire [31:0] f_SN_M1extra1;
//        add addCurrentsFrom_extra1(.x(f_I_synapse_both), .y(f_I_synapse_M1extra1), .out(f_SN_M1extra1));
//        
//         //*********** add currents from extra cortical input 2(M1)  *********
//        wire [31:0] f_drive_to_CN;
//        add addCurrentsFrom_extra2(.x(f_SN_M1extra1), .y(f_I_synapse_M1extra2), .out(f_drive_to_CN));
//        
//       
 
        wire [31:0] f_I_synapse_both_gainControlled;
         mult mult_synapse(.x(f_I_synapse_both), .y(f_extraCN_syn_gain), .out(f_I_synapse_both_gainControlled));

        wire [31:0]  i_I_from_spindle;
        floor   float_to_int_fromSynapse(
            .in(f_I_synapse_both_gainControlled),
            .out(i_I_from_spindle)
        );
//        

     // S1Neuron Instance Definition
        wire [31:0] i_I_from_spindle_neuronCompensated;
        assign i_I_from_spindle_neuronCompensated = i_I_from_spindle <<< 9;  // always scale after neuron (neuronCompensation)


//
//        wire [31:0] v_neuron_S1;   // membrane potential
//        wire spike_neuron_S1;      // spike sample for visualization only
//        wire each_spike_neuron_S1; // raw spike signals
//        wire [127:0] population_neuron_S1; // spike raster for entire population        
        
//        iz_corticalneuron_th_control S1Neuron(
//            .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
//            .reset(reset_sim),           // reset to initial conditions
//            .I_in(  (i_I_from_spindle_neuronCompensated)),          // input current from synapse
//            .th_scaled( threshold30mv <<< 10),                 // threshold
//            .v_out(v_neuron_S1),               // membrane potential
//            .spike(spike_neuron_S1),           // spike sample
//            .each_spike(each_spike_neuron_S1), // raw spikes
//            .population(population_neuron_S1)  // spikes of population per 1ms simulation time
//        ); 
//
//
//        wire [31:0] f_I_synapse_S1;
//        synapse_simple synapse_simple_from_S1Neuron(
//            .clk(sim_clk),
//            .reset(reset_sim),
//            .spike_in(each_spike_neuron_S1),
//            .f_I_out(f_I_synapse_S1)
//        );
        
        
//       wire [31:0] f_I_synapse_S1_gainControlled;
//       mult mult_synapse(.x(f_I_synapse_S1), .y(f_extraCN_syn_gain), .out(f_I_synapse_S1_gainControlled));
////        
//        wire [31:0]  i_I_from_S1;
//        floor   float_to_int_fromS1(
//            .in(f_I_synapse_S1_gainControlled),
//            .out(i_I_from_S1)
//        );
//        wire [31:0] i_I_from_S1_neuronCompensated_1st;
//        assign i_I_from_S1_neuronCompensated_1st = i_I_from_S1 <<< 9;


        wire [31:0] fixed_drive_to_CN_F0;
        //assign fixed_drive_to_CN_F0 = i_I_from_spindle + i_I_from_extras;
//        assign fixed_drive_to_CN_F0 = i_I_from_spindle <<< 9 + i_I_from_CN1extra+ i_I_from_CN2extra_buttonScaled; 
        //assign fixed_drive_to_CN_F0 = (i_I_from_spindle <<< 9) + i_I_from_CN1extra_buttonScaled + i_CN2_extra_drive;
        
        //
//        assign fixed_drive_to_CN_F0 =  (i_I_from_spindle <<< (9 + i_stuffed_scaler)) +  (i_CN2_extra_drive) ; //button increase the sensory gain like crazy.  (Trial 4): sensory gain upup. 
        
        

//         wire [31:0] i_adjusted_I_from_spindle = i_I_from_spindle <<< 9;    // adjusting baseline sensory gain  //092513 fix (Trial 4): HI-GAIN
          
        
        wire [31:0] i_gainScaled_I_from_spindle;    //092513 fix (Trial 4): HI-GAIN

        unsigned_mult32  sensoryGain_scaleCN(.out(i_gainScaled_I_from_spindle), .a(i_I_from_spindle_neuronCompensated), .b(i_stuffed_scaler)); //092513 fix (Trial 4): HI-GAIN
       
//        assign fixed_drive_to_CN_F0 =  i_I_from_spindle_neuronCompensated + i_gainScaled_I_from_spindle + i_CN2_extra_drive; //092613  HI-GAIN


        assign fixed_drive_to_CN_F0 = i_I_from_spindle_neuronCompensated  + (i_rng_CN1_extra_drive <<< 4) + i_CN2_extra_drive; // (Trial 5):TONIC
       
        // i_I_from_CN1extra:0~10 (15000 amp),  i_I_from_CN2extra_buttonScaled: 1~5  constantly. (4000 * 1~5)
        //fixed_drive_to_CN :5000~ 500000!
        
        
        
        
        // Triggered Input triggered_input0 Instance Definition (ltp)
        always @ (posedge ep50trig[12] or posedge reset_global)
        if (reset_global)
            triggered_input0 <= 32'd0;         //reset to 0      
        else
            triggered_input0 <= {ep02wire, ep01wire};      
        

        // Triggered Input triggered_input1 Instance Definition (ltd)
        always @ (posedge ep50trig[11] or posedge reset_global)
        if (reset_global)
            triggered_input1 <= 32'd0;         //reset to 0      
        else
            triggered_input1 <= {ep02wire, ep01wire};      
        

        // Triggered Input triggered_input2 Instance Definition (p_delta)
        always @ (posedge ep50trig[10] or posedge reset_global)
        if (reset_global)
            triggered_input2 <= 32'd0;         //reset to 0      
        else
            triggered_input2 <= {ep02wire, ep01wire};      
        


        // Triggered Input f_Ia_gain Instance Definition (f_Ia_gain)
        always @ (posedge ep50trig[1] or posedge reset_global)
        if (reset_global)
            f_Ia_gain <= 32'h3F800000;           //reset to 1.0      
        else
            f_Ia_gain <= {ep02wire, ep01wire};      
        
        // Triggered Input f_II_gain Instance Definition (f_II_gain)
        always @ (posedge ep50trig[2] or posedge reset_global)
        if (reset_global)
            f_II_gain <= 32'h3F800000;          //reset to 1.0      
        else
            f_II_gain <= {ep02wire, ep01wire};              
            
        // Triggered Input triggered_input3 Instance Definition (syn_gain)
        always @ (posedge ep50trig[3] or posedge reset_global)
        if (reset_global)
            triggered_input3 <= 32'd1;         //reset to 1.0      
        else
            triggered_input3 <= {ep02wire, ep01wire};      
            
         // Triggered Input overflow Instance Definition (overflow proportion) :0.0 ~ 1.0
        always @ (posedge ep50trig[4] or posedge reset_global)
        if (reset_global)
            f_overflow_proportion <= 32'h3F800000;         //reset to 1.0     range :0.0 ~ 1.0
        else
            f_overflow_proportion <= {ep02wire, ep01wire};      
            
        reg [31:0] CN_offset;
        // Triggered Input Instance Definition (offset to subtract )
        always @ (posedge ep50trig[6] or posedge reset_global)
        if (reset_global)
            CN_offset <= 32'd0;         //reset to 0.0      
        else
            CN_offset <= {ep02wire, ep01wire};          
        

        // Triggered Input triggered_input4 Instance Definition (clk_divider)
        always @ (posedge ep50trig[7] or posedge reset_global)
        if (reset_global)
            triggered_input4 <= 32'd381;         //rhard coded half count for 0.5x real speed, 381 for real time speed  
        else
            triggered_input4 <= {ep02wire, ep01wire};      
            
        // extra cortical drive, controlled in python frontend    
//        reg [31:0] i_CN_extra_drive;
//        
//        always @(posedge ep50trig[8] or posedge reset_global)
//        begin
//            if (reset_global) begin    
//                i_CN_extra_drive <= 32'd0;             
//            end
//            else begin           
//                i_CN_extra_drive <= {ep02wire, ep01wire};       
//            end
//        end   
//        
        
     
        // Triggered Input triggered_input4 Instance Definition (extra cortical input 1)
         reg [31:0] i_CN1_extra_drive;
        
        always @ (posedge ep50trig[8] or posedge reset_global)
        if (reset_global)
            i_CN1_extra_drive <= 32'd0;         //rhard coded half count for 0.5x real speed, 381 for real time speed  
        else
            i_CN1_extra_drive <= {ep02wire, ep01wire};      
            
            
        // Triggered Input triggered_input0 Instance Definition (extra cortical input 2 (wave))
        reg [31:0] f_CN2_extra_drive;
        
        always @ (posedge ep50trig[9] or posedge reset_global)
        if (reset_global)
            f_CN2_extra_drive <= 32'h0;         //reset to 0      
        else
            f_CN2_extra_drive <= {ep02wire, ep01wire};    
        
        
       // Triggered Input extra CNs Gain Instance Definition 
        reg [31:0] f_extraCN_syn_gain;
        always @ (posedge ep50trig[13] or posedge reset_global)
        if (reset_global)
            f_extraCN_syn_gain <= 32'h40000000;         //reset to 2.0  
        else
            f_extraCN_syn_gain <= {ep02wire, ep01wire};    
        
        
     wire [31:0]  spike_count_neuron_sync_CN1;
      spike_counter  sync_counter_CN1
      (                 .clk(neuron_clk),
                        .reset(reset_sim),
                        .spike_in(each_spike_neuron_CN1),
                        .spike_count(spike_count_neuron_sync_CN1) );       

        
//
//        // Spike Counter spike_counter0 Instance Definition
//	wire    dummy_slow;
//        spikecnt_async	spike_counter0
//        (      .spike(each_spike_neuron0),
//                .int_cnt_out(spike_count_neuron0),
//                .slow_clk(sim_clk),
//                .fast_clk(clk1),
//                .reset(reset_global),
//                .clear_out(dummy_slow));


    
        // Waveform Generator mixed_input0 Wire Definitions
        wire [31:0] mixed_input0;   // Wave out signal
 

        // Waveform Generator mixed_input0 Instance Definition        
        waveform_from_pipe_bram_2s gen_mixed_input_CN1(
            .reset(reset_sim),               // reset the waveform
            .pipe_clk(ti_clk),                  // target interface clock from opalkelly interface
            .pipe_in_write(pipe_in_write),      // write enable signal from opalkelly pipe in
            .data_from_trig(f_CN2_extra_drive),	// data from one of ep50 channel
            .is_from_trigger(is_from_trigger),
            .pipe_in_data(pipe_in_data),        // waveform data from opalkelly pipe in (e.g.sine wave)
            .pop_clk(sim_clk),                  // trigger next waveform sample every 1ms
            .wave(mixed_input0)                   // wave out signal
        );

        wire [31:0]  i_CN2_extra_drive;
        floor   synapse_float_to_int2(
//            .in(f_drive_to_CN),
            .in(mixed_input0),
            .out(i_CN2_extra_drive)
        );
  
        
        
    //FPGA-FPGA Outputs
    assign spikeout1 = spike_neuron_CN1;
    assign spikeout2 = each_spike_neuron_CN1;
    assign spikeout3 = 1'b0;
    assign spikeout4 = 1'b0;
    assign spikeout5 = 1'b0;
    assign spikeout6 = each_spike_neuron_CN1_OF;
    assign spikeout7 = 1'b0;
    assign spikeout8 = 1'b0;
    assign spikeout9 = 1'b0;
    assign spikeout10 = 1'b0;
    assign spikeout11 = 1'b0;
    assign spikeout12 = 1'b0;
    assign spikeout13 = 1'b0;
    assign spikeout14 = 1'b0;





    //***** button gain controller test *******
    
    //-------
   // DFFs
   //-------
   
    reg [3:0] i_pre_scaler;
    always @(posedge sim_clk  or posedge reset_global) begin
        if(reset_global) begin
            i_pre_scaler <= 4'b0000;   // '001'0  => '0' 
        end
        else if (temp_input_1d ^ temp_input) begin // detect level change  
            if (i_pre_scaler == 4'b1110) begin // '111'0 -> '7'
                i_pre_scaler <= 4'b0000;
            end else begin 
                i_pre_scaler <= i_pre_scaler + 1;
            end
        end
        else begin
            i_pre_scaler <= i_pre_scaler;
        end
    end
    
    wire [3:0] i_scaler;
    assign i_scaler = i_pre_scaler >> 1;
    

    wire [31:0] i_stuffed_scaler;
    assign i_stuffed_scaler = {28'd0, i_scaler[3:0]};
      
    
    
    reg temp_input, temp_input_1d;
    always @(posedge sim_clk) begin
        temp_input <= spikein6;
        temp_input_1d <= temp_input;
    end



        // Output and OpalKelly Interface Instance Definitions
        //assign led = 0;
        //wire reset_external;
        reg reset_external_clean;
       always @ (posedge sim_clk)
        if (spikein14)
            reset_external_clean <= spikein14;      
        else
            reset_external_clean <= 0;    

        
        
        assign reset_global = ep00wire[0] | reset_external_clean;
        assign reset_sim = ep00wire[2] ;
        assign is_from_trigger = ~ep00wire[1];
        okWireOR # (.N(18)) wireOR (ok2, ok2x);
        okHost okHI(
            .hi_in(hi_in),  .hi_out(hi_out),    .hi_inout(hi_inout),    .hi_aa(hi_aa),
            .ti_clk(ti_clk),    .ok1(ok1),  .ok2(ok2)   );
        
        //okTriggerIn ep50    (.ok1(ok1), .ep_addr(8'h50),    .ep_clk(clk1),  .ep_trigger(ep50trig)   );
        okTriggerIn ep50    (.ok1(ok1), .ep_addr(8'h50),    .ep_clk(sim_clk),  .ep_trigger(ep50trig)   );
        
        okWireIn    wi00    (.ok1(ok1), .ep_addr(8'h00),    .ep_dataout(ep00wire)   );
        okWireIn    wi01    (.ok1(ok1), .ep_addr(8'h01),    .ep_dataout(ep01wire)   );
        okWireIn    wi02    (.ok1(ok1), .ep_addr(8'h02),    .ep_dataout(ep02wire)   );

        okWireOut wo20 (    .ep_datain(i_rng_CN1_extra_drive[15:0]),  .ok1(ok1),  .ok2(ok2x[0*17 +: 17]), .ep_addr(8'h20)    );
        okWireOut wo21 (    .ep_datain(i_rng_CN1_extra_drive[31:16]),  .ok1(ok1),  .ok2(ok2x[1*17 +: 17]), .ep_addr(8'h21)   );    
        
        okWireOut wo22 (    .ep_datain(i_gainScaled_I_from_spindle[15:0]),  .ok1(ok1),  .ok2(ok2x[2*17 +: 17]), .ep_addr(8'h22)    );
        okWireOut wo23 (    .ep_datain(i_gainScaled_I_from_spindle[31:16]),  .ok1(ok1),  .ok2(ok2x[3*17 +: 17]), .ep_addr(8'h23)   );    
        
        okWireOut wo24 (    .ep_datain(i_I_from_spindle_neuronCompensated[15:0]),  .ok1(ok1),  .ok2(ok2x[4*17 +: 17]), .ep_addr(8'h24)    );
        okWireOut wo25 (    .ep_datain(i_I_from_spindle_neuronCompensated[31:16]),  .ok1(ok1),  .ok2(ok2x[5*17 +: 17]), .ep_addr(8'h25)   );    
        
        okWireOut wo26 (    .ep_datain(f_drive_to_CN[15:0]),  .ok1(ok1),  .ok2(ok2x[6*17 +: 17]), .ep_addr(8'h26)    );
        okWireOut wo27 (    .ep_datain(f_drive_to_CN[31:16]),  .ok1(ok1),  .ok2(ok2x[7*17 +: 17]), .ep_addr(8'h27)   );    
        
        okWireOut wo28 (    .ep_datain(i_CN2_extra_drive[15:0]),  .ok1(ok1),  .ok2(ok2x[8*17 +: 17]), .ep_addr(8'h28)    );
        okWireOut wo29 (    .ep_datain(i_CN2_extra_drive[31:16]),  .ok1(ok1),  .ok2(ok2x[9*17 +: 17]), .ep_addr(8'h29)   );  

        okWireOut wo2A (    .ep_datain(i_I_from_spindle[15:0]),  .ok1(ok1),  .ok2(ok2x[10*17 +: 17]), .ep_addr(8'h2A)    );
        okWireOut wo2B (    .ep_datain(i_I_from_spindle[31:16]),  .ok1(ok1),  .ok2(ok2x[11*17 +: 17]), .ep_addr(8'h2B)   ); 
        
        okWireOut wo2C (    .ep_datain(i_time[15:0]),  .ok1(ok1),  .ok2(ok2x[12*17 +: 17]), .ep_addr(8'h2C)    );
        okWireOut wo2D (    .ep_datain(i_time[31:16]),  .ok1(ok1),  .ok2(ok2x[13*17 +: 17]), .ep_addr(8'h2D)   ); 

        okWireOut wo2E (    .ep_datain(i_scaled_drive_to_CN[15:0]),  .ok1(ok1),  .ok2(ok2x[14*17 +: 17]), .ep_addr(8'h2E)    );
        okWireOut wo2F (    .ep_datain(i_scaled_drive_to_CN[31:16]),  .ok1(ok1),  .ok2(ok2x[15*17 +: 17]), .ep_addr(8'h2F)   ); 
        
        
        okBTPipeIn ep80 (   .ok1(ok1), .ok2(ok2x[16*17 +: 17]), .ep_addr(8'h80), .ep_write(pipe_in_write),
                            .ep_blockstrobe(), .ep_dataout(pipe_in_data), .ep_ready(1'b1));
        
                
        

        // Clock Generator clk_gen0 Instance Definition
        gen_clk clocks(
            .rawclk(clk1),
            .half_cnt(triggered_input4),
            .clk_out1(neuron_clk),
            .clk_out2(sim_clk),
            .clk_out3(spindle_clk),
            .int_neuron_cnt_out()
        );
        
        
        reg [31:0] i_time;

        always @(posedge sim_clk or posedge reset_global)
         begin
           if (reset_global)
            begin
              i_time <= 32'd0;
            end else begin
              i_time <= i_time + 1; 
            end
          end
        
        
        
           //***   Addition of two currents ******//
       //ire [31:0] i_drive_to_CN_F0;
       // assign i_drive_to_CN_F0 = i_EPSC_synapse0 + i_EPSC_synapse0_II + i_EPSC_synapse1 + i_CN_extra_drive;   

        // latching 
        reg [31:0] fixed_drive_to_CN;

        always @(posedge sim_clk or posedge reset_global)
         begin
           if (reset_global)
            begin
              fixed_drive_to_CN <= 32'h0;
            end else begin
              fixed_drive_to_CN <= fixed_drive_to_CN_F0; 
            end
          end
        
        
        wire [31:0] fixed_drive_to_CN_offset_subtracted;
        assign fixed_drive_to_CN_offset_subtracted = (fixed_drive_to_CN > CN_offset)? (fixed_drive_to_CN - CN_offset): 32'd0;
        
        
        wire [31:0] v_neuron_CN1;   // membrane potential
        wire spike_neuron_CN1;      // spike sample for visualization only
        wire each_spike_neuron_CN1; // raw spike signals
        wire [127:0] population_neuron_CN1; // spike raster for entire population        
        
     // CN1 Instance Definition

        iz_corticalneuron_th_control CN1(
            .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
            .reset(reset_sim),           // reset to initial conditions
            .I_in(  (fixed_drive_to_CN_offset_subtracted)),          // input current from synapse
            .th_scaled( threshold30mv <<< 10),                 // threshold
            .v_out(v_neuron_CN1),               // membrane potential
            .spike(spike_neuron_CN1),           // spike sample
            .each_spike(each_spike_neuron_CN1), // raw spikes
            .population(population_neuron_CN1)  // spikes of population per 1ms simulation time
        ); 
        
        
        wire [31:0] f_drive_to_CN;
        int_to_float fixed_drive_of(.in(fixed_drive_to_CN_offset_subtracted), .out(f_drive_to_CN)); // int to float
        
        wire [31:0] f_scaled_drive_to_CN;
        mult mult_synapse_of(.x(f_drive_to_CN), .y(f_overflow_proportion), .out(f_scaled_drive_to_CN));
         
       wire [31:0] i_scaled_drive_to_CN;
       floor   synapse_float_to_int_of(
            .in(f_scaled_drive_to_CN),
            .out(i_scaled_drive_to_CN)
        );

             
        wire [31:0] v_neuron_CN1_OF;   // membrane potential
        wire spike_neuron_CN1_OF;      // spike sample for visualization only
        wire each_spike_neuron_CN1_OF; // raw spike signals
        wire [127:0] population_neuron_CN1_OF; // spike raster for entire population        
        
        iz_corticalneuron_th_control CN1_overflow(
            .clk(neuron_clk),               // neuron clock (128 cycles per 1ms simulation time)
            .reset(reset_sim),           // reset to initial conditions
            .I_in(  (i_scaled_drive_to_CN)),          // input current from synapse
            .th_scaled( threshold30mv <<< 10),                 // threshold
            .v_out(v_neuron_CN1_OF),               // membrane potential
            .spike(spike_neuron_CN1_OF),           // spike sample
            .each_spike(each_spike_neuron_CN1_OF), // raw spikes
            .population(population_neuron_CN1_OF)  // spikes of population per 1ms simulation time
        ); 






      wire [31:0] CN1_rand_out;
      rng rng_CN1(               
        .clk1(neuron_clk),
        .clk2(neuron_clk),
        .reset(reset_sim),
        .out(CN1_rand_out)
        ); 
        
       wire [31:0] i_rng_current_to_MN1;
       wire [31:0] i_rng_CN1_extra_drive;
//       assign i_rng_CN1_extra_drive= {i_CN1_extra_drive[31:4] , CN1_rand_out[3:0]};
        
        
        
/////////////////////// END INSTANCE DEFINITIONS //////////////////////////

	// ** LEDs
    assign led[0] = ~reset_global;
    assign led[1] = ~spikein1;
    assign led[2] = ~spikein5;
    assign led[3] = ~spikein9;
    assign led[4] = ~i_stuffed_scaler[0];
    assign led[5] = ~i_stuffed_scaler[1];
    assign led[6] = ~i_stuffed_scaler[2]; // 
    assign led[7] = ~sim_clk; // clock
    
endmodule
