
# Micro RTSP Server for jetson Nano

Este é um servidor RTSP básico implementado em C utilizando GStreamer. O objetivo do projeto é capturar o vídeo de uma câmera conectada na máquina e disponibilizar o stream via RTSP.

## Estrutura do Projeto

```
.
├── Makefile               # Script para compilar e executar o servidor RTSP
├── micro_rtsp_server      # Executável do servidor RTSP
├── micro_rtsp_server.c    # Código-fonte em C do servidor RTSP
```

## Dependências

Para compilar e executar este projeto, você precisa ter as seguintes dependências instaladas:

- GStreamer 1.0
- GStreamer RTSP Server
- Compilador GCC

No Ubuntu, você pode instalar essas dependências executando o seguinte comando:

```bash
sudo apt-get install gstreamer1.0-tools gstreamer1.0-plugins-good \
gstreamer1.0-rtsp gstreamer1.0-libav libgstrtspserver-1.0-dev
```

## Como Compilar e Executar

1. **Clone o repositório ou copie os arquivos para sua máquina.**
   
2. **Compile o código:**

   Use o `Makefile` para compilar o servidor RTSP:

   ```bash
   make build
   ```

   Isso irá gerar o executável `micro_rtsp_server`.

3. **Execute o servidor:**

   Para iniciar o servidor RTSP, execute o comando:

   ```bash
   make run
   ```

   Isso irá iniciar o servidor RTSP e ele mostrará no terminal o endereço onde o stream pode ser acessado, algo como:

   ```
   Stream disponível em rtsp://<seu-ip-local>:8554/mjpeg/1
   ```

## Como Acessar o Stream

Uma vez que o servidor RTSP esteja em execução, você pode acessar o stream de vídeo com qualquer player que suporte RTSP, como VLC ou OpenCV.

### Usando VLC

Abra o VLC e acesse o menu **Mídia > Abrir Fluxo de Rede**. Insira o URL RTSP fornecido pelo servidor, por exemplo:

```
rtsp://192.168.0.26:8554/mjpeg/1
```

### Usando OpenCV em Python

Aqui está um exemplo de como acessar o stream usando OpenCV:

```python
import cv2

cap = cv2.VideoCapture("rtsp://192.168.0.26:8554/mjpeg/1")

while True:
    ret, frame = cap.read()
    if not ret:
        print("Erro ao acessar o stream")
        break

    cv2.imshow('RTSP Stream', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```

## Estrutura do Código

### `micro_rtsp_server.c`

Este arquivo contém o código-fonte do servidor RTSP. Ele utiliza a biblioteca GStreamer para capturar o vídeo da câmera `/dev/video0` e transmiti-lo via RTSP. O código foi organizado para:

- Capturar vídeo da câmera usando `v4l2src`.
- Codificar o vídeo em MJPEG com `jpegenc`.
- Transmitir o stream através de `rtpjpegpay` para enviar os pacotes RTP.

### `Makefile`

O `Makefile` contém os seguintes comandos:

- `make build`: Compila o código-fonte e gera o executável.
- `make run`: Compila o código (se necessário) e executa o servidor RTSP.
- `make clean`: Remove o executável gerado (`micro_rtsp_server`).

## Como Personalizar

Você pode personalizar o código para:

1. **Alterar a resolução do vídeo**: Modifique os parâmetros `WIDTH` e `HEIGHT` no código-fonte para ajustar a resolução.
2. **Alterar a taxa de quadros**: Modifique a constante `FRAMERATE` no código-fonte para alterar a taxa de quadros.
3. **Adicionar logs**: O código já possui logs para monitorar conexões e envio de frames. Para adicionar mais logs, você pode utilizar as funções `g_print()` para enviar mensagens ao terminal.

## Problemas Conhecidos

- **Streaming travado ou em buffer**: Se você observar o vídeo travado ou bufferizando constantemente, pode ser necessário ajustar a taxa de frames ou a qualidade da codificação.
- **Erro de negociação de formato**: Certifique-se de que a sua câmera suporte o formato e a resolução especificados no código-fonte.

## Licença

Este projeto é de código aberto e está disponível sob a licença [MIT](https://opensource.org/licenses/MIT).
```

### Explicação:

1. **Descrição Geral**: Explica o que o projeto faz e para que serve.
2. **Estrutura do Projeto**: Um resumo dos arquivos e pastas.
3. **Dependências**: Como instalar as bibliotecas necessárias para rodar o projeto.
4. **Instruções de Compilação e Execução**: Orienta sobre como compilar e rodar o servidor RTSP.
5. **Acesso ao Stream**: Explica como acessar o stream via VLC ou OpenCV.
6. **Personalização**: Informa como modificar a resolução, taxa de quadros e logs.
7. **Problemas Conhecidos**: Algumas dicas para problemas comuns.
8. **Licença**: Inclui a licença do projeto.

Você pode ajustar e expandir esse `README` conforme as suas necessidades.
