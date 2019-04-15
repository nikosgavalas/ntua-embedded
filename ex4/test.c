#include <linux/kernel.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define SYSCALL_NR 378

int main() {
    int ret = syscall(SYSCALL_NR);
    printf("Ret: %d\n", ret);

    return 0;
}
