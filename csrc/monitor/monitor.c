#include "../include/memory.h"
#include "../include/monitor.h"
#include "../include/common.h"

FILE *log_fp = NULL;

void assert_fail_msg() {
}

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  IFDEF(CONFIG_TRACE, Log("If trace is enabled, a log file will be generated "
        "to record the trace. This may lead to a large log file. "
        "If it is not necessary, you can disable it in menuconfig"));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
  //Log("Exercise: Please remove me in the source code and compile NEMU again.");
  //assert(0);
}

void sdb_set_batch_mode();

void init_monitor(int argc, char *argv[]) {
 // long img_size = load_img();

  //init_sdb();

  //IFDEF(CONFIG_ITRACE, init_disasm());

  welcome();
}


void am_init_monitor() {
  //load_img();
  welcome();
}
