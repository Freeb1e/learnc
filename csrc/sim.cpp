#include "common.h"
#include "memory.h"
Vnpc *dut = nullptr;
VerilatedVcdC *m_trace = nullptr;
vluint64_t sim_time = 0;

void maininit() {
    dut = new Vnpc;
    Verilated::traceEverOn(true);
    m_trace = new VerilatedVcdC;
    dut->trace(m_trace, 5);
    m_trace->open("waveform.vcd");
}

void stopsim() {
    m_trace->close();
    delete dut;
    exit(EXIT_SUCCESS);
}

void half_cycle() {
    dut->clk ^= 1;
    dut->eval();
    m_trace->dump(sim_time);
    sim_time++;
}

int main(int argc, char** argv, char** env) {
    maininit();
    load_img(img_file_path, pmem);
    using std::cout;
    using std::endl;
    cout<<"Image loaded successfully."<<endl;
    while (sim_time < MAX_SIM_TIME) {
        cout<<"Sim time: "<<sim_time<<endl;
        if(sim_time==0) {
            dut->rst = 1;
        } else if(sim_time==4) {
            dut->rst = 0;
        }
        half_cycle();
    }
    stopsim();
}
