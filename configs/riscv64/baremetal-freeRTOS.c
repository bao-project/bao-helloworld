#include <config.h>

VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/baremetal.bin))
VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/free-rtos.bin))

struct config config = {
    
    CONFIG_HEADER
    
    .shmemlist_size = 1,
    .shmemlist = (struct shmem[]) {
        [0] = { .size = 0x00010000, }
    },
    
    .vmlist_size = 2,
    .vmlist = (struct vm_config[]) {
        { 
            .image = {
                .base_addr = 0x80200000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image),
                .size = VM_IMAGE_SIZE(baremetal_image)
            },

            .entry = 0x80200000,

            .platform = {
                .cpu_num = 2,
                
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
                        /* 8250 UART */
                        .pa = 0x10000000,
                        .va = 0x10000000,
                        .size = 0x1000,
                    }
                },

                .arch = {
                    .irqc = {
                        .plic = {
                            .base = 0xc000000,
                        }
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = 0x0,
                .load_addr = VM_IMAGE_OFFSET(freertos_image),
                .size = VM_IMAGE_SIZE(freertos_image)
            },

            .entry = 0x0,

            .platform = {
                .cpu_num = 2,
                
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
                        .base = 0xD0000000,
                        .size = 0x00010000,
                        .shmem_id = 0,
                        .interrupt_num = 1,
                        .interrupts = (irqid_t[]) {52}
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* 8250 UART */
                        .pa = 0x10000000,
                        .va = 0xff000000,
                        .size = 0x1000
                    }
                },

                .arch = {
                    .irqc = {
                        .plic = {
                            .base = 0xc000000,
                        }
                    }
                }
            },
        },
    },
};
