PROJ = driver6502

# run tests
TESTS = test_clk_divider
test: test_clk_divider
	./test_clk_divider
# TODO: generalize
# test: $(TESTS)
# 	for t in $(TESTS); do \
# 		./$$t \
# 	done

# build tests
# assumes googletest installed (tested w/ v1.15.2)
# TODO: refactor for general testing
# test_%: test_%.cc obj_dir/V%.h
# 	$(CXX) %< -o tests -lgtest_main -lgtest `pkg-config --cflags verilator` obj_dir/libverilated.a obj_dir/libV%.a

test_clk_divider: test_clk_divider.cc obj_dir/Vclk_divider.h
	$(CXX) $< -o $@ -lgtest_main -lgtest `pkg-config --cflags verilator` obj_dir/libverilated.a obj_dir/libVclk_divider.a


# Verilator 
# TODO: generalize
# obj_dir/V%.h: %.v
# 	verilator --cc $<
# 	make -C obj_dir -f V%.mk
obj_dir/Vclk_divider.h: clk_divider.v
	verilator --cc $<
	make -C obj_dir -f Vclk_divider.mk

# ICESTORM targets for synthesis
PIN_DEF = 6502_driver.pcf
DEVICE = up5k
# DEVICE = hx1k
PACKAGE = sg48

all: $(PROJ).rpt $(PROJ).bin

%.json: %.v
	yosys -p 'synth_ice40 -top top -json $@' $<

%.asc: $(PIN_DEF) %.json
	nextpnr-ice40 --$(DEVICE) --package $(PACKAGE) --asc $@ --pcf $< --json $*.json

%.bin: %.asc
	icepack $< $@

%.rpt: %.asc
	icetime -d $(DEVICE) -mtr $@ $<

prog: $(PROJ).bin
	iCEburn.py  -e -v -w  $<

sudo-prog: $(PROJ).bin
	@echo 'Executing prog as root!!!'
	iCEburn.py  -e -v -w  $<

clean:
	rm -f $(PROJ).json $(PROJ).asc $(PROJ).rpt $(PROJ).bin

.PHONY: all prog clean test verilate
