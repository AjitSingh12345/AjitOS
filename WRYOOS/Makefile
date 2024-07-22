gpp_params = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
as_params = --32
ld_params = -melf_i386

objects = loader.o kernel.o

%.o: %.cpp
	g++ $(gpp_params) -o $@ -c $<

%.o: %.s
	as $(as_params) -o $@ $<

myKernel.bin: linker.ld $(objects)
	ld $(ld_params) -T $< -o $@ $(objects)

install: myKernel.bin
	sudo cp $< /boot/myKernel.bin

myKernel.iso: myKernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "AjitOS" {' >> iso/boot/grub/grub.cfg
	echo ' multiboot /boot/myKernel.bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo ' }' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run myKernel.iso:
	VirtualBox --startvm "AjitOS" &