#ifndef __COMMON_H__   // 1. 检查这里的名字
#define __COMMON_H__   // 2. 这里的名字必须和上面完全一致！


#include "stdint.h"
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vnpc.h"
#include "Vnpc__Syms.h"
#include "Vnpc__Dpi.h"
#include "macro.h"
#include "utils.h"
extern Vnpc *dut;
extern VerilatedVcdC *m_trace;
extern vluint64_t sim_time;
extern bool abort_sign;
extern void stopsim(); 
#define MAX_SIM_TIME 20
#define __GUEST_ISA__ rv32-npc

#define Log(format, ...) \
    _Log(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      MUXDEF(CONFIG_TARGET_AM, printf(ANSI_FMT(format, ANSI_FG_RED) "\n", ## __VA_ARGS__), \
        (fflush(stdout), fprintf(stderr, ANSI_FMT(format, ANSI_FG_RED) "\n", ##  __VA_ARGS__))); \
      IFNDEF(CONFIG_TARGET_AM, extern FILE* log_fp; fflush(log_fp)); \
      extern void assert_fail_msg(); \
      assert_fail_msg(); \
      assert(cond); \
    } \
  } while (0)

#define panic(format, ...) Assert(0, format, ## __VA_ARGS__)

#define TODO() panic("please implement me")

typedef struct {
  uint32_t gpr[32];
  uint32_t pc;
} CPU_state_npc;

extern CPU_state_npc npc_regs;
extern CPU_state_npc nemu_regs;
extern void get_CPU_state_npc(CPU_state_npc *state);

#define CONFIG_ITRACE 1
// #define CONFIG_MTRACE 1


#endif