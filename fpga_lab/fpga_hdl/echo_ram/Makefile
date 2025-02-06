# ICESTORM targets for synthesis
PROJ = echo_ram
TOP = tinytest
PIN_DEF = echo_ram.pcf
DEVICE = up5k
# DEVICE = hx1k
PACKAGE = sg48

all: $(PROJ).rpt $(PROJ).bin test

test: tests
	./tests

tests: $(PROJ).sv tests.cc obj_dir/V$(TOP).h
	$(CXX) tests.cc -o tests -lgtest_main -lgtest `pkg-config --cflags verilator` obj_dir/libverilated.a obj_dir/libV$(TOP).a

# Verilator 
obj_dir/V$(TOP).h: $(PROJ).sv
	verilator --top $(TOP) --cc $(PROJ).sv 
	make -C obj_dir -f V$(TOP).mk

%.json: %.sv
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

.PHONY: all prog clean test
