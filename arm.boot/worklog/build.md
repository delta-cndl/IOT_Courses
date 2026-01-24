# Understanding the build

We are concerned here about the "build part" of the makefile, the
classical targets `all` and `clean`.

Notice the typical structure of a makefile for embedded systems.

- A few variables that can be configured
  - which QEMU to use
  - which toolchain to use
  - for which board is the software built
- A self-configuring part

Look at the self-configuring part:

    ifeq ($(BOARD),versatile)
      ...
    endif

As you can see, once the board is know, the rest of the options and variables are set, both for the GNU toolchain and for QEMU. 

Notice that the build directory is dependent on the board:

    BUILD=build/$(BOARD)

Indeed, such makefile may build for different targets.    
Notice the target `clean-all`.

**Question:** what should the configuration of the GNU toolchain and the configuration of the QEMU be correlated? 

==> They must be correlated because QEMU executes exactly the machine that the toolchain targets.

**Question:** how are they correlated? What options need to be the same or similar?

==> They are correlated by targeting the same hardware characteristics: the CPU type, the machine/board model, and the memory size.
The compiler options (e.g. -mcpu=cortex-a8, linker script, memory assumptions) must match the QEMU options (-cpu cortex-a8, -M versatilepb, -m MEMSIZE), so that the generated code corresponds exactly to the machine emulated by QEMU.

An crucial part of the makefile is the linker script.
Look at the correct linker script for the board `versatile`.

**Question:** explain the different sections

==> The linker script defines the memory layout of the target board by placing code (.text), constants (.rodata), initialized data (.data), uninitialized data (.bss), and runtime areas (stack/heap) at precise addresses matching the hardware.
the different sections are: _start, _etext _sdata, _edata_sbss, _ebss _stack_top

**Question:** explain why the linker script is "versatile" specific?
**Question:** make sure they are compatible with the memory map

Go check the memory map in the documentation:

- Versatile Application Baseboard for ARM926EJ-S User Guide
- Programmer’s Reference, section 4.1, « Memory Map » (page 140)

**Question:** is the amount of SDRAM memory in the makefile correct? 
**Question:** are the sections in the right place?

**Question:** why are the different flags given to the compiler?

In particular:
- what are these flags?
    ```make 
    -D$(CPU) -DMEMORY="($(MEMSIZE)*1024)
    ```
- what are these flags?
    ```make 
    -nostdlib -ffreestanding
    ```
- what are these flags
    ```make 
    -MT $@ -MMD -MP -MF $(BUILD)/$*.d
    ```
- what is the line?
    ```make 
    -include $(wildcard $(BUILD)/*.d)
    ```

**Question:** why are the different flags given to the linker?

```make
  -nostdlib -static
```

**Question:** what is the "-g" flag given to both the compiler and linker?
**Question:** is the "-g" flag necessary to execute the code?
