# Variables
CC = gcc                   # Compilador
CFLAGS = -g -Wall -Wextra -std=c99 -g  # Flags para el compilador
SRC = main.c functions.c CMDlist.c HistoryList.c CommandList.c  # Archivos fuente
OBJ = $(SRC:.c=.o)         # Archivos objeto
TARGET = exe       # Nombre del ejecutable

# Regla por defecto
all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para crear los archivos objeto
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c functions.h HistoryList.h CMDlist.h CommandList.h
functions.o: functions.c functions.h
HistoryList.o: HistoryList.c HistoryList.h
CMDlist.o: CMDlist.c CMDlist.h
CommandList.o: CommandList.c CommandList.h

# Limpiar archivos generados
clean:
	rm -f $(OBJ) $(TARGET)

# Regla para mostrar ayuda
help:
	@echo "Makefile para compilar el programa."
	@echo "Comandos disponibles:"
	@echo "  make           Compila el programa."
	@echo "  make clean     Elimina los archivos objeto y el ejecutable."
	@echo "  make help      Muestra esta ayuda."

.PHONY: all clean help  # Evita conflictos con archivos llamados clean o help
