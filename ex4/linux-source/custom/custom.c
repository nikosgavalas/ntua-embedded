#include <linux/kernel.h>

asmlinkage long sys_custom(void) {
    printk(KERN_ALERT "Greeting from kernel and team 2 from shift 1\n");
    return 0;
}
