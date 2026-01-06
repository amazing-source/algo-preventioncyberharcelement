# Makefile pour le detecteur de cyberharcelement

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = -lcurl -ljson-c

# Fichiers sources
SRCS = main.c api_client.c comment_generator.c harassment_detector.c stats.c
OBJS = $(SRCS:.c=.o)
TARGET = cyberharcelement_detector

# Cible par defaut
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

# Reconstruction complete
rebuild: clean all

.PHONY: all clean rebuild
