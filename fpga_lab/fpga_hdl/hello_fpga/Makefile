# run tests
test: tests
	./tests

PROJ = hello_fpga
# build tests
# assumes googletest installed (tested w/ v1.15.2)
tests: $(PROJ).v tests.cc obj_dir/V$(PROJ).h
	$(CXX) tests.cc -o tests -lgtest_main -lgtest `pkg-config --cflags verilator` obj_dir/libverilated.a obj_dir/libV$(PROJ).a


# Verilator 
obj_dir/V$(PROJ).h: $(PROJ).v
	verilator --cc $(PROJ).v
	make -C obj_dir -f V$(PROJ).mk

# ICESTORM targets for synthesis
PIN_DEF = $(PROJ).pcf
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
