#include <locale.h>
#include <stdio.h>
#define BUF_NUM 10
#define BUF_LENTH 128
extern char itrace_buf[BUF_NUM][BUF_LENTH];
extern int head_ptr;

void ringbuf_init();
void ringbuf_append(const char *entry);
void ringbuf_print();