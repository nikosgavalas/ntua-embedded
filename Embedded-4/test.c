#include <stdio.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

#define SYSCALL_NR 378

int main()
{
	int ret = syscall(SYSCALL_NR);
	printf("Ret: %d\n", ret);

	return 0;
}
