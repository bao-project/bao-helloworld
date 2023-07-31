# "Hello world"? We prefer "Hello Bao!"

Welcome to the Bao Hypervisor! Get ready for an interactive journey as we explore the world of Bao together. Whether you're a seasoned Bao user or a newcomer, this tour is designed to give you a practical and enthusiastic introduction to our powerful hypervisor.

If you're already familiar with Bao or want to dive into specific setups provided by our team, feel free to skip ahead to the Bao demos section.

In this guide, we will take a tour of the different components required to build a setup using the Bao hypervisor and learn how the different components interact. For this purpose, the guide contains the following topics:

- A **getting started** to help users on preparing the environment to build the setup and also some pointers to documentations of Bao (in case you want to go deeper in any detail);

- An **initial setup** for giving the first steps on this tour. This section aims to explore the different components of the system and get the first practical example of this guide;

- An **interactive tutorial on changing the guests** running on top of Bao;

- A **practical example** of changing the setup running;

- An example of **how different guests can coexist and interact** with each other;

## Getting Started

Before we dive into the thrilling aspects of Bao, let's make sure you're all set up and ready to go. In this section, we'll guide you through preparing your environment to build the setup. Don't worry; we'll provide you with helpful pointers to Bao's documentation in case you want to explore any details further.

### Recommended Operating System: Linux (e.g., Ubuntu 22.04 or older versions)
To make the most of this tutorial and the Bao hypervisor, we recommend using a Linux-based operating system. While the instructions may work on other platforms, our focus is on Linux, specifically Ubuntu 22.04 or older versions. This will ensure compatibility and an optimal experience throughout the tour.

### Installing Required Dependencies
Before we can dive into the world of Bao, we need to install several dependencies to enable a seamless setup process. Open your terminal and run the following command to install the necessary packages:

```console
sudo apt install build-essential bison flex git libssl-dev ninja-build \
    u-boot-tools pandoc libslirp-dev pkg-config libglib2.0-dev libpixman-1-dev \
    gettext-base curl xterm cmake python3-pip
```

This command will install essential tools and libraries required for building and running Bao.
Next, we need to install some Python packages. Execute the following command to do so:

```console
pip3 install pykwalify packaging pyelftools
```

### Ensuring Sufficient Free Space

Please be aware that sufficient free space is crucial for this journey, especially due to the Linux image that will be built for the Linux guest VM. To ensure a smooth experience and avoid any space-related issues, we recommend having at least 20GB of free space available on your system.
With your environment set up and all the dependencies installed, you're now ready to dive into the world of Bao hypervisor and create your virtualized wonders!

---

## Initial setup - Taking the First Steps!

Now that you're geared up, it's time to take the first steps on this tour. In the Initial Setup section, we'll explore the different components of the system and walk you through a practical example to give you a solid foundation.

![Init Setup](/img/init-setup.svg)

## Build Guests - Building Your First Bao Guests

We'll start by building your first Bao guests. You'll get hands-on experience creating both a Baremetal Guest and a Linux Guest. Let's get those virtual machines up and running!

### 1. Baremetal Guest

![Init Setup](/img/init-setup-bare.svg)

### 2. Linux Guest

## Build Bao Hypervisor - Laying the Foundation
Next up, we'll guide you through building the Bao Hypervisor itself. This critical step forms the backbone of your virtualization environment.

## Build Firmware - Powering Up Your Setup

No tour is complete without a firmware build. We'll show you how to get your setup up and running with our straightforward firmware-building process.


## Let's Try It Out! - Unleash the Power

Now that everything is set up, let's put it to the test! We'll show you how to fire up your newly created virtual machines and experience the magic of Bao firsthand.

## Well, Maybe the Setup Was Not Perfect...

As we continue on this thrilling tour, it's time to explore the art of changing your Bao setup. Mastering the ability to modify your virtual environment opens up endless possibilities. Don't worry if you encounter a few challenges along the way; learning through hands-on experience is the key!

In the following sections, we'll walk you through step-by-step instructions to make various changes to your guests. By the end of this part of the tour, you'll have a deeper understanding of how the different components interact, and you'll be confidently making adjustments to suit your needs.

## Different scenario? Different setup!

In this section, we'll dive into different scenarios and explore how to set up specific environments with Bao. Flexibility is one of Bao's strengths, and we'll guide you through adapting your setup to suit various needs.

## Guests must socialize, right?

### Let's Share Content! - Memory Sharing Made Easy
Sharing files between guests is essential for seamless collaboration. We'll demonstrate how to set up file sharing and make it a breeze for your virtual machines.

### Teaching Guests to Be Polite - Networking Etiquette
Just like in the real world, networking etiquette matters in the virtual world too! We'll guide you through setting up networking between guests, enabling smooth communication while maintaining order and politeness.