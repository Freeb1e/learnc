/**
 * difftest.h
 * 用于 NPC 调用 NEMU 动态库的接口声明
 */

#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>

// 拷贝方向常量 (必须与 NEMU 定义一致)
// DIFFTEST_TO_DUT: 把 NEMU 的状态拷给 NPC (检查用)
// DIFFTEST_TO_REF: 把 NPC 的状态拷给 NEMU (同步用)
enum { DIFFTEST_TO_DUT, DIFFTEST_TO_REF };

// ==========================================================
// 1. 定义与 NEMU 对应的函数指针类型
// ==========================================================

// 对应: void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction);
typedef void (*difftest_memcpy_t)(uint64_t addr, void *buf, size_t n, bool direction);

// 对应: void difftest_regcpy(void *dut, bool direction);
typedef void (*difftest_regcpy_t)(void *dut, bool direction);

// 对应: void difftest_exec(uint64_t n);
typedef void (*difftest_exec_t)(uint64_t n);

// 对应: void difftest_raise_intr(uint64_t NO);
typedef void (*difftest_raise_intr_t)(uint64_t NO);

// 对应: void difftest_init(int port);
typedef void (*difftest_init_t)(int port);


// ==========================================================
// 2. 声明全局函数指针 (在 difftest.cpp 中定义)
// ==========================================================
extern difftest_memcpy_t ref_difftest_memcpy;
extern difftest_regcpy_t ref_difftest_regcpy;
extern difftest_exec_t   ref_difftest_exec;
extern difftest_raise_intr_t ref_difftest_raise_intr;
extern difftest_init_t   ref_difftest_init;


// ==========================================================
// 3. 辅助函数声明
// ==========================================================

/**
 * 初始化 DiffTest
 * @param ref_so_file NEMU 动态库(.so)的路径
 * @param img_size    bin 文件的大小
 * @param port        DiffTest 端口 (通常设为 1234 或 0 即可)
 */
void init_difftest(const char *ref_so_file, long img_size, int port);

#endif // __DIFFTEST_H__