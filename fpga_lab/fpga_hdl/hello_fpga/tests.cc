#include <gtest/gtest.h>
#include "verilated.h"

// Verilator generated code
#include "obj_dir/Vhello_fpga.h"

TEST(HelloFPGA, TestDemo) {
    // GoogleTest example
    ASSERT_EQ(1, 1);
}

uint8_t ctr_val(const Vhello_fpga& model) {
    // Read the 3 bit ctr value out of model, expose as
    // uint8_t
    return
        model.CTR0 |
        (model.CTR1 << 1) |
        (model.CTR2 << 2);
}

TEST(HelloFPGA, VerilatorDemo) {
    auto ctx = std::make_unique<VerilatedContext>();
    auto top = std::make_unique<Vhello_fpga>(ctx.get());

    ASSERT_EQ(0, ctr_val(*top.get()));
    top->eval();
    ASSERT_EQ(1, top->ALWAYS_PIN);

    int steps = 0;
    // Count number of cycles needed to advance ctr
    while (ctr_val(*top.get()) == 0 && steps < 1<<22) {
        steps ++;
        top->CLK = 0;
        top->eval();
        top->CLK = 1;
        top->eval();

        ASSERT_EQ(1, top->ALWAYS_PIN);
    }

    ASSERT_EQ(1, ctr_val(*top.get()));
    ASSERT_EQ((1<<20)+1, steps);
}
