#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/baremetal-setup/baremetal.bin));

struct config config = {

    CONFIG_HEADER

    .vmlist_size = 1,
    .vmlist = {
        { 
            .image = VM_IMAGE_BUILTIN(baremetal_image, 0x80200000),

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x80200000,
                        .size = 0x4000000 
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* 8250 */
                        .pa = 0x10000000,
                        .va = 0x10000000,
                        .size = 0x1000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10}
                    },
                },

                .arch = {
                    .plic_base = 0xc000000,
                }
            },
        }
    },
};