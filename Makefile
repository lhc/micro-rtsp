# Nome do executável
TARGET = micro_rtsp_server

# Compilador
CC = gcc

# Arquivo de origem
SRC = micro_rtsp_server.c

# Flags do compilador
CFLAGS = -Wall -O2 `pkg-config --cflags gstreamer-1.0 gstreamer-rtsp-server-1.0`

# Bibliotecas necessárias
LIBS = `pkg-config --libs gstreamer-1.0 gstreamer-rtsp-server-1.0` -lgstrtsp-1.0

# Regra padrão para buildar o executável
build: $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Regra para rodar o programa
run: build
	./$(TARGET)

# Limpar arquivos gerados
clean:
	rm -f $(TARGET)

