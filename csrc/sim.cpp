#include "include/common.h"
#include "include/memory.h"
#include "include/monitor.h"
#include "include/cpu-exec.h"
#include "tools/tools.h"
#include "include/ringbuf.h"
#include "difftest/difftest.h"
Vnpc *dut = nullptr;
VerilatedVcdC *m_trace = nullptr;
vluint64_t sim_time = 0;

#ifndef NPC_HOME
#define NPC_HOME "."
#endif
bool abort_sign = false;
void waveinit() {
    dut = new Vnpc;
    Verilated::traceEverOn(true);
    m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open(NPC_HOME "/waveform.vcd");
    dut->rst = 1;
    half_cycle();
    half_cycle();
    half_cycle();
    dut->rst = 0;
}

void stopsim() {
    m_trace->close();
    delete dut;
    if(abort_sign) exit(EXIT_FAILURE);
    else exit(EXIT_SUCCESS);
}


int main(int argc, char** argv, char** env) {

  
  if (argc < 2) {
    printf( ANSI_FG_YELLOW ANSI_BG_RED "Using default IMG." ANSI_NONE "\n");
    printf("Usage: %s [image-file] to load external image\n", argv[0]);
    //return -1; // 退出程序
  }

  // 2. 直接赋值
  // "最简单实现"的核心：直接把 argv[1] 当作文件路径
  char *img_file_path = argv[1];
    //初始化波形记录，并完成CPU复位
    waveinit();
    //设置DPI-C接口上下文
    init_dpi_scope();
    //初始化反汇编器
#ifdef CONFIG_ITRACE
    init_disasm();
    ringbuf_init();
#endif
    abort_sign = false;
    //加载镜像文件到内存
    load_img(img_file_path, pmem);
    
    init_difftest(NPC_HOME "/csrc/difftest/build/riscv32-nemu-interpreter-so", MSIZE, 0);

    init_monitor(argc, argv);

    sdb_mainloop();

    stopsim();
}
