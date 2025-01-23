#include <gtest/gtest.h>
#include "verilated.h"

// Verilator generated code
#include "obj_dir/Vclk_divider.h"


// Divide by 2 is fixed
TEST(ClkDivider, Div2) {
    auto ctx = std::make_unique<VerilatedContext>();
    auto top = std::make_unique<Vclk_divider>(ctx.get());

    top->reset = 1;
    top->clk = 0;
    top->eval();
    top->reset = 0;
    top->eval();

    auto count_state = [&](bool val) -> int{
        int count = 0;
        int steps = 0;
        while (steps < 1000) {
            top->clk = 0;
            top->eval();
            top->clk = 1;
            top->eval();
            count += 1;
            if (top->divided != val) {
                break;
            }
            steps++;
        }
        if (steps >= 1000) {
            return -1;
        }
        return count;
    };


    // Test for 3 output cycles
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(2, count_state(0));
        ASSERT_EQ(2, count_state(1));
    }

}

// TODO: set parameter and test other divisors
// TEST(ClkDivider, Div4) {
//     // TODO: set parameter
//     ASSERT_EQ(4, 2);
// }
