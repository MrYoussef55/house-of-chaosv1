# ============================================================
#  Makefile — Merged Menu + Game project
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 \
          $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs) \
          -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

TARGET  = game

# ---- source files ----
SRCS =  main.c       \
        game.c       \
        game_multi.c \
        menu.c       \
        menua.c      \
        menus.c      \
        options.c    \
        validation.c \
        button.c     \
        playeri.c    \
        enemiei.c    \
        collision.c  \
        minimap.c    \
        room.c       \
        video.c      \
        enigme1.c    \
        enigme_run.c
# NOTE: logic.c is excluded — it belongs to the older split-screen kitchen game
# and its symbols (InitMinimap, CollisionAABB, etc.) clash with minimap.c / collision.c

OBJS = $(SRCS:.c=.o)

# ---- rules ----
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
