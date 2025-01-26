# run tests
test: tests
	./tests

# build tests
# assumes googletest installed (tested w/ v1.15.2)
tests: hello_fpga.v tests.cc obj_dir/Vhello_fpga.h
	$(CXX) tests.cc -o tests -lgtest_main -lgtest `pkg-config --cflags verilator` obj_dir/libverilated.a obj_dir/libVhello_fpga.a


# Verilator 
obj_dir/Vhello_fpga.h: hello_fpga.v
	verilator --cc hello_fpga.v
	make -C obj_dir -f Vhello_fpga.mk

# ICESTORM targets for synthesis
PROJ = hello_fpga
PIN_DEF = hello_fpga.pcf
DEVICE = lp1k
# DEVICE = hx1k
PACKAGE = cm36

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
