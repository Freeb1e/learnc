#include "../include/memory.h"
#include "../include/monitor.h"
#include "../include/common.h"

FILE *log_fp = NULL;

void assert_fail_msg() {
}

static void welcome() {
  Log("Trace: %s", MUXDEF(CONFIG_TRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));
  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to %s-NEMU!\n", ANSI_FMT(str(__GUEST_ISA__), ANSI_FG_YELLOW ANSI_BG_RED));
  printf("For help, type \"help\"\n");
}

void sdb_set_batch_mode();

void init_monitor(int argc, char *argv[]) {
  welcome();
}


void am_init_monitor() {
  //load_img();
  welcome();
}
