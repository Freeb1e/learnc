#include "stdint.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vnpc.h"
#include "Vnpc__Syms.h"

extern Vnpc *dut;
extern VerilatedVcdC *m_trace;
extern vluint64_t sim_time;
#define MAX_SIM_TIME 20