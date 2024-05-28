# Makefile para compilar e executar o jogo

# Nome do executável
TARGET = jogo

# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Bibliotecas necessárias
LIBS = -lm

# Arquivos fonte
SOURCES = main.c timer.c screen.c keyboard.c

# Comando padrão
all: $(TARGET)

# Regra de compilação
$(TARGET): $(SOURCES)
		$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

# Regra para limpar os arquivos gerados
clean:
		rm -f $(TARGET)

# Regra para rodar o jogo
run: $(TARGET)
		./$(TARGET)

# Regra para compilar e rodar o jogo
play: all run

.PHONY: clean run play
