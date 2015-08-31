TOOL_PREFIX = arm-linux-
CC      = $(TOOL_PREFIX)gcc
STRIP   = $(TOOL_PREFIX)strip

INCDIR =  . -I./inc

CFLAGS = -fno-strict-aliasing -O3 -I$(INCDIR) -DLIBEV

ifeq ($(findstring debug,$(MAKECMDGOALS)), debug)
CFLAGS += -DDEBUG
endif

CFLAGS += -Wall

LDFLAGS = -L./lib -lulibev -lpthread -lm

all:
	$(CC) -c buffer.c $(CFLAGS)
	$(CC) -c fq.c $(CFLAGS)
	$(CC) -c main.c $(CFLAGS)
	$(CC) -o daemon_iprcv daemon.c
	$(CC) -o iprecv buffer.o fq.o main.o $(LDFLAGS)
	$(STRIP) daemon_iprcv
	$(STRIP) iprecv
	chmod 777 daemon_iprcv
	chmod 777 iprecv
	mv daemon_iprcv /mnt/hgfs/dos/update_sd/iprcv
	mv iprecv /mnt/hgfs/dos/update_sd/iprcv

debug:
	make MAKECMDGOALS=debug

clean:
	rm -f *.o
	rm -f *.bak
