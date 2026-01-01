/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <readline/readline.h>
#include <readline/history.h>
#include "./common.h"
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets()
{
  static char *line_read = NULL;

  if (line_read)
  {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read)
  {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args)
{
//   cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args)
{
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
    printf("si command received\n");
//   int n;
//   if (args == NULL)
//     n = 1;
//   else
//     n = atoi(args);
//   cpu_exec(n);
  return 0;
}

static int cmd_info(char *args)
{
//   if (args == NULL)
//   {
//     //printf("Please specify 'r' for registers or 'w' for watchpoints\n");
//     printf("pc\t0x%08x\n", cpu.pc);
//   }
//   if (strcmp(args, "r") == 0)
//   {
//     isa_reg_display();
//   }
//   else if (strcmp(args, "w") == 0)
//   {
//     // wp_display();
//     TODO();
//   }
//   else
//   {
//     printf("Unknown argument '%s'\n", args);
//   }
  return 0;
}

static int cmd_ring(char *args)
{
    // extern void ringbuf_print();
    // ringbuf_print();
    return 0;
}

static int cmd_x(char *args)
{
//   if (args == NULL)
//   {
//     printf("Usage: x N EXPR\n");
//     return 0;
//   }
//   char *arg1 = strtok(args, " ");
//   char *arg2 = strtok(NULL, " ");
//   printf("arg1=%s arg2=%s\n", arg1, arg2);
//   int N = atoi(arg1);
//   vaddr_t addr = (vaddr_t)strtoull(arg2, NULL, 0);
//   while (N--)
//   {
//     word_t data = vaddr_read(addr, 4);
//     printf("0x%lx\n", (unsigned long)data);
//     addr += 4;
//   }
  return 0;
}
void sdb_set_batch_mode();
static int cmd_b(char *args)
{
  sdb_set_batch_mode();
  return 0;
}
static struct
{
  const char *name;
  const char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display information about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Step N instructions exactly", cmd_si},
    {"info", "Display register state or watchpoint information", cmd_info},
    {"x", "Examine memory", cmd_x},
    {"p", "Evaluate expression", NULL},
    {"w", "Set a watchpoint", NULL},
    {"d", "Delete a watchpoint", NULL},
    {"b","enable batch mode",cmd_b},
    {"ring", "Print the instruction ring buffer", cmd_ring}
    /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args)
{
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL)
  {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++)
    {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else
  {
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(arg, cmd_table[i].name) == 0)
      {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode()
{
  return;
  is_batch_mode = true;
}

void sdb_mainloop()
{
  if (is_batch_mode)
  {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;)
  {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL)
    {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end)
    {
      args = NULL;
    }


    int i;
    for (i = 0; i < NR_CMD; i++)
    {
      if (strcmp(cmd, cmd_table[i].name) == 0)
      {
        if (cmd_table[i].handler(args) < 0)
        {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD)
    {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

