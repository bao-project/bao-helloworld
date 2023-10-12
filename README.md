# "Hello world"? We prefer "Hello Bao!"

Welcome to the Bao Hypervisor! Get ready for an interactive journey as we
explore the world of Bao together. Whether you're a seasoned Bao user or a
newcomer, this tour is designed to give you a practical and enthusiastic
introduction to our powerful hypervisor.

If you're already familiar with Bao or want to dive into specific setups
provided by our team, feel free to skip ahead to the Bao demos section.

In this guide, we will take a tour of the different components required to
build a setup using the Bao hypervisor and learn how the different components
interact. For this purpose, the guide contains the following topics:

- A **getting started** to help users on preparing the environment to build the
  setup and also some pointers to documentations of Bao (in case you want to go
  deeper in any detail);

- An **initial setup** for giving the first steps on this tour. This section
  aims to explore the different components of the system and get the first
  practical example of this guide;

- An **interactive tutorial on changing the guests** running on top of Bao;

- A **practical example** of changing the setup running;

- An example of **how different guests can coexist and interact** with each
  other;

## 1. Getting Started

Before we dive into the thrilling aspects of Bao, let's make sure you're all
set up and ready to go. In this section, we'll guide you through preparing your
environment to build the setup. Don't worry; we'll provide you with helpful
pointers to Bao's documentation in case you want to explore any details
further.

### 1.1 Recommended Operating System: Linux (e.g., Ubuntu 22.04)
To make the most of this tutorial and the Bao hypervisor, we recommend using a
Linux-based operating system. While the instructions may work on other
platforms, our focus is on Linux, specifically Ubuntu 22.04 or older versions.
This will ensure compatibility and an optimal experience throughout the tour.

### 1.2 Installing Required Dependencies
Before we can dive into the world of Bao, we need to install several
dependencies to enable a seamless setup process. Open your terminal and run the
following command to install the necessary packages:

```sh
sudo apt install build-essential bison flex git libssl-dev ninja-build \
    u-boot-tools pandoc libslirp-dev pkg-config libglib2.0-dev libpixman-1-dev \
    gettext-base curl xterm cmake python3-pip
```

This command will install essential tools and libraries required for building
and running Bao. Next, we need to install some Python packages. Execute the
following command to do so:

```sh
pip3 install pykwalify packaging pyelftools
```

### 1.3 Download and setup the toolchain

#### 1.3.1. Choosing the Right Toolchain

[arm-toolchains]:
  https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
[riscv-toolchains]:
  https://github.com/sifive/freedom-tools/releases

Before we delve deeper, let's ensure you have the right tools at your disposal.
We'll guide you through obtaining and configuring the appropriate cross-compile
toolchain for your target architecture. This step is essential for a smooth
development experience.

|  Architecture  | Toolchain Name       | Download Link                    |
|----------------|:--------------------:|:--------------------------------:|
| Armv8 Aarch64  | aarch64-none-elf-    | [Arm Developer][arm-toolchains]  |
| Armv7/8 Aarch32| arm-none-eabi-       | [Arm Developer][arm-toolchains]  |
| RISC-V         | riscv64-unknown-elf- | [SiFive Tools][riscv-toolchains] |



#### 1.3.2. Installing and Configuring the Toolchain

Install the toolchain. Then, set the **CROSS_COMPILE** environment variable
with the reference toolchain prefix path:

```sh
export CROSS_COMPILE=/path/to/toolchain/install/dir/bin/your-toolchain-prefix-
```

### 1.4 Ensuring Enough Free Space

Please be aware that sufficient free space is crucial for this journey,
especially due to the Linux image that will be built for the Linux guest VM.
To ensure a smooth experience and avoid any space-related issues, we recommend
having at least 13 GiB of free space available on your system. With your
environment set up and all the dependencies installed, you're now ready to dive
into the world of Bao hypervisor and create your virtualized wonders!

| Component         | Required Space | Percentage of space Required |
|-------------------|:--------------:|:---------------------------: |
| Bao               | 155.8 MiB      | 1.23%                        |
| Guest (Linux)     | 10.5 GiB       | 84.74%                       |
| Guest (freeRTOS)  | 24.8 MiB       | 0.20%                        |
| Guest (baremetal) | 4.2 MiB        | 0.03%                        |
| Tools (QEMU)      | 1.7 GiB        | 13.74%                       |
| Tools (u-boot)    | 237 MiB        | 1.87%                        |
| Tools (ATF)       | 52 MiB         | 0.41%                        |

---

## 2. Initial setup - Taking the First Steps!

Now that you're geared up, it's time to take the first steps on this tour. In
the Initial Setup section, we'll explore the different components of the system
and walk you through a practical example to give you a solid foundation.

To ensure a smooth journey ahead, let's start by creating a development
environment. We'll begin by establishing a directory structure for the various
components of our setups. Open up your terminal and execute the following
commands:

```sh
export ROOT_DIR=$(realpath .)
export SETUP_BUILD=$ROOT_DIR/bin
export PATCHES_DIR=$ROOT_DIR/patches

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

### 2.1. Build Guest - Building Your First Bao Guest

[bao-demos-platforms]: https://github.com/bao-project/bao-demos#appendix-i

Let's kickstart your journey by building your inaugural Bao guest! Here, you'll
gain hands-on experience crafting a Baremetal Guest. Let's get that virtual
machine up and running!But before we dive into the hands-on excitement, let's
understand the setup we're crafting. Our goal is to deploy a baremetal system
atop the Bao hypervisor, as illustrated in the figure below:

![Init Setup](/img/single-guest.svg)

:information_source: For the sake of simplicity and accessibility, we'll detach
from physical hardware and use QEMU (don't worry, we'll guide you through its
installation later in the tutorial). However, remember that you can apply these
steps to various [other platforms][arm-toolchains].


To start, let's define an environment variable for the baremetal app source
code:

```sh
export BAREMETAL_SRCS=$ROOT_DIR/baremetal
```

Then, clone the Bao baremetal guest application we've prepared (you can skip
this step if you already have your own baremetal source):
```sh
git clone https://github.com/bao-project/bao-baremetal-guest.git\
    --branch demo $BAREMETAL_SRCS
```

And now, let's compile it (for simplicity, our example includes a Makefile to
compile the baremetal compilation):

```sh
git -C $BAREMETAL_SRCS apply $PATCHES_DIR/baremetal.patch
make -C $BAREMETAL_SRCS PLATFORM=qemu-aarch64-virt
```

Upon completing these steps, you'll find a binary file in the
``BAREMETAL_SRCS`` directory. If you followed our provided Makefile, this
precious gem will bear the name ``baremetal.bin``. Now, move the binary file to
your build directory (``BUILD_GUESTS_DIR``):

```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-setup
cp $BAREMETAL_SRCS/build/qemu-aarch64-virt/baremetal.bin \
    $BUILD_GUESTS_DIR/baremetal-setup/baremetal.bin
```

### 2.2. Build Bao Hypervisor - Laying the Foundation
Next up, we'll guide you through building the Bao Hypervisor itself. This
critical step forms the backbone of your virtualization environment.

Our first stride in this journey involves configuring the hypervisor using
Bao's configuration file. For this specific setup, we're offering you the
[configuration file](configs/baremetal.c) to ease the process. If you're
curious to explore different configuration options, our detailed our detailed
Bao config documentation is
[here](https://github.com/bao-project/bao-docs/tree/wip/bao-\
classic_config) to help.

```c
VM_IMAGE(baremetal_image, XSTR(BUILD_GUESTS_DIR/baremetal-setup/baremetal.bin));
```

:warning: If you are using a directory structure of the one presented in the
tutorial, please make sure to update the following code in the
[configuration file](configs/baremetal.c).

Undoubtedly, if we're envisioning our baremetal system dancing atop the
hypervisor stage, we first need that hypervisor in place. Fear not, for our
adept team has already shouldered the arduous task. Bao stands ready and
waiting for you to harness its power. No need to roll up your sleeves; it's a
breeze. Let's embark on this stage-setting journey:

#### 2.2.1. Cloning the Bao Hypervisor
Your gateway to seamless virtualization begins with cloning the Bao Hypervisor
repository. Execute the following commands in your terminal to initiate this
crucial step:

```sh
export BAO_SRCS=$ROOT_DIR/bao
git clone https://github.com/bao-project/bao-hypervisor $BAO_SRCS\
    --branch demo
```

#### 2.2.2. Copying Your Configuration

Now, let's ensure your unique configuration is seamlessly integrated. Copy your
configuration file to the working directory with the following commands:

```sh
mkdir -p $mkdir -p $BUILD_BAO_DIR/config
cp -L $ROOT_DIR/configs/baremetal.c\
    $BUILD_BAO_DIR/config/baremetal.c
```

#### 2.2.3. Compiling Bao Hypervisor
With all set, it's time to bring your Bao Hypervisor to life. You now just need
to compile it!

```sh
make -C $BAO_SRCS\
    PLATFORM=qemu-aarch64-virt\
    CONFIG_REPO=$ROOT_DIR/configs\
    CONFIG=baremetal\
    CONFIG_BUILTIN=y\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$BUILD_GUESTS_DIR
```

Upon completing these steps, you'll find a binary file in the BAO_SRCS
directory, called bao.bin. Now, move the binary file to your build directory:

```sh
cp $BAO_SRCS/bin/qemu-aarch64-virt/baremetal/bao.bin $BUILD_BAO_DIR/bao.bin
```

## 3. Build Firmware - Powering Up Your Setup

No journey is truly complete without firmware. It's the fuel that powers your
virtual world. That's why we're here to guide you through acquiring the
essential firmware tailored to your target platform (you can find the pointer
to build the firmware to other platforms
[here](https://github.com/bao-project/bao-demos#b5-build-firmware-and-deploy)).

### 3.1 Welcome to the QEMU platform!

Why bother with a hardware platform when you have QEMU? If you haven't got it
yet, fret not. We're here to guide you through the process of building and
installing it. In this guide, our focus will be on Aarch64 QEMU.

However, if you're already equipped with qemu-system-aarch64, or if compiling
isn't your cup of tea and you'd rather install it directly using a package
manager or another method, ensure that you're working with version 7.2.0 or
higher. In that case, you can jump ahead to the next step.

To install QEMU, simply run the following commands:

```sh
export QEMU_DIR=$ROOT_DIR/tools/qemu-aarch64
export TOOLS_DIR=$ROOT_DIR/tools/bin
mkdir -p $ROOT_DIR/tools/bin
mkdir -p $TOOLS_DIR
git clone https://github.com/qemu/qemu.git $QEMU_DIR --depth 1\
   --branch v7.2.0
cd $QEMU_DIR
./configure --target-list=aarch64-softmmu --enable-slirp
make -j$(nproc)
sudo make install
```

### 3.2 Now you need u-boot

To build and install u-boot, execute the following commands:

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

### 3.3 Almost there, let's build TF-A

One more tool to go! Let's build TF-A:
```sh
export ATF_DIR=$ROOT_DIR/tools/arm-trusted-firmware
git clone https://github.com/bao-project/arm-trusted-firmware.git\
   $ATF_DIR --branch bao/demo --depth 1
cd $ATF_DIR
make PLAT=qemu bl1 fip BL33=$$TOOLS_DIR/u-boot.bin\
   QEMU_USE_GIC_DRIVER=QEMU_GICV3
dd if=$ATF_DIR/build/qemu/release/bl1.bin\
   of=$TOOLS_DIR/flash.bin
dd if=$ATF_DIR/build/qemu/release/fip.bin\
   of=$TOOLS_DIR/flash.bin seek=64 bs=4096 conv=notrunc
```


## 4. Let's Try It Out! - Unleash the Power

Now that the stage is set, it's time to witness the magic firsthand. Brace
yourself as we ignite the virtual flames and bring your creation to life. Get
ready for an experience like no other as we embark on this journey:

:white_check_mark: Build guest (baremetal)

:white_check_mark: Build bao hypervisor

:white_check_mark: Build firmware (qemu)

With all the pieces in place, it's time to launch QEMU and behold the fruits of
your labor. The moment of truth awaits, so let's dive right in:

```sh
qemu-system-aarch64 -nographic\
   -M virt,secure=on,virtualization=on,gic-version=3 \
   -cpu cortex-a53 -smp 4 -m 4G\
   -bios $TOOLS_DIR/flash.bin \
   -device loader,file="$BUILD_BAO_DIR/bao.bin",addr=0x50000000,force-raw=on\
   -device virtio-net-device,netdev=net0 \
   -netdev user,id=net0,hostfwd=tcp:127.0.0.1:5555-:22\
   -device virtio-serial-device -chardev pty,id=serial3 \
   -device virtconsole,chardev=serial3
```

Now, you should see TF-A and U-boot initialization messages. After, set up
connections and jump into the world of Bao. QEMU will reveal the
pseudoterminals where it placed the virtio serial. Here's an example:

```sh
char device redirected to /dev/pts/4 (label serial3)
```

To make the connection, open a fresh terminal window and establish a connection
to the specified pseudoterminal. Here's how:

```sh
screen /dev/pts/4
```

Finally, make u-boot jump to where the bao image was loaded:
```sh
go 0x50000000
```
And you should have an output as follows:

[asciinema-video]: https://asciinema.org/a/Kn7yUQ6V7Hy1sB6OAsAjOCthK
[asciinema-image]: https://asciinema.org/a/Kn7yUQ6V7Hy1sB6OAsAjOCthK.png

[![asciicast][asciinema-image]][asciinema-video]


When you want to leave QEMU press `Ctrl-a` then `x`.

## 5. Well, Maybe the Setup Was Not Perfect...

As we continue on this thrilling tour, it's time to explore the art of changing
your Bao setup. Mastering the ability to modify your virtual environment opens
up endless possibilities. Don't worry if you encounter a few challenges along
the way; learning through hands-on experience is the key!

In the following sections, we'll walk you through step-by-step instructions to
make various changes to your guests. By the end of this part of the tour,
you'll have a deeper understanding of how the different components interact,
and you'll be confidently making adjustments to suit your needs.

### 5.1 Add a second guest - freeRTOS

In this section, we'll delve into various scenarios and demonstrate how to
configure specific environments using Bao. One of Bao's notable strengths lies
in its flexibility, allowing you to tailor your setup to a range of
requirements.

Let's kick things off by incorporating a second VM running FreeRTOS.

![Init Setup](/img/dual-guest-rtos.svg)

First, we can use the baremetal compiled from the first setup:
```sh
cp $BAREMETAL_SRCS/build/qemu-aarch64-virt/baremetal.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup/baremetal.bin
```

#### 5.1.1. Compile freeRTOS
Then, let's compile our new guest:

```sh
export FREERTOS_SRCS=$ROOT_DIR/freertos
export FREERTOS_PARAMS="STD_ADDR_SPACE=y"

git clone --recursive --shallow-submodules\
    https://github.com/bao-project/freertos-over-bao.git\
    $FREERTOS_SRCS --branch demo
git -C $FREERTOS_SRCS apply $PATCHES_DIR/freeRTOS.patch
make -C $FREERTOS_SRCS PLATFORM=qemu-aarch64-virt $FREERTOS_PARAMS
```

Upon completing these steps, you'll find a binary file in the `FREERTOS_SRCS`
directory, called `free_rtos.bin`. Move the binary file to your build directory
(`BUILD_GUESTS_DIR`):

```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup
cp $FREERTOS_SRCS/build/qemu-aarch64-virt/freertos.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-setup/free-rtos.bin
```

#### 5.1.2. Integrating the new guest

Now, we have both guests needed for our setup. However, there are some steps
required to fit the two VMs on our platform. Let's understand the differences
between the configuration of the first setup and the configuration of the
second setup.

First of all, we need to add the second VM image:

```diff
- VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-setup/baremetal.bin));
+ VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/baremetal.bin));
+ VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/free-rtos.bin));
```

Also, since we now have 2 VMs, we need to change the `vm_list_size` in our
configuration:

```diff
- .vmlist_size = 1,
+ .vmlist_size = 2,
```

Next, we need to think about resources. In the first setup, we assigned 4 vCPUs
to the baremetal. But this time, we need to split the vCPUs between the two
VMs:

```diff
- .cpu_num = 4,
+ .cpu_num = 2,
```

Additionally, we need to include all the configurations of the second VM.
(Details are omitted for simplicity but you can check further details in the
[configuration file](configs/baremetal-freeRTOS.c)):

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

As we've seen, changing the guests includes changing the configuration file.
Therefore, we need to repeat the process of building Bao. First, copy your
configuration file to the working directory with the following commands:

```sh
mkdir -p $mkdir -p $BUILD_BAO_DIR/config
cp -L $ROOT_DIR/configs/baremetal-freeRTOS.c\
    $BUILD_BAO_DIR/config/baremetal-freeRTOS.c
```

Then, you just need to compile it. Please note that the flag `CONFIG` defines
the configuration file to be used on the compilation of Bao!

```sh
make -C $BAO_SRCS\
    PLATFORM=qemu-aarch64-virt\
    CONFIG_REPO=$ROOT_DIR/configs\
    CONFIG=baremetal-freeRTOS\
    CONFIG_BUILTIN=y\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD
```

Upon completing these steps, you'll find a binary file in the `BAO_SRCS`
directory, called `bao.bin`. Move the binary file to your build directory
(`BUILD_BAO_DIR`):

```sh
cp $BAO_SRCS/bin/qemu-aarch64-virt/baremetal-freeRTOS/bao.bin \
    $BUILD_BAO_DIR/bao.bin
```

#### 5.1.4. Ready for launch!

Now, we have everything configured for testing our new setup! Just run the
following command:
```sh
qemu-system-aarch64 -nographic \
  -M virt,secure=on,virtualization=on,gic-version=3 \
  -cpu cortex-a53 -smp 4 -m 4G \
  -bios $TOOLS_DIR/flash.bin \
  -device loader,file="$BUILD_BAO_DIR/bao.bin",addr=0x50000000,force-raw=on \
  -device virtio-net-device,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp:127.0.0.1:5555-:22 \
  -device virtio-serial-device -chardev pty,id=serial3 \
  -device virtconsole,chardev=serial3
```

### 5.2 It was still not perfect right? Let's try out a Linux too

Let's now introduce a third VM running the Linux OS.

![Init Setup](/img/triple-guest.svg)

First, we can re-use our guests from the previous setup:
```sh
mkdir -p $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup
cp $BAREMETAL_SRCS/build/qemu-aarch64-virt/baremetal.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/baremetal.bin
cp $FREERTOS_SRCS/build/qemu-aarch64-virt/freertos.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/freertos.bin
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

Setup and environment variable pointing to the target architecture and platform
specific config to be used by buildroot:

```sh
export LINUX_CFG_FRAG=$(ls $ROOT_DIR/srcs/configs/base.config\
    $ROOT_DIR/srcs/configs/aarch64.config\
    $ROOT_DIR/srcs/configs/qemu-aarch64-virt.config 2> /dev/null)
```

Setup buildroot environment variables:
```sh
export BUILDROOT_SRCS=$LINUX_DIR/buildroot-aarch64-$LINUX_VERSION
export BUILDROOT_DEFCFG=$ROOT_DIR/srcs/buildroot/aarch64.config
export LINUX_OVERRIDE_SRCDIR=$LINUX_SRCS
```

Clone the latest buildroot at the latest stable version
```sh
git clone https://github.com/buildroot/buildroot.git $BUILDROOT_SRCS\
    --depth 1 --branch 2022.11
cd $BUILDROOT_SRCS
```

Use our provided buildroot defconfig, which itselfs points to the a Linux
kernel defconfig and patches and build.

```sh
make defconfig BR2_DEFCONFIG=$BUILDROOT_DEFCFG
make linux-reconfigure all

mv $BUILDROOT_SRCS/output/images/Image\
    $BUILDROOT_SRCS/output/images/Image-qemu-aarch64-virt
```

The device tree for this setup is available in srcs/devicetrees/
qemu-aarch64-virt. For a device tree file named linux.dts define a environment
variable and build:

```sh
export LINUX_VM=linux
dtc $ROOT_DIR/srcs/devicetrees/qemu-aarch64-virt/$LINUX_VM.dts >\
    $LINUX_DIR/linux-build/$LINUX_VM.dtb
```

Wrap the kernel image and device tree blob in a single binary:
```sh
make -j $(nproc) -C $ROOT_DIR/srcs/lloader\
    ARCH=aarch64\
    IMAGE=$BUILDROOT_SRCS/output/images/Image-qemu-aarch64-virt\
    DTB=$LINUX_DIR/linux-build/$LINUX_VM.dtb\
    TARGET=$LINUX_DIR/linux-build/$LINUX_VM
```

Finaly, copy the binary file to the (compiled) guests folder:
```sh
cp $LINUX_DIR/linux-build/$LINUX_VM.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/linux.bin
```


#### 5.2.2 Welcome our new guest!

After building our new guest, it's time to integrate in our setup. You can find
all details in the [configuration file](/configs/baremetal-freeRTOS-linux.c).

 After that, we need to load our guests:
```diff
- VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/baremetal.bin));
- VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-setup/free-rtos.bin));
+ VM_IMAGE(baremetal_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/baremetal.bin));
+ VM_IMAGE(freertos_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/free-rtos.bin));
+ VM_IMAGE(linux_image, XSTR(BAO_WRKDIR_IMGS/guests/baremetal-freeRTOS-linux-setup/linux.bin));
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

#### 5.2.3. Let's rebuild Bao!

As we've seen, changing the guests includes changing the configuration file.
Therefore, we need to repeat the process of building Bao. First, copy your
configuration file to the working directory with the following commands:

```sh
mkdir -p $mkdir -p $BUILD_BAO_DIR/config
cp -L $ROOT_DIR/configs/baremetal-freeRTOS-linux.c\
    $BUILD_BAO_DIR/config/baremetal-freeRTOS-linux.c
```

Then, you just need to compile it:
```sh
make -C $BAO_SRCS\
    PLATFORM=qemu-aarch64-virt\
    CONFIG_REPO=$ROOT_DIR/configs\
    CONFIG=baremetal-freeRTOS-linux\
    CONFIG_BUILTIN=y\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD
```

Upon completing these steps, you'll find a binary file in the `BAO_SRCS`
directory, called `bao.bin`. Move the binary file to your build directory
(`BUILD_BAO_DIR`):

```sh
cp $BAO_SRCS/bin/qemu-aarch64-virt/baremetal-freeRTOS-linux/bao.bin \
    $BUILD_BAO_DIR/bao.bin
```

#### 5.2.4. Ready to go!

With all the pieces in place, it's time to launch QEMU and behold the fruits of
your labor. The moment of truth awaits, so let's dive right in:

```sh
qemu-system-aarch64 -nographic \
  -M virt,secure=on,virtualization=on,gic-version=3 \
  -cpu cortex-a53 -smp 4 -m 4G \
  -bios $TOOLS_DIR/flash.bin \
  -device loader,file="$BUILD_BAO_DIR/bao.bin",addr=0x50000000,force-raw=on \
  -device virtio-net-device,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp:127.0.0.1:5555-:22 \
  -device virtio-serial-device -chardev pty,id=serial3 \
  -device virtconsole,chardev=serial3
```

The platform's first available UART is assigned to the baremetal and the
FreeRTOS guests. In this manner, you can connect to them using the following
command:

```sh
screen /dev/pts/4
```

The Linux guest is also accessible via ssh at the static address 192.168.42.15.
The password for root is root.

## 5.3 Guests must socialize, right?

In certain scenarios, it's imperative for guests to establish a communication
channel. To accomplish this, we'll utilize shared memory and Inter-Process
Communication (IPC) mechanisms, allowing the Linux VM to seamlessly interact
with the system.

![Init Setup](/img/triple-guest-shmem.svg)


### 5.3.1. Add Shared Memory and IPC to our guest
Let's kick off by integrating an IPC into Linux. To do this, we'll make the
necessary additions to the Linux device-tree. For simplicity, the
[linux-shmem.dts](/configs/baremetal-freeRTOS-linux-shmem.c) file already
encompasses the following changes:

```diff
+    bao-ipc@f0000000 {
+        compatible = "bao,ipcshmem";
+        reg = <0x0 0xf0000000 0x0 0x00010000>;
+		read-channel = <0x0 0x2000>;
+		write-channel = <0x2000 0x2000>;
+        interrupts = <0 52 1>;
+		id = <0>;
+    };
```

Now, let's generate the updated device tree:

```sh
export LINUX_VM=linux-shmem
dtc $ROOT_DIR/srcs/devicetrees/qemu-aarch64-virt/$LINUX_VM.dts >\
    $LINUX_DIR/linux-build/$LINUX_VM.dtb
```

:warning: To correctly introduce these changes, you need to ensure that you
applied the patch to Linux, as described [before](#521-build-linux-guest).

Bundle the kernel image and device tree blob into a single binary:
```sh
make -j $(nproc) -C $ROOT_DIR/srcs/lloader\
    ARCH=aarch64\
    IMAGE=$BUILDROOT_SRCS/output/images/Image-qemu-aarch64-virt\
    DTB=$LINUX_DIR/linux-build/$LINUX_VM.dtb\
    TARGET=$LINUX_DIR/linux-build/$LINUX_VM
```

Finally, move the binary file to the (compiled) guests folder:
```sh
cp $LINUX_DIR/linux-build/$LINUX_VM.bin \
    $BUILD_GUESTS_DIR/baremetal-freeRTOS-linux-setup/$LINUX_VM.bin
```

### 5.3.2. Rebuild Bao

Given that you've modified one of the guests, it's now essential to rebuild
Bao:
```sh
mkdir -p $mkdir -p $BUILD_BAO_DIR/config
cp -L $ROOT_DIR/configs/baremetal-freeRTOS-linux.c\
    $BUILD_BAO_DIR/config/baremetal-freeRTOS-linux.c
```

Subsequently, compile it:
```sh
make -C $BAO_SRCS\
    PLATFORM=qemu-aarch64-virt\
    CONFIG_REPO=$ROOT_DIR/configs\
    CONFIG=baremetal-freeRTOS-linux\
    CONFIG_BUILTIN=y\
    CPPFLAGS=-DBAO_WRKDIR_IMGS=$SETUP_BUILD
```

Upon successful completion, you'll locate a binary file named bao.bin in the
``BAO_SRCS`` directory. Move it to your build directory (``BUILD_BAO_DIR``):

```sh
cp $BAO_SRCS/bin/qemu-aarch64-virt/baremetal-freeRTOS-linux/bao.bin \
    $BUILD_BAO_DIR/bao.bin
```


### 5.3.3. Run Our Setup

Now, you're ready to execute the final setup:

```sh
qemu-system-aarch64 -nographic \
  -M virt,secure=on,virtualization=on,gic-version=3 \
  -cpu cortex-a53 -smp 4 -m 4G \
  -bios $TOOLS_DIR/flash.bin \
  -device loader,file="$BUILD_BAO_DIR/bao.bin",addr=0x50000000,force-raw=on \
  -device virtio-net-device,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp:127.0.0.1:5555-:22 \
  -device virtio-serial-device -chardev pty,id=serial3 \
  -device virtconsole,chardev=serial3
```

If all went according to plan, you should be able to spot the IPC on Linux by
running the following command:
```sh
ls /dev
```

You'll see your IPC as depicted in the following image:

![Init Setup](/img/shmem-IPC.png)

From here, you can employ the IPC on Linux to dispatch messages to FreeRTOS by
writing to ``/dev/baoipc0``:
```sh
echo "Hello, Bao!" > /dev/baoipc0
```

Or retrieve the latest FreeRTOS message by reading from ``/dev/baoipc0``:
```sh
cat /dev/baoipc0
```
