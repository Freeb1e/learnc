#include <locale.h>
#include "ringbuf.h"
#include "common.h"
#define MAX_INST_TO_PRINT 10
enum { NPC_RUNNING , NPC_STOP , NPC_ABORT , NPC_QUIT, NPC_END};
extern uint8_t npc_state;
static void excute(uint64_t n);
extern uint32_t read_register(int addr);
extern uint32_t read_pc();
extern void isa_reg_display();
extern const char *regs[];
void cpu_exec(uint64_t n);
extern void init_dpi_scope();
extern void half_cycle();

extern void print_reg_CPU_state(CPU_state_npc *state);

#define PER_INSTR_LOG_SIZE 128