.PHONY: build run RUN clean
VERILOG = $(wildcard vsrc/*.sv)
VERILOG += $(wildcard vsrc/*.v)
CSOURCE=$(shell find csrc -name "*.cpp")
CSOURCE+=$(shell find csrc -name "*.c")
TOP_NAME ?= npc

build:
# 	clear
	verilator --trace -cc $(VERILOG) --exe $(CSOURCE) -LDFLAGS "-lreadline" --top-module $(TOP_NAME) -Mdir obj_dir -Ivsrc
	$(MAKE) -C obj_dir -f V$(TOP_NAME).mk V$(TOP_NAME)

run: build
	./obj_dir/V$(TOP_NAME)
test: buildtest
	./obj_dir/V$(TOP_NAME)
RUN: run

see:
	gtkwave waveform.vcd

clean:
	rm -rf obj_dir waveform.vcd