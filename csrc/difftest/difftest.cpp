#include "difftest.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h> // 必须包含这个用于 dlopen, dlsym
#include "../include/memory.h"
#include "../include/common.h"

// 定义全局函数指针变量
difftest_memcpy_t ref_difftest_memcpy = NULL;
difftest_regcpy_t ref_difftest_regcpy = NULL;
difftest_exec_t   ref_difftest_exec   = NULL;
difftest_raise_intr_t ref_difftest_raise_intr = NULL;
difftest_init_t   ref_difftest_init   = NULL;

void init_difftest(const char *ref_so_file, long img_size, int port) {
    assert(ref_so_file != NULL);

    // 1. 打开动态链接库
    // RTLD_LAZY: 暂时不解析未定义的符号，等到用的时候再解析
    void *handle = dlopen(ref_so_file, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        assert(0 && "无法加载 NEMU 动态库，请检查路径是否正确");
    }

    // 2. 映射符号 (Symbol Binding)
    // dlsym 返回 void*，需要强制转换为对应的函数指针类型
    ref_difftest_memcpy = (difftest_memcpy_t)dlsym(handle, "difftest_memcpy");
    ref_difftest_regcpy = (difftest_regcpy_t)dlsym(handle, "difftest_regcpy");
    ref_difftest_exec   = (difftest_exec_t)  dlsym(handle, "difftest_exec");
    ref_difftest_raise_intr = (difftest_raise_intr_t)dlsym(handle, "difftest_raise_intr");
    ref_difftest_init   = (difftest_init_t)  dlsym(handle, "difftest_init");

    // 3. 检查所有接口是否都找到了
    assert(ref_difftest_memcpy);
    assert(ref_difftest_regcpy);
    assert(ref_difftest_exec);
    assert(ref_difftest_raise_intr);
    assert(ref_difftest_init);

    // 4. 调用 NEMU 的初始化
    printf("DiffTest: Initializing Ref: %s\n", ref_so_file);
    ref_difftest_init(port);
    
    // 5. 将 NPC 的内存初始化状态同步给 NEMU

     ref_difftest_memcpy(0x80000000, pmem, img_size, DIFFTEST_TO_REF);
    
    // 6. 将 NPC 的寄存器初始状态同步给 NEMU
     get_CPU_state_npc(&npc_regs);
     ref_difftest_regcpy(&npc_regs, DIFFTEST_TO_REF);
}