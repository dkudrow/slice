#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void emmc_dump_block(unsigned char *buf)
{
  int i;
  for (i=0; i<512; i++) {
    if (!(i%32))
      printf("\n");
    printf("%x", buf[i] >> 4);
    printf("%x", buf[i] & 0xF);
  }
  printf("\n");
}

void emmc_read_block(unsigned block, unsigned char *buf)
{
  int fd = open("/dev/sdb", O_RDONLY);
  lseek(fd, block*512, SEEK_SET);
  int r = read(fd, buf, 512);
  if (r != 512) printf("Read failed! Got %d bytes!\n", r);
  close(fd);
}


