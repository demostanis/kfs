NAME = kfs

ARCH = i386

CC = build/tcc-$(TCC_VERSION)/$(ARCH)-tcc
CFLAGS = -Wall -Werror -Wextra
LDFLAGS = -nostdlib -Wl,-Ttext,0x100000

AS = nasm
ASFLAGS = -felf32

OBJS = kernel.o kmain.o

all: build/tcc $(NAME) release

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

release: $(NAME).iso
$(NAME).iso: $(NAME)
	grub-mkrescue -o $@ $(NAME) rootfs

run: release
	qemu-system-$(ARCH) -cdrom $(NAME).iso

TCC_VERSION = 0.9.27
TCC_ARCHIVE = tcc-$(TCC_VERSION).tar.bz2
TCC_SRC = http://download.savannah.gnu.org/releases/tinycc/$(TCC_ARCHIVE)
build/tcc: build/tcc-$(TCC_VERSION)
build/tcc-$(TCC_VERSION):
	mkdir -p build
	curl -sLo build/$(TCC_ARCHIVE) $(TCC_SRC)
	tar -C build -xf build/$(TCC_ARCHIVE)
	patch -p0 <./tcc-segfault.patch
	( cd build/tcc-$(TCC_VERSION); \
		./configure --prefix=$@ && \
		make -j$$(nproc) cross-$(ARCH) )

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

clear:
	$(RM) -r build

re: fclean all

.PHONY: all
