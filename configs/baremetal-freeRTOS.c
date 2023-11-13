#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/baremetal-freeRTOS-setup/baremetal.bin));
VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/baremetal-freeRTOS-setup/freertos.bin));

struct config config = {

    CONFIG_HEADER

    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },

    .vmlist_size = 2,
    .vmlist = {
        {
            .image = VM_IMAGE_BUILTIN(baremetal_image, 0x80200000),

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 3,

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
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {10}
                    }
                },

                .arch = {
                    .plic_base = 0xc000000,
                }
            },
        },
        { 
            .image = VM_IMAGE_BUILTIN(freertos_image, 0x0),

            .entry = 0x0,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x0,
                        .size = 0x8000000 
                    }
                },

                .ipc_num = 1,
                .ipcs = (struct ipc[]) {
                    {
                        .base = 0x70000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* 8250 */
                        .pa = 0x10000000,
                        .va = 0xff000000,
                        .size = 0x10000
                    }
               },

                .arch = {
                    .plic_base = 0xc000000,
                }
            },
        },
    },
};