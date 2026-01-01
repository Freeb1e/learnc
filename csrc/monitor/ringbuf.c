#include "../include/ringbuf.h"

char itrace_buf[BUF_NUM][BUF_LENTH];
int head_ptr=0;

void ringbuf_init() {
    head_ptr=0;
    for(int i=0;i<BUF_NUM;i++){
        sprintf(itrace_buf[i],"NULL");
    }
    return;
}

void ringbuf_append(const char *entry) {
    sprintf(itrace_buf[head_ptr], "%s", entry);
    head_ptr = (head_ptr + 1) % BUF_NUM;
    return;
}

void ringbuf_print() {
    int idx;
    for (int i = 0; i < BUF_NUM; i++) {
        idx = (head_ptr + i) % BUF_NUM;
        if(idx%BUF_NUM==(head_ptr-1+BUF_NUM)% BUF_NUM)
            printf("\033[1;31m-->%d %s\033[0m\n", idx,itrace_buf[idx]);
        else
            printf("%d %s\n", idx,itrace_buf[idx]);
    }
    return;
}
