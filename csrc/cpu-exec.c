#include "include/cpu-exec.h"
#include "include/common.h"
#include "tools/tools.h"
uint8_t npc_state = NPC_STOP;
const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
char instr_logbuf[PER_INSTR_LOG_SIZE];
bool g_print_step = false;
static void excute_once(uint64_t n);
static void trace_and_difftest(char *logbuf, uint32_t dnpc);


void half_cycle()
{
  dut->clk ^= 1;
  dut->eval();
  m_trace->dump(sim_time);
  sim_time++;
}
static svScope g_scope = NULL;
static svScope g_scope_reg = NULL;

void init_dpi_scope()
{
  g_scope = svGetScopeFromName("TOP.npc");
  if (!g_scope)
  {
    g_scope = svGetScopeFromName("npc");
  }
  if (!g_scope)
  {
    printf("Error: Verify DPI Scope failed! Neither 'TOP.npc' nor 'npc' found.\n");
    // assert(0);
  }

  g_scope_reg = svGetScopeFromName("TOP.npc.datapath1.u_RegisterFile");
  if (!g_scope_reg)
  {
    g_scope_reg = svGetScopeFromName("npc.datapath1.u_RegisterFile");
  }
  if (!g_scope_reg)
  {
    printf("Error: Verify DPI Scope failed! Neither 'TOP.npc.datapath1.u_RegisterFile' nor 'npc.datapath1.u_RegisterFile' found.\n");
  }
}

extern "C"
{
  void get_reg(int addr, int *reg_data);
  void get_pc_inst(int *pc, int *inst);
}
uint32_t read_register(int addr)
{
  int data = 0;
  svSetScope(g_scope_reg);
  get_reg(addr, &data);
  return (uint32_t)data;
}
uint32_t read_pc()
{
  int pc = 0;
  int inst = 0;
  svSetScope(g_scope);
  get_pc_inst(&pc, &inst);
  return (uint32_t)pc;
}
extern "C" void ebreak()
{
  npc_state = NPC_END;
  printf("\33[1;32mHit ebreak: Simulation requested stop!\33[0m\n");
  exit(0);
}

void cpu_exec(uint64_t n)
{
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (npc_state)
  {
  case NPC_END:
  case NPC_QUIT:
  case NPC_ABORT:
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
    return;
  default:
    npc_state = NPC_RUNNING;
  }

  excute_once(n);

  switch (npc_state)
  {
  case NPC_RUNNING:
    npc_state = NPC_STOP;
    break;
  case NPC_QUIT:
    printf("NPC_QUIT\n");
    break;
  case NPC_ABORT:
    printf("NPC_ABORT\n");
    break;
  case NPC_END:
    //printf("NPC_END\n");
    break;
  default:
    break;
  }
}

static void excute_once(uint64_t n)
{
  int pc, inst;

  for (; n > 0; n--)
  {
    half_cycle();
    half_cycle();

    svSetScope(g_scope);
    get_pc_inst(&pc, &inst);
    
#ifdef CONFIG_ITRACE
    char *p = instr_logbuf;
    p += snprintf(p, sizeof(instr_logbuf), "0x%08x:", pc);
    int ilen = 4;
    int i;
    uint8_t *inst_ptr = (uint8_t *)&inst;
    for (i = ilen - 1; i >= 0; i--)
    {
      p += snprintf(p, 4, " %02x", inst_ptr[i]);
    }
    int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
    int space_len = ilen_max - ilen;
    if (space_len < 0)
      space_len = 0;
    space_len = space_len * 3 + 1;
    memset(p, ' ', space_len);
    p += space_len;
    if(inst != 0){
      void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
      disassemble(p, instr_logbuf + sizeof(instr_logbuf) - p, pc , (uint8_t *) &inst , ilen);
    }   
    ringbuf_append(instr_logbuf);
    if (npc_state != NPC_RUNNING) break;
    trace_and_difftest(instr_logbuf, pc);
#endif    
    
  }
}

static void trace_and_difftest(char *logbuf, uint32_t dnpc) {
    if (g_print_step) {
        IFDEF(CONFIG_ITRACE, puts(logbuf));
    }
}

void isa_reg_display()
{
  printf("----------------- Registers -----------------\n");
  // 1. 遍历 32 个通用寄存器
  for (int i = 0; i < 32; i++)
  {
    // 调用 DPI 封装函数获取值
    uint32_t val = read_register(i);
    printf("x%02d  %-4s 0x%08x   %d\n", i, regs[i], val, val);
  }
  // int pc_val, inst_val;
  // get_pc_inst(&pc_val, &inst_val);
  // printf("%-4s 0x%08x   %d\n", "pc", pc_val, pc_val);
  printf("---------------------------------------------\n");
}
