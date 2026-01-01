#include "common.h"
#define MSIZE 0x08000000
#define MEM_BASE 0x80000000
extern char *img_file_path;
extern uint8_t pmem[MSIZE];
extern "C" uint32_t pmem_read(uint32_t vaddr);
extern "C" void pmem_write(uint32_t *pmem, uint32_t addr, uint8_t len);
long load_img(const char *img_file_path, uint8_t *pmem);
void addr_vtoc(uint32_t vaddr, uint32_t *paddr);