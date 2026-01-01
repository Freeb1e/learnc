#include "include/common.h"
#include "include/memory.h"
#include "include/monitor.h"
#include "include/cpu-exec.h"
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
    waveinit();
    init_dpi_scope();
    load_img(img_file_path, pmem);
    using std::cout;
    using std::endl;
    cout<<"Image loaded successfully."<<endl;
    init_monitor(argc, argv);
    sdb_mainloop();
    // while (sim_time < MAX_SIM_TIME) {
    //     cout<<"Sim time: "<<sim_time<<endl;
    //     if(sim_time==0) {
    //         dut->rst = 1;
    //     } else if(sim_time==4) {
    //         dut->rst = 0;
    //     }
    //     half_cycle();
    // }
    stopsim();
}
