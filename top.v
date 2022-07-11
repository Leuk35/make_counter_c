// DESCRIPTION: Verilator: Verilog example with sub-modules
//
// This file ONLY for example
// ======================================================================

module top #(parameter SUB_NUMBER=2) /* 2 sub-modules */
  (
   // Sequential Inputs signals
   input              clk,
   input              resetn,
   
   // Combinatory Data Inputs signals
   input              in_start,
   input integer      in_select,
   input [31:0]       in_data,

   // Data Outputs signals
   output wire [(SUB_NUMBER-1):0]  out_status,
   output wire [31:0] out_data
   );

   // Internal nodes
   wire [31:0] sub_stopcount;
   wire sub_start;
   wire [(SUB_NUMBER-1):0]  sub_status;
   wire [(SUB_NUMBER-1):0][31:0] sub_data;
   
   // parameter definition
   parameter integer sub_index_0 = 0 ;
   parameter integer sub_index_1 = 1 ;

   // Connect up the outputs
   assign sub_start     = in_start;
   assign sub_stopcount = in_data;
   assign out_status    = resetn ? (in_select > 0 ? sub_status & 1<<(in_select-1): '0) : '0;
   assign out_data      = sub_data[in_select-1];
   
   // And an counter up example sub module.
   sub_counter #(.count_up(1)) counter_up  /* count_up is true => count up */
        (
            // Inputs
            .clk                       (clk),
            .resetn                    (resetn),
            .start                     (in_select[sub_index_0] & sub_start),
            .stopcount                 (sub_stopcount),
            .count                     (sub_data[sub_index_0]),
            .status                    (sub_status[sub_index_0])
        );
        
   // And an counter down example sub module.
   sub_counter #(.count_up(0)) counter_down /* count_up is false => count down */
        (
            // Inputs
            .clk                       (clk),
            .resetn                    (resetn),
            .start                     (in_select[sub_index_1] & sub_start),
            .stopcount                 (sub_stopcount),
            .count                     (sub_data[sub_index_1]),
            .status                    (sub_status[sub_index_1])
        );
   //defparam counter_down.count_up = 0;

   // Print some stuff as an example
   initial begin
      if ($test$plusargs("trace") != 0) begin
         $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
         $dumpfile("logs/vlt_dump.vcd");
         $dumpvars();
      end
      $display("[%0t] Model running...\n", $time);
   end

endmodule
