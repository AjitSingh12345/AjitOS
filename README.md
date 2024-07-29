# AjitOS

AjitOS is a minimal operating system designed to be run on any x64/x86 system. It includes basic command-line interface functionality, FAT filesystem support, and interrupt handling.

## Getting Started

These instructions will help you set up and run AjitOS on your local machine.

### Prerequisites

To build and run AjitOS, you need the following tools:
- [NASM](https://www.nasm.us/) (Netwide Assembler)
- [GCC Cross Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) for i686-elf
- [QEMU](https://www.qemu.org/) (Quick Emulator)
- [mtools](https://www.gnu.org/software/mtools/)

Install the necessary dependencies:

```sh
sudo apt-get update
sudo apt-get install nasm gcc qemu mtools
```

Additionally, you need to set up a GCC cross-compiler for i686-elf. Follow the instructions on the [OSDev Wiki](https://wiki.osdev.org/GCC_Cross-Compiler) to install the cross-compiler.

### Building AjitOS

1. Clone the repository:

    ```sh
    git clone https://github.com/yourusername/AjitOS.git
    cd AjitOS
    ```

2. Build the project:

    ```sh
    make clean
    make
    ```

### Running AjitOS

1. Run AjitOS using QEMU:

    ```sh
    ./run.sh
    ```

### Demo

[![Watch the AjitOS Demo](https://via.placeholder.com/150)](file:///C:/Users/asing/Videos/Captures/AjitOS%20Demo.mp4)
