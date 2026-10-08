# "Hello world"? We prefer "Hello Bao!"

Welcome to the Bao Hypervisor! Get ready for an interactive journey as we explore the world of Bao together. Whether you're a seasoned Bao user or a newcomer, this tour is designed to give you a practical and enthusiastic introduction to our powerful hypervisor.

If you're already familiar with Bao or want to dive into specific setups provided by our team, feel free to skip ahead to the [Bao demos](https://github.com/bao-project/bao-demos) repository.

In this guide, we will take a tour of the different components required to build a setup using the Bao hypervisor and learn how the different components interact. For this purpose, the guide contains the following topics:

- A **getting started** to help users on preparing the environment to build the setup and also some pointers to documentations of Bao (in case you want to go deeper in any detail);

- An **initial setup** for giving the first steps on this tour. This section aims to explore the different components of the system and get the first practical example of this guide;

- An **interactive tutorial on changing the guests** running on top of Bao;

- A **practical example** of changing the setup running;

- An example of **how different guests can coexist and interact** with each other;

The guide can be followed for two target architectures, both emulated with QEMU:

| Architecture     | `ARCH`    | `PLATFORM`          |
|------------------|-----------|---------------------|
| Armv8-A AArch64  | `aarch64` | `qemu-aarch64-virt` |
| RISC-V RV64      | `riscv64` | `qemu-riscv64-virt` |

Most of the steps are exactly the same for both. Whenever a step depends on the architecture, you will find one collapsible block per architecture, like the two below. Open the one that matches your target and ignore the other:

<details>
<summary><b>AArch64</b></summary>

Instructions that only apply to `qemu-aarch64-virt`.

</details>

<details>
<summary><b>RISC-V</b></summary>

Instructions that only apply to `qemu-riscv64-virt`.

</details>

## 1. Getting Started

Before we dive into the thrilling aspects of Bao, let's make sure you're all set up and ready to go. In this section, we'll guide you through preparing your environment to build the setup. Don't worry; we'll provide you with helpful pointers to Bao's documentation in case you want to explore any details further.

### 1.1 Recommended Operating System: Linux (e.g., Ubuntu 22.04 or older versions)
To make the most of this tutorial and the Bao hypervisor, we recommend using a Linux-based operating system. While the instructions may work on other platforms, our focus is on Linux, specifically Ubuntu 22.04 or older versions. This will ensure compatibility and an optimal experience throughout the tour.

### 1.2 Installing Required Dependencies
Before we can dive into the world of Bao, we need to install several dependencies to enable a seamless setup process. Open your terminal and run the following command to install the necessary packages:

```sh
sudo apt install build-essential bison flex git libssl-dev ninja-build \
    u-boot-tools pandoc libslirp-dev pkg-config libglib2.0-dev libpixman-1-dev \
    gettext-base curl xterm cmake python3-pip device-tree-compiler screen
```

This command will install essential tools and libraries required for building and running Bao.
Next, we need to install some Python packages. Execute the following command to do so:

```sh
pip3 install pykwalify packaging pyelftools
```

### 1.3 Download and setup the toolchain

#### 1.3.1. Choosing the Right Toolchain

[arm-toolchains]: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
[riscv-toolchains]: https://github.com/bao-project/bao-riscv-toolchain

Before we delve deeper, let's ensure you have the right tools at your disposal. We'll guide you through obtaining and configuring the appropriate cross-compile toolchain for your target architecture. This step is essential for a smooth development experience.

| Architecture             | Toolchain Name             | Download Link                              |
|--------------------------|:--------------------------:|:------------------------------------------:|
| Armv8 Aarch64            | aarch64-none-elf-          | [Arm Developer][arm-toolchains]            |
| RISC-V                   | riscv64-unknown-elf-       | [Bao RISC-V Toolchain][riscv-toolchains]   |

#### 1.3.2. Installing and Configuring the Toolchain

Install the toolchain. Then, set the **CROSS_COMPILE** environment variable 
with the reference toolchain prefix path:

```sh
export CROSS_COMPILE=/path/to/toolchain/install/dir/bin/your-toolchain-prefix-
```

<details>
<summary><b>RISC-V</b></summary>

The firmware used on RISC-V (OpenSBI) must be linked as a position-independent executable, which the bare-metal `riscv64-unknown-elf-` compiler does not support. For that reason, OpenSBI is built with a Linux compiler instead. You don't need to download anything else: the [Bao RISC-V Toolchain][riscv-toolchains] package ships `riscv64-unknown-linux-gnu-` in the same `bin` directory. Just point a second variable to it:

```sh
export OPENSBI_CROSS_COMPILE=/path/to/toolchain/install/dir/bin/riscv64-unknown-linux-gnu-
```

</details>

### 1.4 Selecting the Target Architecture

Now tell the rest of this guide which architecture you are targeting. These two variables are used by almost every command from here on, so make sure you set them in every new terminal you open:

<details>
<summary><b>AArch64</b></summary>

```sh
export ARCH=aarch64
export PLATFORM=qemu-aarch64-virt
```

</details>

<details>
<summary><b>RISC-V</b></summary>

```sh
export ARCH=riscv64
export PLATFORM=qemu-riscv64-virt
```

</details>

### 1.5 Ensuring Sufficient Free Space

Please be aware that sufficient free space is crucial for this journey, especially due to the Linux image that will be built for the Linux guest VM. To ensure a smooth experience and avoid any space-related issues, we recommend having at least 20GB of free space available on your system.
With your environment set up and all the dependencies installed, you're now ready to dive into the world of Bao hypervisor and create your virtualized wonders!

---

## 2. Initial setup - Taking the First Steps!

Now that you're geared up, it's time to take the first steps on this tour. In the Initial Setup section, we'll explore the different components of the system and walk you through a practical example to give you a solid foundation.

We'll start by cloning this repository:

```sh
git clone https://github.com/bao-project/bao-helloworld.git
cd bao-helloworld
```

To ensure a smooth journey ahead, let's now create a development environment. We'll begin by establishing a directory structure for the various components of our setups. Open up your terminal and execute the following commands:
```sh
export ROOT_DIR=$(realpath .)
export SETUP_BUILD=$ROOT_DIR/bin

export BUILD_GUESTS_DIR=$SETUP_BUILD/guests
export BUILD_BAO_DIR=$SETUP_BUILD/bao
export BUILD_FIRMWARE_DIR=$SETUP_BUILD/firmware
export TOOLS_DIR=$ROOT_DIR/tools/bin

mkdir -p $BUILD_GUESTS_DIR
mkdir -p $BUILD_BAO_DIR
mkdir -p $BUILD_FIRMWARE_DIR
mkdir -p $TOOLS_DIR
```

Upon completing these commands, your directory should resemble the following:
``` sh
├── bin
│   ├── bao
│   ├── firmware
│   └── guests
├── configs
│   ├── aarch64
│   └── riscv64
├── img
│   ├──...
├── srcs
│   ├──...
├── tools
│   └── bin
└── README.md
```

### 2.1. Build Guest - Building Your First Bao Guest

[bao-demos-platforms]: https://github.com/bao-project/bao-demos#appendix-i

Let's kickstart your journey by building your inaugural Bao guest! Here, you'll gain hands-on experience crafting a Baremetal Guest. Let's get that virtual machine up and running! But before we dive into the hands-on excitement, let's understand the setup we're crafting. Our goal is to deploy a baremetal system atop the Bao hypervisor, as illustrated in the figure below:

![Init Setup](/img/single-guest.svg)

> :information_source: For the sake of simplicity and accessibility, we'll detach from physical hardware and use QEMU (don't worry, we'll guide you through its installation later in the tutorial). However, remember that you can apply these steps to various [other platforms][bao-demos-platforms].

To start, let's define an environment variable for the baremetal app source code:
```sh
export BAREMETAL_SRCS=$ROOT_DIR/baremetal
```

Then, clone the Bao baremetal guest application we've prepared (you can skip this step if you already have your own baremetal source):
```sh
git clone https://github.com/bao-project/bao-baremetal-guest.git\
    --branch demo $BAREMETAL_SRCS
```

And now, let's compile it (for simplicity, our example includes a Makefile to compile the baremetal compilation):
```sh
make -C $BAREMETAL_SRCS PLATFORM=$PLATFORM
```

Upon completing these steps, you'll find a binary file in the BAREMETAL_SRCS directory. If you followed our provided Makefile, this precious gem will bear the name ``baremetal.bin``. Now, move the binary file to your build directory (``BUILD_GUESTS_DIR``):

```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-setup
cp $BAREMETAL_SRCS/build/$PLATFORM/baremetal.bin $BUILD_GUESTS_DIR/baremetal-setup/baremetal.bin
```

### 2.2. Build Bao Hypervisor - Laying the Foundation
Next up, we'll guide you through building the Bao Hypervisor itself. This critical step forms the backbone of your virtualization environment.

Our first stride in this journey involves configuring the hypervisor using Bao's configuration file. For this specific setup, we're offering you the configuration file to ease the process ([AArch64](configs/aarch64/baremetal.c) or [RISC-V](configs/riscv64/baremetal.c)). If you're curious to explore different configuration options, our detailed Bao config documentation is [here](https://github.com/bao-project/bao-docs/tree/wip/bao-classic_config) to help.

The configuration tells Bao where to find the guest image we have just built:

```c
VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-setup/baremetal.bin))
```

> :warning: **Warning:** If you are using a directory structure different from the one presented in the tutorial, please make sure to update this line in the configuration file.

The rest of the file describes the virtual machine. This is the part that differs between architectures, since each platform has its own memory map, UART and interrupt controller:

<details>
<summary><b>AArch64</b></summary>

The guest is loaded at `0x50000000` and is given the PL011 UART, the architectural timer interrupt and a virtual GICv3:

```c
.entry = 0x50000000,
...
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
        .interrupts = (irqid_t[]) {27}
    }
},

.arch = {
    .gic = {
        .gicd_addr = 0x08000000,
        .gicr_addr = 0x080A0000,
    }
}
```

</details>

<details>
<summary><b>RISC-V</b></summary>

The guest is loaded at `0x80200000` and is given the 8250 UART and a virtual PLIC (the timer is provided through the SBI/Sstc, so it does not show up in the configuration):

```c
.entry = 0x80200000,
...
.devs =  (struct vm_dev_region[]) {
    {
        /* 8250 UART */
        .pa = 0x10000000,
        .va = 0x10000000,
        .size = 0x1000,
        .interrupt_num = 1,
        .interrupts = (irqid_t[]) {10}
    }
},

.arch = {
    .irqc = {
        .plic = {
            .base = 0xc000000,
        }
    }
}
```

</details>

Undoubtedly, if we're envisioning our baremetal system dancing atop the hypervisor stage, we first need that hypervisor in place. Fear not, for our adept team has already shouldered the arduous task. Bao stands ready and waiting for you to harness its power. No need to roll up your sleeves; it's a breeze. Let's embark on this stage-setting journey:

#### 2.2.1. Cloning the Bao Hypervisor
Your gateway to seamless virtualization begins with cloning the Bao Hypervisor repository. Execute the following commands in your terminal to initiate this crucial step:
```sh
export BAO_SRCS=$ROOT_DIR/bao
git clone https://github.com/bao-project/bao-hypervisor $BAO_SRCS
git -C $BAO_SRCS checkout 73e8fab4ddf2d83589cb50893ee21e2dfa59dc87
```

#### 2.2.2. Compiling Bao Hypervisor
With all set, it's time to bring your Bao Hypervisor to life. You now just need to compile it! Before that, there is one build parameter that depends on your architecture:

<details>
<summary><b>AArch64</b></summary>

No extra parameters are needed:

```sh
export BAO_PARAMS=""
```

</details>

<details>
<summary><b>RISC-V</b></summary>

For this platform Bao uses the RISC-V Advanced Interrupt Architecture (AIA) by default. The guests in this guide, including the Linux kernel version we build later, use the PLIC instead, so we ask Bao to do the same:

```sh
export BAO_PARAMS="IRQC=PLIC"
```

</details>

Note that `CONFIG_REPO` points to the folder holding the configuration files of your architecture, and `CONFIG` selects one of them:
```sh
make -C $BAO_SRCS\
    PLATFORM=$PLATFORM\
    CONFIG_REPO=$ROOT_DIR/configs/$ARCH\
    CONFIG=baremetal\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD\
    $BAO_PARAMS
```

Upon completing these steps, you'll find a binary file in the BAO_SRCS directory, called bao.bin. Now, move the binary file to your build directory (BUILD_BAO_DIR):

```sh
cp $BAO_SRCS/bin/$PLATFORM/baremetal/bao.bin $BUILD_BAO_DIR/bao.bin
```

## 3. Build Firmware - Powering Up Your Setup

No journey is truly complete without firmware. It's the fuel that powers your virtual world. That's why we're here to guide you through acquiring the essential firmware tailored to your target platform (you can find the pointer to build the firmware to other platforms [here](https://github.com/bao-project/bao-demos#b5-build-firmware-and-deploy)).

### 3.1 Welcome to the QEMU platform!

Why bother with a hardware platform when you have QEMU? If you haven't got it yet, fret not. We're here to guide you through the process of building and installing it.

However, if you're already equipped with `qemu-system-aarch64` or `qemu-system-riscv64` (the one for your target), or if compiling isn't your cup of tea and you'd rather install it directly using a package manager or another method, ensure that you're working with version 7.2.0 or higher. In that case, you can jump ahead to the next step.

To install QEMU, simply run the following commands:

```sh
export QEMU_DIR=$ROOT_DIR/tools/qemu-$ARCH
git clone https://github.com/qemu/qemu.git $QEMU_DIR --depth 1\
   --branch v11.0.0
cd $QEMU_DIR
./configure --target-list=$ARCH-softmmu --enable-slirp
make -j$(nproc)
sudo make install
cd $ROOT_DIR
```

### 3.2 Now you need the platform firmware

The firmware that runs before Bao is the main difference between the two architectures:

<details>
<summary><b>AArch64</b></summary>

On AArch64 we use TF-A, which then hands over to U-Boot. U-Boot is the one that will jump to Bao.

To build u-boot, execute the following commands:

```sh
export UBOOT_DIR=$ROOT_DIR/tools/u-boot
git clone https://github.com/u-boot/u-boot.git $UBOOT_DIR --depth 1\
   --branch v2022.10

cd $UBOOT_DIR
make qemu_arm64_defconfig

echo "CONFIG_TFABOOT=y" >> .config
echo "CONFIG_SYS_TEXT_BASE=0x60000000" >> .config

make -j$(nproc)

cp $UBOOT_DIR/u-boot.bin $TOOLS_DIR
```

One more tool to go! Let's build TF-A:
```sh
export ATF_DIR=$ROOT_DIR/tools/arm-trusted-firmware
git clone https://github.com/bao-project/arm-trusted-firmware.git\
   $ATF_DIR --branch bao/demo --depth 1
cd $ATF_DIR
make PLAT=qemu bl1 fip BL33=$TOOLS_DIR/u-boot.bin\
   QEMU_USE_GIC_DRIVER=QEMU_GICV3
dd if=$ATF_DIR/build/qemu/release/bl1.bin\
   of=$TOOLS_DIR/flash.bin
dd if=$ATF_DIR/build/qemu/release/fip.bin\
   of=$TOOLS_DIR/flash.bin seek=64 bs=4096 conv=notrunc
cd $ROOT_DIR
```

</details>

<details>
<summary><b>RISC-V</b></summary>

On RISC-V we use OpenSBI, which jumps directly to Bao. For now you only need to get its sources:

```sh
export OPENSBI_DIR=$ROOT_DIR/tools/opensbi
git clone https://github.com/bao-project/opensbi.git $OPENSBI_DIR\
    --depth 1 --branch bao/demo
```

OpenSBI carries Bao as its payload, which means the Bao image gets embedded in the firmware binary. Because of that, we will build it right before launching QEMU, in the next section, and build it again every time Bao changes.

</details>

## 4. Let's Try It Out! - Unleash the Power

Now that the stage is set, it's time to witness the magic firsthand. Brace yourself as we ignite the virtual flames and bring your creation to life. Get ready for an experience like no other as we embark on this journey:

:white_check_mark: Build guest (baremetal)

:white_check_mark: Build bao hypervisor

:white_check_mark: Build firmware (qemu)

With all the pieces in place, it's time to launch QEMU and behold the fruits of your labor. The moment of truth awaits, so let's dive right in. These are the commands we will come back to every time we want to run a new setup:

<details>
<summary><b>AArch64</b></summary>

```sh
qemu-system-aarch64 -nographic\
   -M virt,secure=on,virtualization=on,gic-version=3 \
   -cpu cortex-a53 -smp 4 -m 4G\
   -bios $TOOLS_DIR/flash.bin \
   -device loader,file="$BUILD_BAO_DIR/bao.bin",addr=0x50000000,force-raw=on\
   -device virtio-net-device,netdev=net0 -netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
   -device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3
```

Now, you should see TF-A and U-boot initialization messages:

![Qemu Boot](/img/TF-A_U-boot.png)

Once you get the U-Boot prompt (`=>`), make u-boot jump to where the bao image was loaded:
```sh
go 0x50000000
```

</details>

<details>
<summary><b>RISC-V</b></summary>

First, build OpenSBI with the Bao image we have just compiled as its payload:

```sh
make -C $OPENSBI_DIR PLATFORM=generic \
    CROSS_COMPILE=$OPENSBI_CROSS_COMPILE \
    FW_PAYLOAD=y \
    FW_PAYLOAD_FDT_ADDR=0x80100000\
    FW_PAYLOAD_PATH=$BUILD_BAO_DIR/bao.bin
cp $OPENSBI_DIR/build/platform/generic/firmware/fw_payload.bin $TOOLS_DIR/opensbi.bin
```

Then launch QEMU:

```sh
qemu-system-riscv64 -nographic\
   -M virt -cpu rv64,priv_spec=v1.13.0,sstc=true,svpbmt=true -smp 4 -m 4G\
   -bios $TOOLS_DIR/opensbi.bin\
   -device virtio-net-device,netdev=net0 -netdev user,id=net0,net=192.168.42.0/24,hostfwd=tcp:127.0.0.1:5555-:22\
   -device virtio-serial-device -chardev pty,id=serial3 -device virtconsole,chardev=serial3
```

You should see the OpenSBI initialization messages, and Bao starts right after them, with no further action needed.

</details>

In both cases, QEMU starts by revealing the pseudoterminal where it placed the virtio serial. Here's an example:

```sh
char device redirected to /dev/pts/4 (label serial3)
```

You can ignore it for now. We'll use it later, when we add a Linux guest.

And you should have an output as follows:

![System Init](/img/System_Init.png)

When you want to leave QEMU press `Ctrl-a` then `x`.

## 5. Well, Maybe the Setup Was Not Perfect...

As we continue on this thrilling tour, it's time to explore the art of changing your Bao setup. Mastering the ability to modify your virtual environment opens up endless possibilities. Don't worry if you encounter a few challenges along the way; learning through hands-on experience is the key!

In the following sections, we'll walk you through step-by-step instructions to make various changes to your guests. By the end of this part of the tour, you'll have a deeper understanding of how the different components interact, and you'll be confidently making adjustments to suit your needs.

### 5.1 Add a second guest - freeRTOS

In this section, we'll delve into various scenarios and demonstrate how to configure specific environments using Bao. One of Bao's notable strengths lies in its flexibility, allowing you to tailor your setup to a range of requirements.

Let's kick things off by incorporating a second VM running FreeRTOS.

![Init Setup](/img/dual-guest-rtos.svg)

First, we can use the baremetal compiled from the first setup:
```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup
cp $BAREMETAL_SRCS/build/$PLATFORM/baremetal.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup/baremetal.bin
```

#### 5.1.1. Compile freeRTOS
Then, let's compile our new guest:

```sh
export FREERTOS_SRCS=$ROOT_DIR/freertos
export FREERTOS_PARAMS="STD_ADDR_SPACE=y SHMEM_BASE=0xD0000000 SHMEM_SIZE=0x1000000"

git clone --recursive --shallow-submodules\
    https://github.com/bao-project/freertos-over-bao.git\
    $FREERTOS_SRCS --branch bao-helloworld
make -C $FREERTOS_SRCS PLATFORM=$PLATFORM $FREERTOS_PARAMS
```

> :information_source: `STD_ADDR_SPACE=y` builds FreeRTOS for a platform-independent memory map (memory at `0x0`, UART at `0xff000000`). This is why the FreeRTOS VM looks almost the same in the configuration files of both architectures. `SHMEM_BASE` and `SHMEM_SIZE` tell it where to find the shared memory that we will use [later](#53-guests-must-socialize-right) to talk to Linux.

Upon completing these steps, you'll find a binary file in the `FREERTOS_SRCS` directory, called `freertos.bin`. Move the binary file to your build directory (`BUILD_GUESTS_DIR`):

```sh
cp $FREERTOS_SRCS/build/$PLATFORM/freertos.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup/free-rtos.bin
```

#### 5.1.2. Integrating the new guest

Now, we have both guests needed for our setup. However, there are some steps required to fit the two VMs on our platform. Let's understand the differences between the configuration of the first setup and the configuration of the second setup.

First of all, we need to add the second VM image:

```diff
- VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-setup/baremetal.bin))
+ VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/baremetal.bin))
+ VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/free-rtos.bin))
```

Also, since we now have 2 VMs, we need to change the `vmlist_size` in our configuration:

```diff
- .vmlist_size = 1,
+ .vmlist_size = 2,
```

Next, we need to think about resources. In the first setup, we assigned 4 vCPUs to the baremetal. But this time, we need to split the vCPUs between the two VMs:

```diff
- .cpu_num = 4,
+ .cpu_num = 2,
```

The platform has a single UART, which is now shared by both guests. Since an interrupt can only be assigned to one VM, the UART interrupt is removed from the baremetal VM.

Additionally, we need to include all the configurations of the second VM. (Details are omitted for simplicity but you can check further details in the configuration file for [AArch64](configs/aarch64/baremetal-freeRTOS.c) or [RISC-V](configs/riscv64/baremetal-freeRTOS.c)):
```diff
+        { 
+            .image = {
+                .base_addr = 0x0,
+                .load_addr = VM_IMAGE_OFFSET(freertos_image),
+                .size = VM_IMAGE_SIZE(freertos_image)
+            },
+
+            ...        // omitted for simplicity
+        },
```

#### 5.1.3. Let's rebuild Bao!

As we've seen, changing the guests includes changing the configuration file. Therefore, we need to repeat the process of building Bao. Please note that the flag `CONFIG` defines the configuration file to be used on the compilation of Bao!

```sh
make -C $BAO_SRCS\
    PLATFORM=$PLATFORM\
    CONFIG_REPO=$ROOT_DIR/configs/$ARCH\
    CONFIG=baremetal-freeRTOS\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD\
    $BAO_PARAMS
```

Upon completing these steps, you'll find a binary file in the `BAO_SRCS` directory, called `bao.bin`. Move the binary file to your build directory (`BUILD_BAO_DIR`):

```sh
cp $BAO_SRCS/bin/$PLATFORM/baremetal-freeRTOS/bao.bin $BUILD_BAO_DIR/bao.bin
```

#### 5.1.4. Ready for launch!

Now, we have everything configured for testing our new setup! Just repeat the steps from [section 4](#4-lets-try-it-out---unleash-the-power) for your architecture:

- **AArch64**: launch QEMU with the same command and run `go 0x50000000` at the U-Boot prompt;
- **RISC-V**: rebuild OpenSBI, so that it picks up the new `bao.bin`, and launch QEMU with the same command.

You should now see the output of both guests, with the two FreeRTOS tasks printing alongside the baremetal handlers:

```
Bao bare-metal test guest
Bao FreeRTOS guest
cpu 0 up
cpu 1 up
Task1: 0
Task2: 0
cpu0: timer_handler
cpu1: ipi_handler
Task1: 1
Task2: 1
```

### 5.2 It was still not perfect right? Let's try out a Linux too

Let's now introduce a third VM running the Linux OS.

![Init Setup](/img/triple-guest.svg)

First, we can re-use our guests from the previous setup:
```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup
cp $BAREMETAL_SRCS/build/$PLATFORM/baremetal.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/baremetal.bin
cp $FREERTOS_SRCS/build/$PLATFORM/freertos.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/free-rtos.bin
```

#### 5.2.1 Build Linux Guest

Now let's start by building our linux guest. Setup linux environment variables:
```sh
export LINUX_DIR=$ROOT_DIR/linux
export LINUX_REPO=https://github.com/torvalds/linux.git
export LINUX_VERSION=v6.1

export LINUX_SRCS=$LINUX_DIR/linux-$LINUX_VERSION

mkdir -p $LINUX_DIR/linux-$LINUX_VERSION
mkdir -p $LINUX_DIR/linux-build

git clone $LINUX_REPO $LINUX_SRCS\
    --depth 1 --branch $LINUX_VERSION
cd $LINUX_SRCS
git apply $ROOT_DIR/srcs/patches/$LINUX_VERSION/*.patch
```

Setup and environment variable pointing to the target architecture and platform specific config to be used by buildroot:

```sh
export LINUX_CFG_FRAG=$(ls $ROOT_DIR/srcs/configs/base.config\
    $ROOT_DIR/srcs/configs/$ARCH.config\
    $ROOT_DIR/srcs/configs/$PLATFORM.config 2> /dev/null)
```

Setup buildroot environment variables:
```sh
export BUILDROOT_SRCS=$LINUX_DIR/buildroot-$ARCH-$LINUX_VERSION
export BUILDROOT_DEFCFG=$ROOT_DIR/srcs/buildroot/$ARCH.config
export LINUX_OVERRIDE_SRCDIR=$LINUX_SRCS
```

Clone the latest buildroot at the latest stable version
```sh
git clone https://github.com/buildroot/buildroot.git $BUILDROOT_SRCS\
    --depth 1 --branch 2022.11
cd $BUILDROOT_SRCS
```

Use our provided buildroot defconfig, which itselfs points to the a Linux kernel defconfig and patches and build (grab a coffee, this is the longest step of the guide):
```sh
make defconfig BR2_DEFCONFIG=$BUILDROOT_DEFCFG
make linux-reconfigure all

mv $BUILDROOT_SRCS/output/images/Image\
    $BUILDROOT_SRCS/output/images/Image-$PLATFORM
cd $ROOT_DIR
```
The device tree for this setup is available in `srcs/devicetrees/$PLATFORM`. For a device tree file named linux.dts define a virtual machine variable and build:
```sh
export LINUX_VM=linux
dtc $ROOT_DIR/srcs/devicetrees/$PLATFORM/$LINUX_VM.dts >\
    $LINUX_DIR/linux-build/$LINUX_VM.dtb
```

Wrap the kernel image and device tree blob in a single binary:
```sh
make -j $(nproc) -C $ROOT_DIR/srcs/lloader\
    ARCH=$ARCH\
    IMAGE=$BUILDROOT_SRCS/output/images/Image-$PLATFORM\
    DTB=$LINUX_DIR/linux-build/$LINUX_VM.dtb\
    TARGET=$LINUX_DIR/linux-build/$LINUX_VM
```

Finaly, copy the binary file to the (compiled) guests folder:
```sh
cp $LINUX_DIR/linux-build/$LINUX_VM.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/linux.bin
```


#### 5.2.2 Welcome our new guest!

After building our new guest, it's time to integrate in our setup. You can find all the details in the configuration file for [AArch64](configs/aarch64/baremetal-freeRTOS-linux.c) or [RISC-V](configs/riscv64/baremetal-freeRTOS-linux.c).

 After that, we need to load our guests:
```diff
- VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/baremetal.bin))
- VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/free-rtos.bin))
+ VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/baremetal.bin))
+ VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/free-rtos.bin))
+ VM_IMAGE(linux_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/linux.bin))
```

Let's now update our VM list size to integrate our new guest:
```diff
-    .vmlist_size = 2,
+    .vmlist_size = 3,
```

Then, we need to rearrange the number of vCPUs:
```diff
    // baremetal configuration
    {
-       .cpu_num = 2,
+       .cpu_num = 1,
        ...
    },

    // freeRTOS configuration
    {   
-       .cpu_num = 2,
+       .cpu_num = 1,
        ...
    },

    // linux configuration
    {   
+       .cpu_num = 2,
    }
```

Finally, the Linux VM gets 1GiB of memory and the platform's virtio devices, which provide its network interface and its console. Once again, where these live depends on the platform:

<details>
<summary><b>AArch64</b></summary>

```c
.entry = 0x60000000,
...
.regions =  (struct vm_mem_region[]) {
    {
        .base = 0x60000000,
        .size = 0x40000000,
        .place_phys = true,
        .phys = 0x60000000
    }
},
...
{
    /* virtio devices */
    .pa = 0xa003000,
    .va = 0xa003000,
    .size = 0x1000,
    .interrupt_num = 8,
    .interrupts = (irqid_t[]) {72,73,74,75,76,77,78,79}
},
```

</details>

<details>
<summary><b>RISC-V</b></summary>

```c
.entry = 0x90200000,
...
.regions =  (struct vm_mem_region[]) {
    {
        .base = 0x90000000,
        .size = 0x40000000,
        .place_phys = true,
        .phys = 0x90000000
    }
},
...
{
    /* virtio devices */
    .pa = 0x10001000,
    .va = 0x10001000,
    .size = 0x8000,
    .interrupt_num = 8,
    .interrupts = (irqid_t[]) {1,2,3,4,5,6,7,8}
},
```

</details>

#### 5.2.3. Let's rebuild Bao!

As we've seen, changing the guests includes changing the configuration file. Therefore, we need to repeat the process of building Bao:

```sh
make -C $BAO_SRCS\
    PLATFORM=$PLATFORM\
    CONFIG_REPO=$ROOT_DIR/configs/$ARCH\
    CONFIG=baremetal-freeRTOS-linux\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD\
    $BAO_PARAMS
```

Upon completing these steps, you'll find a binary file in the `BAO_SRCS` directory, called `bao.bin`. Move the binary file to your build directory (`BUILD_BAO_DIR`):

```sh
cp $BAO_SRCS/bin/$PLATFORM/baremetal-freeRTOS-linux/bao.bin $BUILD_BAO_DIR/bao.bin
```

#### 5.2.4. Ready to go!

With all the pieces in place, it's time to launch QEMU and behold the fruits of your labor. Once again, repeat the steps from [section 4](#4-lets-try-it-out---unleash-the-power) for your architecture (remember that on RISC-V this includes rebuilding OpenSBI, and on AArch64 running `go 0x50000000` in U-Boot).

The platform's UART is assigned to the baremetal and the FreeRTOS guests, so they keep printing in the terminal where you launched QEMU. The Linux guest uses the virtio console instead. This is where the pseudoterminal that QEMU revealed at startup comes into play:

```sh
char device redirected to /dev/pts/4 (label serial3)
```

To make the connection, open a fresh terminal window and establish a connection to the specified pseudoterminal. Here's how:

```sh
screen /dev/pts/4
```

> :warning: **Warning:** The number of the pseudoterminal changes from run to run. Use the one QEMU printed for you.

You can log in as `root`, using the password `root`. The Linux guest is also accessible via ssh at the static address 192.168.42.15, which QEMU forwards to port 5555 of your machine:

```sh
ssh root@localhost -p 5555
```

### 5.3 Guests must socialize, right?

In certain scenarios, it's imperative for guests to establish a communication channel. To accomplish this, we'll utilize shared memory and Inter-Process Communication (IPC) mechanisms, allowing the Linux VM to seamlessly interact with the system.

![Init Setup](/img/triple-guest-shmem.svg)

On the Bao side, this is described by a shared memory object, declared at the top of the configuration, and by an IPC in each of the VMs that are allowed to use it (see the configuration file for [AArch64](configs/aarch64/baremetal-freeRTOS-linux-shmem.c) or [RISC-V](configs/riscv64/baremetal-freeRTOS-linux-shmem.c)):

```c
.shmemlist_size = 1,
.shmemlist = (struct shmem[]) {
    [0] = { .size = 0x00010000, }
},
...
    // freeRTOS configuration
    .ipcs = (struct ipc[]) {
        {
            .base = 0xD0000000,
            .size = 0x00010000,
            .shmem_id = 0,
            .interrupt_num = 1,
            .interrupts = (irqid_t[]) {52}
        }
    },
...
    // linux configuration
    .ipcs = (struct ipc[]) {
        {
            .base = 0xf0000000,
            .size = 0x00010000,
            .shmem_id = 0,
            .interrupt_num = 1,
            .interrupts = (irqid_t[]) {52}
        }
    },
```

The FreeRTOS guest we built already knows about its end of the channel (remember `SHMEM_BASE=0xD0000000`?). What is missing is telling Linux about its own.

#### 5.3.1. Add Shared Memory and IPC to our guest
Let's kick off by integrating an IPC into Linux. To do this, we'll make the necessary additions to the Linux device-tree. For simplicity, the `linux-shmem.dts` file in `srcs/devicetrees/$PLATFORM` already encompasses the following changes:

<details>
<summary><b>AArch64</b></summary>

```diff
+    bao-ipc@f0000000 {
+        compatible = "bao,ipcshmem";
+        reg = <0x0 0xf0000000 0x0 0x00010000>;
+        read-channel = <0x0 0x2000>;
+        write-channel = <0x2000 0x2000>;
+        interrupts = <0 52 1>;
+        id = <0>;
+    };
```

</details>

<details>
<summary><b>RISC-V</b></summary>

```diff
+    bao-ipc@f0000000 {
+        compatible = "bao,ipcshmem";
+        reg = <0x0 0xf0000000 0x0 0x00010000>;
+        read-channel = <0x0 0x2000>;
+        write-channel = <0x2000 0x2000>;
+        interrupt-parent = <&plic>;
+        interrupts = <52>;
+        id = <0>;
+    };
```

</details>

Now, let's generate the updated device tree:

```sh
export LINUX_VM=linux-shmem
dtc $ROOT_DIR/srcs/devicetrees/$PLATFORM/$LINUX_VM.dts >\
    $LINUX_DIR/linux-build/$LINUX_VM.dtb
```

> :warning: **Warning:** To correctly introduce these changes, you need to ensure that you applied the patch to Linux, as described [before](#521-build-linux-guest).

Bundle the kernel image and device tree blob into a single binary:
```sh
make -j $(nproc) -C $ROOT_DIR/srcs/lloader\
    ARCH=$ARCH\
    IMAGE=$BUILDROOT_SRCS/output/images/Image-$PLATFORM\
    DTB=$LINUX_DIR/linux-build/$LINUX_VM.dtb\
    TARGET=$LINUX_DIR/linux-build/$LINUX_VM
```

Finally, move the binary file to the (compiled) guests folder, along with the other two guests:
```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-shmem-setup
cp $LINUX_DIR/linux-build/$LINUX_VM.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-shmem-setup/$LINUX_VM.bin
cp $FREERTOS_SRCS/build/$PLATFORM/freertos.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-shmem-setup/free-rtos.bin
cp $BAREMETAL_SRCS/build/$PLATFORM/baremetal.bin $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-shmem-setup/baremetal.bin
```

#### 5.3.2. Rebuild Bao

Given that you've modified one of the guests, it's now essential to rebuild Bao:
```sh
make -C $BAO_SRCS\
    PLATFORM=$PLATFORM\
    CONFIG_REPO=$ROOT_DIR/configs/$ARCH\
    CONFIG=baremetal-freeRTOS-linux-shmem\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD\
    $BAO_PARAMS
```

Upon successful completion, you'll locate a binary file named bao.bin in the ``BAO_SRCS`` directory. Move it to your build directory (``BUILD_BAO_DIR``):

```sh
cp $BAO_SRCS/bin/$PLATFORM/baremetal-freeRTOS-linux-shmem/bao.bin $BUILD_BAO_DIR/bao.bin
```


#### 5.3.3. Run Our Setup

Now, you're ready to execute the final setup. Launch it as you did for the [previous one](#524-ready-to-go), and connect to the Linux guest.

If all went according to plan, you should be able to spot the IPC on Linux by running the following command:
```sh
ls /dev
```

You'll see your IPC as depicted in the following image:
![Init Setup](/img/shmem-IPC.png)

From here, you can employ the IPC on Linux to dispatch messages to FreeRTOS by writing to ``/dev/baoipc0``:
```sh
echo "Hello, Bao!" > /dev/baoipc0
```

In the terminal where you launched QEMU, FreeRTOS acknowledges it:
```
message from linux: Hello, Bao!
```

Or retrieve the latest FreeRTOS message by reading from ``/dev/baoipc0``:
```sh
cat /dev/baoipc0
```
