#include "include/memory.h"
#include "include/common.h"
char *img_file_path = nullptr;

uint8_t pmem[MSIZE] = {
  // 0x00500093 (addi x1, x0, 5)
  0x93, 0x00, 0x50, 0x00,

  // 0xffd08113 (addi x2, x1, -3)
  0x13, 0x81, 0xd0, 0xff,

  // 0xff600193 (addi x3, x0, -10)
  0x93, 0x01, 0x60, 0xff,

  // 0x00f18213 (addi x4, x3, 15)
  0x13, 0x82, 0xf1, 0x00,

  // 0x00000293 (addi x5, x0, 0)
  0x93, 0x02, 0x00, 0x00,

  // 0x06428313 (addi x6, x5, 100)
  0x13, 0x83, 0x42, 0x06,

  // 0x7ff00393 (addi x7, x0, 2047)
  0x93, 0x03, 0xf0, 0x7f,

  // 0x80000413 (addi x8, x0, -2048)
  0x13, 0x04, 0x00, 0x80,

  // 0x00148493 (addi x9, x9, 1)
  0x93, 0x84, 0x14, 0x00,


  //ebreak
  0x73, 0x00, 0x10, 0x00
};

uint8_t* addr_vtoc(uint32_t vaddr) {
  //printf("vaddr: 0x%08x\n", vaddr);
  return pmem + (vaddr - MEM_BASE);
}

extern "C" int pmem_read(int raddr) {
  uint32_t rdata=0;
  uint8_t *paddr = addr_vtoc(raddr);
  #ifdef MEM_TRACE
  printf("raddr: 0x%08x, paddr: %p  \n", raddr, paddr);
  #endif
  if (paddr < pmem || paddr + 4 > pmem + MSIZE) {
    #ifdef MEM_TRACE
    printf("pmem_read out of bound raddr: 0x%08x\n", raddr);
    #endif
    return 0x7f7f7f7f;
  }
  for (uint8_t i = 0; i < 4; i++) {
    rdata |= (*(paddr + i)) << (i * 8);
  }
  *pmem = rdata;
  return rdata;
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
    uint8_t *host_ptr = addr_vtoc((uint32_t)waddr);
    #ifdef MEM_TRACE
    printf("waddr: 0x%08x, paddr: %p, wdata: 0x%08x, wmask: 0x%02x\n", waddr, host_ptr, wdata, (unsigned char)wmask);
    #endif
    for (int i = 0; i < 4; i++) {
        if ((wmask >> i) & 0x01) {
            // 4. 写入数据
            host_ptr[i] = (uint8_t)((wdata >> (i * 8)) & 0xff);
        }
    }
}

long load_img(const char *img_file_path, uint8_t *pmem) {
    if (img_file_path == nullptr) {
        return 0; 
    }

    FILE *fp = fopen(img_file_path, "rb");
    if (fp == nullptr) {
        perror("fopen failed");
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET); 

    if (size > MSIZE) {
        std::cerr << "Error: Image size (" << size << ") is too large for memory (" << MSIZE << ")" << std::endl;
        fclose(fp);
        exit(-1);
    }
    size_t ret = fread(pmem, 1, size, fp); 
    if (ret != (size_t)size) {
        perror("fread mismatch");
        fclose(fp);
        exit(-1);
    }
    fclose(fp);
    std::cout << "Successfully loaded image: " << img_file_path << " size: " << size << " bytes" << std::endl;
    
    return size; 
}