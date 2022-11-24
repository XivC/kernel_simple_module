obj-m += read_pci.o
obj-m += read_page.o

all: user
	make -C /lib/modules/$(shell uname -r)/build M=${PWD} modules

	sudo insmod read_pci.ko
	sudo insmod read_page.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

	sudo rmmod read_pci
	sudo rmmod read_page
	sudo rm user_read_page user_read_pci_dev

user:
	gcc user_read_page.c -o user_read_page
	gcc user_read_pci_dev.c -o user_read_pci_dev
