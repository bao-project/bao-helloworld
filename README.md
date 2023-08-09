# "Hello world"? We prefer "Hello Bao!"

Welcome to the Bao Hypervisor! Get ready for an interactive journey as we explore the world of Bao together. Whether you're a seasoned Bao user or a newcomer, this tour is designed to give you a practical and enthusiastic introduction to our powerful hypervisor.

If you're already familiar with Bao or want to dive into specific setups provided by our team, feel free to skip ahead to the Bao demos section.

In this guide, we will take a tour of the different components required to build a setup using the Bao hypervisor and learn how the different components interact. For this purpose, the guide contains the following topics:

- A **getting started** to help users on preparing the environment to build the setup and also some pointers to documentations of Bao (in case you want to go deeper in any detail);

- An **initial setup** for giving the first steps on this tour. This section aims to explore the different components of the system and get the first practical example of this guide;

- An **interactive tutorial on changing the guests** running on top of Bao;

- A **practical example** of changing the setup running;

- An example of **how different guests can coexist and interact** with each other;

## 1. Getting Started

Before we dive into the thrilling aspects of Bao, let's make sure you're all set up and ready to go. In this section, we'll guide you through preparing your environment to build the setup. Don't worry; we'll provide you with helpful pointers to Bao's documentation in case you want to explore any details further.

### 1.1 Recommended Operating System: Linux (e.g., Ubuntu 22.04 or older versions)
To make the most of this tutorial and the Bao hypervisor, we recommend using a Linux-based operating system. While the instructions may work on other platforms, our focus is on Linux, specifically Ubuntu 22.04 or older versions. This will ensure compatibility and an optimal experience throughout the tour.

### 1.2 Installing Required Dependencies
Before we can dive into the world of Bao, we need to install several dependencies to enable a seamless setup process. Open your terminal and run the following command to install the necessary packages:

```sh
sudo apt install build-essential bison flex git libssl-dev ninja-build \
    u-boot-tools pandoc libslirp-dev pkg-config libglib2.0-dev libpixman-1-dev \
    gettext-base curl xterm cmake python3-pip
```

This command will install essential tools and libraries required for building and running Bao.
Next, we need to install some Python packages. Execute the following command to do so:

```sh
pip3 install pykwalify packaging pyelftools
```

### 1.3 Download and setup the toolchain

### Choosing the Right Toolchain

[arm-toolchains]: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
[riscv-toolchains]: https://github.com/sifive/freedom-tools/releases

Before we delve deeper, let's ensure you have the right tools at your disposal. We'll guide you through obtaining and configuring the appropriate cross-compile toolchain for your target architecture. This step is essential for a smooth development experience.

a) Armv8 Aarch64: For this architecture, opt for the ``aarch64-none-elf-`` toolchain. Download it from the [Arm Developer's][arm-toolchains] website.

b) Armv7 or Armv8 Aarch32: In this case, go for the ``arm-none-eabi-`` toolchain. Download it from the [Arm Developer's][arm-toolchains] website.

c) RISC-V: If you're working with RISC-V, select the ``riscv64-unknown-elf-`` toolchain. You can download it from [SiFive's Freedom Tools][riscv-toolchains] GitHub repository.

### Installing and Configuring the Toolchain

Install the toolchain. Then, set the **CROSS_COMPILE** environment variable 
with the reference toolchain prefix path:

```sh
export CROSS_COMPILE=/path/to/toolchain/install/dir/bin/your-toolchain-prefix-
```

### 1.4 Ensuring Sufficient Free Space

Please be aware that sufficient free space is crucial for this journey, especially due to the Linux image that will be built for the Linux guest VM. To ensure a smooth experience and avoid any space-related issues, we recommend having at least 20GB of free space available on your system.
With your environment set up and all the dependencies installed, you're now ready to dive into the world of Bao hypervisor and create your virtualized wonders!

---

## Initial setup - Taking the First Steps!

Now that you're geared up, it's time to take the first steps on this tour. In the Initial Setup section, we'll explore the different components of the system and walk you through a practical example to give you a solid foundation.

To ensure a smooth journey ahead, let's start by creating a development environment. While this step isn't mandatory, it will greatly assist you in navigating the upcoming stages. Remember, this is just one approach to defining your workspace; feel free to explore alternatives that best suit your specific requirements.

We'll begin by establishing a directory structure for the various components of our setups. Open up your terminal and execute the following commands:
```c
export ROOT_DIR=$(realpath .)
export SETUP_BUILD=$ROOT_DIR/bin

export BUILD_GUESTS_DIR=$SETUP_BUILD/guests
export BUILD_BAO_DIR=$SETUP_BUILD/bao
export BUILD_FIRMWARE_DIR=$SETUP_BUILD/firmware

mkdir -p $BUILD_GUESTS_DIR
mkdir -p $BUILD_BAO_DIR
mkdir -p $BUILD_FIRMWARE_DIR
```

Upon completing these commands, your directory should resemble the following:
``` sh
├── bin
│   ├── bao
│   ├── firmware
│   └── guests
├── configs
│   ├──...
├── img
│  ├──...
└──README.md
```

## 1. Build Guest - Building Your First Bao Guest

Let's kickstart your journey by building your inaugural Bao guest! Here, you'll gain hands-on experience crafting a Baremetal Guest. Let's get that virtual machine up and running!

But before we dive into the hands-on excitement, let's understand the setup we're crafting. Our goal is to deploy a bare-metal system atop the Bao hypervisor, as illustrated in the figure below:

![Init Setup](/img/single-guest.svg)

For the sake of simplicity and accessibility, we'll detach from physical hardware and utilize QEMU (don't worry, we'll guide you through its installation later in the tutorial). However, remember that you can apply these steps to various other platforms as well:

|                     |     Platform      |  ARCH   |
| ------------------- | :---------------: | :-----: |
| Xilinx ZCU102       |      zcu102       | aarch64 |
| Xilinx ZCU104       |      zcu104       | aarch64 |
| NXP i.MX8QM         |      imx8qm       | aarch64 |
| Nvidia TX2          |        tx2        | aarch64 |
| Raspberry 4 Model B |       rpi4        | aarch64 |
| QEMU Aarch64 virt   | qemu-aarch64-virt | aarch64 |
| FVP-A AArch64       |       fvp-a       | aarch64 |
| FVP-R AArch64       |       fvp-r       | aarch64 |
| FVP-A AArch32       |   fvp-a-aarch32   | aarch32 |
| FVP-R AArch32       |   fvp-r-aarch32   | aarch32 |
| QEMU RV64 virt      | qemu-riscv64-virt | riscv64 |

Let's start by describing the (virtual) platform, which represents the environment where the guest operates—essentially, what it perceives as its physical hardware. For our setup, we'll define a basic guest platform consisting of a timer, a 64MiB memory region, and a UART for communication. Sounds straightforward, doesn't it? Let's translate this into the Bao configuration file:

### Platform Description

The (virtual) platform comprises three main components:
  
  1. Virtual CPUs (vCPUs), in this case, 4:
```c
.cpu_num = 4,
```

  2. Memory region, which is 64MiB in this instance:
```c
.region_num = 1,
.regions =  (struct vm_mem_region[]) {
    {
        .base = 0x50000000,
        .size = 0x4000000 
    }
},
```
  3. Devices—namely, the timer and the UART:
```c
.dev_num = 2,
.devs =  (struct vm_dev_region[]) {
    {   
        /* PL011 */
        .pa = 0x9000000,
        .va = 0x9000000,
        .size = 0x10000,
        .interrupt_num = 1,
        .interrupts = (irqid_t[]) {33}                        
    },
    {   
        /* Arch timer interrupt */
        .interrupt_num = 1,
        .interrupts = 
            (irqid_t[]) {27}                         
    }
},
```

Now that we're clear on our destination, let's forge ahead and construct our guest. We'll provide you with a simple example to help you along, but remember, this isn't the only path. Feel free to explore various bare-metal applications; after all, you're the developer!

To start, let's define an environment variable for the bare-metal app source code:
```c
export BAREMETAL_SRCS=$ROOT_DIR/baremetal
```

Then, clone the Bao bare-metal guest application we've prepared (you can skip this step if you already have your own bare-metal source):
```c
git clone https://github.com/bao-project/bao-baremetal-guest.git\
    --branch demo $BAREMETAL_SRCS
```

And now, let's compile it (for simplicity, our example includes a Makefile to compile the bare-metal compilation):
```c
make -C $BAREMETAL_SRCS PLATFORM=qemu-aarch64-virt
```

Upon completing these steps, you'll uncover a binary treasure awaiting you in the BAREMETAL_SRCS directory. If you followed our provided Makefile, this precious gem will bear the name ``baremetal.bin``. This binary represents the image of your soon-to-be unleashed bare-metal guest. It's time to transfer this treasure to the designated vault, your build directory (``BUILD_GUESTS_DIR``).

Execute the following commands in your terminal to securely transport your binary to its rightful place:
```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-setup
cp $BAREMETAL_SRCS/build/baremetal.bin $BUILD_GUESTS_DIR/baremetal-setup/baremetal.bin
```

## 2. Build Bao Hypervisor - Laying the Foundation
Next up, we'll guide you through building the Bao Hypervisor itself. This critical step forms the backbone of your virtualization environment.

Our first stride in this journey involves configuring the hypervisor using Bao's configuration file. For this specific setup, we're offering you the [configuration file](configs/baremetal.c)to facilitate the process. If you're curious to explore different configuration options, our detailed our detailed Bao config documentation is [here](https://github.com/bao-project/bao-docs/tree/wip/bao-classic_config) to help.

> [!Warning] If you are using a directory structure of the one presented in the tutorial, please make sure to update the following code in the [configuration file](configs/baremetal.c).
```c
VM_IMAGE(baremetal_image, XSTR(BAO_DEMOS_WRKDIR_IMGS/baremetal.bin));
```

## Build Firmware - Powering Up Your Setup

No tour is complete without a firmware build. We'll show you how to get your setup up and running with our straightforward firmware-building process.


## Let's Try It Out! - Unleash the Power

Now that everything is set up, let's put it to the test! We'll show you how to fire up your newly created virtual machines and experience the magic of Bao firsthand.

## Well, Maybe the Setup Was Not Perfect...

As we continue on this thrilling tour, it's time to explore the art of changing your Bao setup. Mastering the ability to modify your virtual environment opens up endless possibilities. Don't worry if you encounter a few challenges along the way; learning through hands-on experience is the key!

In the following sections, we'll walk you through step-by-step instructions to make various changes to your guests. By the end of this part of the tour, you'll have a deeper understanding of how the different components interact, and you'll be confidently making adjustments to suit your needs.

### Add a second guest

![Init Setup](/img/dual-guest.svg)

## Different scenario? Different setup!

In this section, we'll dive into different scenarios and explore how to set up specific environments with Bao. Flexibility is one of Bao's strengths, and we'll guide you through adapting your setup to suit various needs.

### Baremetals are not enough for your application? Let's try freeRTOS
![Init Setup](/img/dual-guest-rtos.svg)

### It was still not perfect right? Let's try out Linux

![Init Setup](/img/dual-guest-rtos-linux.svg)


## Guests must socialize, right?

![Init Setup](/img/dual-guest-updated.svg)

### Let's Share Content! - Memory Sharing Made Easy
Sharing files between guests is essential for seamless collaboration. We'll demonstrate how to set up file sharing and make it a breeze for your virtual machines.

### Teaching Guests to Be Polite - Networking Etiquette
Just like in the real world, networking etiquette matters in the virtual world too! We'll guide you through setting up networking between guests, enabling smooth communication while maintaining order and politeness.