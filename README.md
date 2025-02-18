# "Hello world"? We prefer "Hello Bao!"

Welcome to the Bao Hypervisor! Get ready for an interactive journey as we
explore the world of Bao together. Whether you're a experience Bao user or a
newcomer, this tour is designed to give you a practical introduction to the Bao
hypervisor.

If you're already familiar with Bao or want to dive into specific setups
provided by our team, feel free to skip ahead to the [Bao demos 
repository](https://github.com/bao-project/bao-demos).

In this guide, we will take a tour of the different components required to
build a setup using the Bao hypervisor and learn how the different components
interact. For this purpose, the guide contains the following topics:

- A **getting started** section that helps users to prepare the environment
  where we will build the target setups. We also provide extra detailed
  documentation notes regarding some implementation aspects of the hypervisor;
- An **initial setup** section that explores the different components of the
  system and gets the first practical example of this guide;
- An **interactive tutorial on changing the guests** running on top of Bao;
- A **practical example** of changing the setup running;
- An example of **how different guests can coexist and interact** with each
  other;

## 1. Getting Started

In this section, we'll guide you through preparing a development environment to
build Bao. Don't worry; we'll provide you with helpful pointers to Bao's
implementation details in case you want to explore any further.

### 1.1 Recommended Operating System: Linux (e.g., Ubuntu 22.04)
We recommend using a Linux-based operating system to make the most of this
tutorial and the Bao hypervisor. While the instructions may work on other
platforms, this guide is set up on top of a Linux-based machine, specifically
Ubuntu 22.04. This will ensure compatibility and an optimal experience
throughout the tour.

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

---

Now that your environment is set up, it's time to proceed with architecture-
specific instructions. Please follow the guide that corresponds to your target
architecture:

ARM64 (aarch64) → [aarch64.md](qemu-aarch64.md)

RISC-V (riscv64) → [riscv64.md](qemu-riscv64.md)

Click on the corresponding link to proceed the tutorial! 