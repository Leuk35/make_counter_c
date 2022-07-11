// DESCRIPTION: Verilator: Verilog example main
// 
// This file ONLY for example
//======================================================================

// For std::unique_ptr
#include <memory>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }

int main(int argc, char** argv, char** env) {
    // This is a more complicated example, please also see the simpler examples/make_hello_c.

    // Prevent unused variable warnings
    if (false && argc && argv && env) {}

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");

    // Construct a VerilatedContext to hold simulation time, etc.
    // Multiple modules (made later below with Vtop) may share the same
    // context to share time, or modules may have different contexts if
    // they should be independent from each other.

    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    // Do not instead make Vtop as a file-scope static variable, as the
    // "C++ static initialization order fiasco" may cause a crash

    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    // Verilator must compute traced signals
    contextp->traceEverOn(true);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
    // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

    // Set Vtop's input signals
    top->resetn = !0;
    top->clk = 0;
    top->in_start = 0;
    top->in_select = 0;
    top->in_data = 0;
    

    // Simulate until $finish
    while (!contextp->gotFinish()) {
        // Historical note, before Verilator 4.200 Verilated::gotFinish()
        // was used above in place of contextp->gotFinish().
        // Most of the contextp-> calls can use Verilated:: calls instead;
        // the Verilated:: versions just assume there's a single context
        // being used (per thread).  It's faster and clearer to use the
        // newer contextp-> versions.

        contextp->timeInc(1);  // 1 timeprecision period passes...
        // Historical note, before Verilator 4.200 a sc_time_stamp()
        // function was required instead of using timeInc.  Once timeInc()
        // is called (with non-zero), the Verilated libraries assume the
        // new API, and sc_time_stamp() will no longer work.

        // Toggle a fast (time/2 period) clock
        top->clk = !top->clk;

        // Toggle control signals on an edge that doesn't correspond
        // to where the controls are sampled; in this example we do
        // this only on a negedge of clk, because we know
        // reset is not sampled there.
        if (!top->clk) {
            if (contextp->time() > 1 && contextp->time() < 10) {
                top->resetn = !1;  // Assert reset
            } else {
                top->resetn = !0;  // Deassert reset
            }
        }
        
        // Select the sub_component counter_up
        if(contextp->time()>10 && contextp->time() <= 30)
        {
            // Assign some other inputs
            top->in_select = 0x1;
        }
        // Select the sub_component counter_down 
        if(contextp->time()>30 && contextp->time() <= 40)
        {
            // Assign some other inputs
            top->in_select = 0x2;
        }
        // Select the sub_component counter_up until the end
        if(contextp->time()>40)
        {
            // Assign some other inputs
            top->in_select = 0x1;
        }
        
        // Start to count 
        if(contextp->time()>15)
        {
            // Assign some other inputs
            top->in_data = 0x00000025; /* value that stop counting */
            top->in_start = 0x1;       /* start to count */
        }
        
        // Evaluate model
        // (If you have multiple models being simulated in the same
        // timestep then instead of eval(), call eval_step() on each, then
        // eval_end_step() on each. See the manual.)
        top->eval();

        // Stop if count is done
        if (contextp->time() > 40 && top->out_status) {
            // Assign some other inputs
            top->in_start = 0x0;
        }
        
        
        // Read outputs
        VL_PRINTF("[%" PRId64 "] clk=%x rstn=%x start=%x select=%x data_in=%08x" PRIx64 " -> data_status=%x" PRIx64
                                    " data_out=%08x\n",
                  contextp->time(), top->clk, top->resetn, top->in_start, top->in_select,
                  top->in_data, top->out_status, top->out_data);
    }

    // Final model cleanup
    top->final();

    // Coverage analysis (calling write only after the test is known to pass)
#if VM_COVERAGE
    Verilated::mkdir("logs");
    contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Return good completion status
    // Don't use exit() or destructor won't get called
    return 0;
}
