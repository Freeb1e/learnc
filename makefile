.PHONY: build run build1 run1 clean

# ================= 配置与变量 =================

# 1. 尝试获取 CPU 核心数 (兼容性处理：如果 nproc 不存在则默认 1)
JOBS = $(shell nproc 2>/dev/null || echo 1)

VERILOG = $(wildcard vsrc/*.sv)
VERILOG += $(wildcard vsrc/*.v)
CSOURCE = $(shell find csrc -name "*.cpp" -not -path "*/tools/capstone/repo/*")
CSOURCE += $(shell find csrc -name "*.c" -not -path "*/tools/capstone/repo/*")
CSOURCE += $(shell find csrc -name "*.cc" -not -path "*/tools/capstone/repo/*")

TOP_NAME ?= npc
CAPSTONE_INCLUDE = csrc/tools/capstone/repo/include
TOOLS_MAKEFILE = csrc/tools.mk

include $(TOOLS_MAKEFILE)

# 2. 提取公共的 Verilator 参数 (避免代码重复)
#    注意：这里不包含 --output-split，因为这属于优化参数
VFLAGS_COMMON = --trace -cc $(VERILOG) \
                --exe $(CSOURCE) \
                --top-module $(TOP_NAME) \
                -Mdir obj_dir \
                -Ivsrc \
                -LDFLAGS "-lreadline -ldl" \
                -CFLAGS "-I$(abspath $(CAPSTONE_INCLUDE)) $(TOOLS_CTRL) -DNPC_HOME=\\\"$(abspath .)\\\""

# 3. 定义高性能优化的 Verilator 参数
VFLAGS_FAST = --output-split 5000 --output-split-cfuncs 5000

# ================= 原始构建目标 (兼容模式) =================

# 保持原样：单线程编译，无拆分
build:
	verilator $(VFLAGS_COMMON)
	$(MAKE) -C obj_dir -f V$(TOP_NAME).mk V$(TOP_NAME)

run: build
	./obj_dir/V$(TOP_NAME)

# ================= 高速构建目标 (多核模式) =================

# 新增 build1：开启代码拆分 + 多核编译
build1:
	verilator $(VFLAGS_COMMON) $(VFLAGS_FAST)
	$(MAKE) -j$(JOBS) -C obj_dir -f V$(TOP_NAME).mk V$(TOP_NAME)

# 新增 run1：依赖 build1
run1: build1
	./obj_dir/V$(TOP_NAME)

# ================= 其他工具 =================

test: buildtest
	./obj_dir/V$(TOP_NAME)

see:
	gtkwave waveform.vcd

clean:
	rm -rf obj_dir waveform.vcd