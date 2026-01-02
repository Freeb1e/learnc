#include "include/cpu-exec.h"
#include "include/common.h"
#include "tools/tools.h"
#include "difftest/difftest.h"
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

CPU_state_npc npc_regs;
CPU_state_npc nemu_regs;
void half_cycle()
{
  dut->clk ^= 1;
  dut->eval();
  m_trace->dump(sim_time);
  sim_time++;
}
static svScope g_scope = NULL;
static svScope g_scope_reg = NULL;

bool isa_difftest_checkregs(CPU_state_npc *ref_r) {
  uint32_t dut_pc = read_pc();
  if (dut_pc != ref_r->pc) {
    printf(ANSI_FMT("PC is different, dut: 0x%08x, ref: 0x%08x\n", ANSI_FG_RED), dut_pc, ref_r->pc);
   // exit(1);
    return false;
  }

  for (int i = 0; i < 32; i++) {
    uint32_t dut_val = read_register(i);
    if (dut_val != ref_r->gpr[i]) {
      printf(ANSI_FMT("reg %s is different, dut: 0x%08x, ref: 0x%08x\n", ANSI_FG_RED), regs[i], dut_val, ref_r->gpr[i]);
      ringbuf_print();
    //  exit(1);
      return false;
    }
  }
  return true;
}


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
  //exit(0);
}


void get_CPU_state_npc(CPU_state_npc *state)
{
  for (int i = 0; i < 32; i++)
  {
    state->gpr[i] = read_register(i);
  }
  state->pc = read_pc();
}

void cpu_exec(uint64_t n)
{
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (npc_state)
  {
  case NPC_END:
  printf("Program execution has ended NORMAL. To restart the program, exit NEMU and run again.\n");break;
  case NPC_QUIT:
  case NPC_ABORT:
    printf("Program execution has ended ABORT. To restart the program, exit NEMU and run again.\n");
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
    half_cycle();half_cycle();
    half_cycle();half_cycle();
    half_cycle();half_cycle();
    half_cycle();half_cycle();
    half_cycle();half_cycle();
    ref_difftest_exec(1);
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


void dump_nemu_mem(uint64_t start_addr) {
    // 1. 准备缓冲区
    uint8_t buf[256];

    // 2. 检查函数指针是否已加载
    assert(ref_difftest_memcpy != NULL && "Error: ref_difftest_memcpy 未初始化，请先调用 init_difftest");

    // 3. 调用接口读取内存
    // DIFFTEST_TO_DUT: 意思是 "从 Ref (NEMU) 拷给 DUT (NPC)" -> 即读取 NEMU
    ref_difftest_memcpy(start_addr, buf, 256, DIFFTEST_TO_DUT);

    // 4. 格式化打印 (Hex Dump 风格)
    printf("\n=== NEMU Memory Dump (Start: 0x%lx) ===\n", start_addr);
    printf("        00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
    
    for (int i = 0; i < 256; i++) {
        // 每 16 字节换行，并打印当前地址偏移
        if (i % 16 == 0) {
            printf("\n%08lx: ", start_addr + i);
        }
        // 打印字节内容
        printf("%02x ", buf[i]);
    }
    printf("\n==========================================\n\n");
}

void print_reg_CPU_state(CPU_state_npc *state)
{
  printf("----------------- Registers -----------------\n");
  // 1. 遍历 32 个通用寄存器
  for (int i = 0; i < 32; i++)
  {
    uint32_t val = state->gpr[i];
    printf("x%02d  %-4s 0x%08x   %d\n", i, regs[i], val, val);
  }
  printf("%-4s 0x%08x   %d\n", "pc", state->pc, state->pc);
  printf("---------------------------------------------\n");
}

static void trace_and_difftest(char *logbuf, uint32_t dnpc) {
    if (g_print_step) {
        IFDEF(CONFIG_ITRACE, puts(logbuf));
      }
        ref_difftest_regcpy(&nemu_regs, DIFFTEST_TO_DUT);
        if (!isa_difftest_checkregs(&nemu_regs)) {
            printf(ANSI_FMT("Difftest failed at pc = 0x%08x\n", ANSI_FG_RED), dnpc);
            printf("Instruction: %s\n", logbuf);
            npc_state = NPC_ABORT;
            abort_sign = true;
        //print_reg_CPU_state(&nemu_regs);
        //dump_nemu_mem(0x80000000);
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
