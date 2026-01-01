#include "common.h"
#define MSIZE 0x08000000
#define MEM_BASE 0x80000000
extern char *img_file_path;
extern uint8_t pmem[MSIZE];
long load_img(const char *img_file_path, uint8_t *pmem);
void addr_vtoc(uint32_t vaddr, uint32_t *paddr);