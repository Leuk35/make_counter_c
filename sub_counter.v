// DESCRIPTION: Verilator: Verilog counter example sub-module
//
// This file ONLY for example
// ======================================================================

module sub_counter #(parameter count_up = 1)
  (
   input clk,
   input resetn,
   input start,
   input wire [31:0] stopcount,
   output wire status,
   output reg [31:0] count
   );
   if(count_up)
     assign status = start & (count >= stopcount ? '1 : '0);
   else
     assign status = start & ((count > 0 && count <= stopcount )? '1 : '0);
   
   // Example counter/flop
   always_ff @ (posedge clk) begin
      if (!resetn)
         begin
           /*reset when resetn is down*/
           count <= 32'h0;
         end
      else
         begin
         if(start)
           if(count_up)
             /* count up when start is enable*/
             count <= count + 1;
           else
             /* count up when start is enable*/
             count <= count - 1;
         end
      
      /* status message */
      if(status) 
         begin
            $write("*-* All Finished *-*\n");
            $finish;
         end
   end

   // And example coverage analysis
   cover property (@(posedge clk) count==stopcount);
               

endmodule
