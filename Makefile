NAME = kfs

ARCH = i386

CC = build/tcc-$(TCC_VERSION)/$(ARCH)-tcc
CFLAGS = -Wall -Werror -Wextra
LDFLAGS = -nostdlib -Tlinker.ld \
		  -m elf_$(ARCH) --gc-sections
QEMU_ARGS =

AS = nasm
ASFLAGS = -felf32

OBJS = kernel.o video.o printk.o gdt.o lib.o serial.o kmain.o

all: build/tcc $(NAME) release

MAYBE_TEST = cat

tests: CPPFLAGS+=-DRUNTESTS
tests: ASFLAGS+=-DRUNTESTS
tests: QEMU_ARGS+=-nographic
tests: QEMU_ARGS+=-monitor none
tests: MAYBE_TEST=./test
tests: re run

$(NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $< -D__FILENAME__="$$(basename "$<" .c)"

.ONESHELL:
kmain.o: kmain.c
	tests=$$(nm *.o | grep 'ktest_' | awk '{print$$3}' | sort -u)
	( echo 'void ktest(){'; \
		echo "$$tests" | sed 's/.*/void &();&();/g'; \
		echo '}' ) | \
		cat $< - | $(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ -

run: release
	qemu-system-$(ARCH) -enable-kvm \
		-serial stdio -cdrom $(NAME).iso \
		$(QEMU_ARGS) | $(MAYBE_TEST)

TCC_VERSION = 0.9.27
TCC_ARCHIVE = tcc-$(TCC_VERSION).tar.bz2
TCC_SRC = http://download.savannah.gnu.org/releases/tinycc/$(TCC_ARCHIVE)
build/tcc: build/tcc-$(TCC_VERSION)
build/tcc-$(TCC_VERSION):
	mkdir -p build
	curl -sLo build/$(TCC_ARCHIVE) $(TCC_SRC)
	tar -C build -xf build/$(TCC_ARCHIVE)
	( cd build/tcc-$(TCC_VERSION); \
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
release: $(NAME).iso
$(NAME).iso: $(NAME)
	grub-mkrescue \
		--themes="" \
		--install-modules "$(GRUB_MODULES)" \
		-o $@ $(NAME) rootfs -- \
		-rm_r $(GRUB_EXCLUDED_FILES)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

clear:
	$(RM) -r build

re: fclean all

.PHONY: all run build/tcc release clean fclean clear re
