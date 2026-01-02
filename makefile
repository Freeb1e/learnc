.PHONY: build run RUN clean
VERILOG = $(wildcard vsrc/*.sv)
VERILOG += $(wildcard vsrc/*.v)
CSOURCE=$(shell find csrc -name "*.cpp" -not -path "*/tools/capstone/repo/*")
CSOURCE+=$(shell find csrc -name "*.c" -not -path "*/tools/capstone/repo/*")
CSOURCE+=$(shell find csrc -name "*.cc" -not -path "*/tools/capstone/repo/*")

TOP_NAME ?= npc
CAPSTONE_INCLUDE = csrc/tools/capstone/repo/include
TOOLS_MAKEFILE = csrc/tools.mk


include $(TOOLS_MAKEFILE)
build:
# 	clear
	verilator --trace -cc $(VERILOG) --exe $(CSOURCE) --top-module $(TOP_NAME) -Mdir obj_dir -Ivsrc -LDFLAGS "-lreadline" -CFLAGS "-I$(abspath $(CAPSTONE_INCLUDE)) $(TOOLS_CTRL) -DNPC_HOME=\\\"$(abspath .)\\\""
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