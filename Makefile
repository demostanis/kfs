NAME = kfs

ARCH = i386

CC = build/tcc-$(TCC_VERSION)/$(ARCH)-tcc
LIBTCC = build/tcc-$(TCC_VERSION)/$(ARCH)-libtcc1.a
CFLAGS = -Wall -Werror -Wextra -Iinc -g
LDFLAGS = -nostdlib -Tlinker.ld \
		  -m elf_$(ARCH) --gc-sections
QEMU_ARGS =

AS = nasm
ASFLAGS = -felf32 -g

OBJS = $(addprefix o/,\
	   kernel.o video.o printk.o\
	   gdt.o lib.o serial.o\
	   pmem.o vmem.o kmalloc.o\
	   kmain.o\
	   )

# build everything
all: build/tcc $(NAME) release

## i have no idea how to make this multiline...
# show this help
help:
	@perl -e ' $$p=0; while(<>){ $$p=1 if/^# .*/; print $$_=~s/(.+):( |$$).*/\x1b[0;1;2m\$$ make $$1\x1b[0m\n/r=~s/^# /\x1b[1m# \x1b[0;2m/r if $$p; $$p=0 if /:( |$$)/; } ' Makefile | $${PAGER:-less} -R

TEST =
# run tests, with or without KVM
tests tests_nokvm: CPPFLAGS+=-DRUNTESTS
tests tests_nokvm: ASFLAGS+=-DRUNTESTS
tests tests_nokvm: QEMU_ARGS+=-nographic
tests tests_nokvm: QEMU_ARGS+=-monitor none
tests tests_nokvm: TEST=| ./serial_examiner
tests: re run
tests_nokvm: re run_nokvm

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBTCC)

o/%.o: %.c | o
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< -D__FILENAME__=$(basename $<)

.ONESHELL:
o/kmain.o: kmain.c | o
	rm -f $@ # else *.o will match kmain.o
	tests=$$(nm o/*.o | grep 'ktest_' | awk '{print$$3}' | sort -u)
	( echo 'void ktest(){begin_test();'; \
		echo "$$tests" | sed 's/.*/\
			void &(); &();\
			/g'; \
		echo 'end_test();}' ) | \
		cat $< - | $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ -

o/%.o: %.s | o
	$(AS) $(ASFLAGS) -o $@ $<

o:
	mkdir -p o

_run: release
	qemu-system-$(ARCH) \
		-serial stdio -cdrom $(NAME).iso \
		$(QEMU_ARGS) $(TEST)

# run inside QEMU
run: QEMU_ARGS+=-enable-kvm
run: _run

# run without KVM (used for GDB)
run_nokvm: _run

TCC_VERSION = 0.9.27
TCC_ARCHIVE = tcc-$(TCC_VERSION).tar.bz2
TCC_SRC = http://download.savannah.gnu.org/releases/tinycc/$(TCC_ARCHIVE)
build/tcc: build/tcc-$(TCC_VERSION)/$(ARCH)-tcc build/tcc-$(TCC_VERSION)/$(ARCH)-libtcc1.a
build/tcc-$(TCC_VERSION)/$(ARCH)-tcc build/tcc-$(TCC_VERSION)/$(ARCH)-libtcc1.a:
	mkdir -p build
	curl -sLo build/$(TCC_ARCHIVE) $(TCC_SRC)
	tar -C build -xf build/$(TCC_ARCHIVE)
	( cd build/tcc-$(TCC_VERSION); \
		patch -p0 < ../../tcc-generate-proper-debug-symbols.patch && \
		./configure --prefix=$@ && \
		make -j$$(nproc) cross-$(ARCH) )

GRUB_MODULES = iso9660 multiboot \
			   normal part_acorn part_amiga part_apple \
			   part_bsd part_dfly part_dvh part_gpt part_msdos \
			   part_plan part_sun part_sunpc # part_* modules are
											 # always loaded by GRUB,
											 # despite being unneeded
GRUB_EXCLUDED_FILES = boot/grub/x86_64-efi \
					  boot/grub/i386-efi \
					  System mach_kernel \
					  efi efi.img
# create a bootable image
release: $(NAME).iso
$(NAME).iso: $(NAME)
	grub-mkrescue \
		--themes="" \
		--install-modules "$(GRUB_MODULES)" \
		-o $@ $(NAME) rootfs -- \
		-rm_r $(GRUB_EXCLUDED_FILES)

# remove object files
clean:
	$(RM) $(OBJS)

# remove object files and the kernel executable
fclean: clean
	$(RM) $(NAME)

# remove the cross compiler
clear:
	$(RM) -r build

# rebuild the kernel
re: fclean all

# run QEMU for GDB usage
# this should be run before `make gdb`
run_gdb: QEMU_ARGS+=-S -s
run_gdb: run_nokvm

# run GDB
gdb:
	gdb -x .gdb.script

BOCHS = bochs
BOCHS_ARGS = 
# run in bochs emulator
bochs: release
	$(BOCHS) \
		'boot: cdrom' \
		'ata0-slave: type=cdrom, path=kfs.iso, status=inserted' \
		$(BOCHS_ARGS)

.PHONY: all help _run run run_nokvm tests build/tcc release clean fclean clear re run_gdb gdb bochs
