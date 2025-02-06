#include <gtest/gtest.h>
#include "verilated.h"
#include <vector>
#include <random>

// Verilator generated code
#include "obj_dir/Vtinytest.h"

using std::vector;

void clock(Vtinytest* top) {
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
}

TEST(EchoRam, FullRW) {
    puts("echo test started");
    auto ctx = std::make_unique<VerilatedContext>();
    auto top = std::make_unique<Vtinytest>(ctx.get());
    constexpr int kBITS = 8;

    std::mt19937 rng(0);
    auto memory = vector<uint8_t>(1<<kBITS, 0);
    for (int i = 0; i < memory.size(); i++) {
        memory[i] = rng() & 0xff;
    }
    ASSERT_NE(0, memory[0]);

    top->eval();
    ASSERT_EQ(0, top->rw);

    uint32_t addr = 0;
    top->reset = 1;
    top->eval();
    top->reset = 0;
    // Count number of clocks before RW goes high
    while (top->rw == 0) {
        ASSERT_EQ(addr, top->ctr);
        top->data = memory[addr];
        clock(top.get());
        top->eval();
        if (addr == 1<<kBITS && top->rw == 0) {
            puts("wraparound without RW going high!");
        }
        addr++;

    }
    ASSERT_EQ(1<<kBITS, addr);

    // verify data
    addr = 0;
    ASSERT_EQ(top->ctr, 1<<kBITS);

    while (top->rw == 1) {
        ASSERT_EQ(memory[addr], top->data) << "mismatch at " <<  addr;
        addr++;
        clock(top.get());
    }
    ASSERT_EQ(1<<kBITS, addr);

}
