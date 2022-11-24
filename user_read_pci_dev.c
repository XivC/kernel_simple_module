#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER 1024

int main(int argc, char *argv[]){


    char in[BUFFER] = {0};
    char out[BUFFER] = {0};
    int fd = open("/proc/lab_read_pci_dev", O_RDWR);
    sprintf(in, "%s %s", argv[1], argv[2]);

    write(fd, in, BUFFER);
    lseek(fd, 0, SEEK_SET);
    read(fd, out, BUFFER);


    puts(out);
    return 0;
}
