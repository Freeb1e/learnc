#include "include/common.h"
#include "include/memory.h"
#include "include/monitor.h"
#include "include/cpu-exec.h"
#include "tools/tools.h"
#include "include/ringbuf.h"
Vnpc *dut = nullptr;
VerilatedVcdC *m_trace = nullptr;
vluint64_t sim_time = 0;

void waveinit() {
    dut = new Vnpc;
    Verilated::traceEverOn(true);
    m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");
    dut->rst = 1;
    half_cycle();
    half_cycle();
    half_cycle();
    dut->rst = 0;
}

void stopsim() {
    m_trace->close();
    delete dut;
    exit(EXIT_SUCCESS);
}


int main(int argc, char** argv, char** env) {
    //初始化波形记录，并完成CPU复位
    waveinit();
    //设置DPI-C接口上下文
    init_dpi_scope();
    //初始化反汇编器
#ifdef CONFIG_ITRACE
    init_disasm();
    ringbuf_init();
#endif
    //加载镜像文件到内存
    load_img(img_file_path, pmem);

    init_monitor(argc, argv);

    sdb_mainloop();

    stopsim();
}
