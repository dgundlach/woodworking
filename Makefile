CC = gcc -Wall -O -g
CFLAGS = -Wall -O -g

BIN = bin
OBJ = obj

DESTDIR =
PREFIX = /usr/local
SYSCONFDIR = /etc
BINDIR = $(PREFIX)/usr/bin
#SYSTEMD = /usr/lib/systemd/system

DRAWERS = $(BIN)/drawers
PANEL = $(BIN)/panel
DRAWERS_OBJS = $(OBJ)/drawers.o $(OBJ)/gcd.o $(OBJ)/measurement.o
DRAWERS_LIBS = -lm
PANEL_OBJS = $(OBJ)/panel.o $(OBJ)/part.o $(OBJ)/gcd.o $(OBJ)/measurement.o
PANEL_LIBS = -lm

TARGETS = $(DRAWERS) $(PANEL)

all : $(TARGETS)

install: $(TARGETS)
	install -d -m 0755 $(DESTDIR)/$(BINDIR)
	install -m 0755 $(TARGETS) $(DESTDIR)/$(SBINDIR)

clean:
	rm -f $(OBJ)/*.o $(TARGETS)

$(DRAWERS): $(DRAWERS_OBJS)
	$(CC) -o $@ $^ $(DRAWERS_LIBS)

$(PANEL): $(PANEL_OBJS)
	$(CC) -o $@ $^ $(PANEL_LIBS)

$(OBJ)/%.o: %.c
	$(CC) -o $@ -c $^
