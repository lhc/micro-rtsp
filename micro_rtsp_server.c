#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define WIDTH 640
#define HEIGHT 480
#define FRAMERATE 30

// Estrutura para armazenar o estado do cliente
typedef struct {
    guint frame_count;
    gchar *client_ip;
} ClientData;

// Função: Obter IP local da máquina
char* obter_ip_local() {
    char *ip = (char*)malloc(16);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Erro ao criar socket");
        return NULL;
    }

    const char* google_dns_ip = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns_ip);
    serv.sin_port = htons(dns_port);

    int err = connect(sock, (const struct sockaddr*)&serv, sizeof(serv));
    if (err == -1) {
        perror("Erro ao conectar");
        close(sock);
        return NULL;
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*)&name, &namelen);
    if (err == -1) {
        perror("Erro ao obter nome do socket");
        close(sock);
        return NULL;
    }

    const char *p = inet_ntop(AF_INET, &name.sin_addr, ip, 16);
    if (p == NULL) {
        perror("Erro ao converter endereço IP");
        close(sock);
        return NULL;
    }

    close(sock);
    return ip;
}

// Callback: Quando o frame é enviado
static void on_handle_frame(GstElement *payloader, GstBuffer *buffer, GstRTSPClient *client) {
    ClientData *data = g_object_get_data(G_OBJECT(client), "client-data");

    if (data) {
        data->frame_count++;
        g_print("[ %s ] - Frame ID: %u enviado\n", data->client_ip, data->frame_count);
    }
}

// Callback: Quando um cliente se conecta
void ao_conectar_cliente(GstRTSPClient *client, gpointer user_data) {
    GstRTSPConnection *conn = gst_rtsp_client_get_connection(client);
    if (conn != NULL) {
        const gchar *ip_address = gst_rtsp_connection_get_ip(conn);
        g_print("Cliente conectado do IP: %s\n", ip_address ? ip_address : "desconhecido");

        // Cria uma estrutura para armazenar os dados do cliente
        ClientData *data = g_new0(ClientData, 1);
        data->frame_count = 0;
        data->client_ip = g_strdup(ip_address);

        // Armazena os dados no cliente RTSP
        g_object_set_data_full(G_OBJECT(client), "client-data", data, (GDestroyNotify)g_free);
    } else {
        g_print("Conexao RTSP invalida ou nao encontrada\n");
    }
}

// Callback: Quando o streaming começa
void ao_preparar_midia(GstRTSPMedia *media, gpointer user_data) {
    g_print("Streaming sendo enviado ao cliente...\n");

    // Obtém o elemento responsável por enviar os frames (payloader)
    // GstElement *payloader = gst_bin_get_by_name(GST_BIN(media), "pay0");
    GstElement *media_bin = gst_rtsp_media_get_element(media);
    GstElement *payloader = gst_bin_get_by_name(GST_BIN(media_bin), "pay0");

    if (payloader) {
        // Conecta o callback para ser chamado a cada frame enviado
        g_signal_connect(payloader, "handle-payload", G_CALLBACK(on_handle_frame), user_data);
        g_object_unref(payloader);
    }
}

// Função: Configurar servidor RTSP
GstRTSPServer* configurar_servidor_rtsp() {
    GstRTSPServer *server = gst_rtsp_server_new();
    gst_rtsp_server_set_service(server, "8554");
    g_signal_connect(server, "client-connected", G_CALLBACK(ao_conectar_cliente), NULL);
    return server;
}

// Função: Configurar fábrica de mídia
GstRTSPMediaFactory* configurar_fabrica_midia() {
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
    gst_rtsp_media_factory_set_launch(factory,
        "( "
        "v4l2src device=/dev/video0 ! "
        "video/x-raw, width=(int)" G_STRINGIFY(WIDTH) ", height=(int)" G_STRINGIFY(HEIGHT) ", framerate=(fraction)" G_STRINGIFY(FRAMERATE) "/1 ! "
        "videoconvert ! "
        "jpegenc ! "
        "rtpjpegpay name=pay0 pt=96 "
        ")");
    g_signal_connect(factory, "media-configure", G_CALLBACK(ao_preparar_midia), NULL);
    return factory;
}

// Função: Iniciar loop principal
void iniciar_loop_principal(GMainLoop *loop) {
    g_main_loop_run(loop);
}

// Função principal
int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    GstRTSPServer *server = configurar_servidor_rtsp();
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory *factory = configurar_fabrica_midia();

    gst_rtsp_mount_points_add_factory(mounts, "/mjpeg/1", factory);
    g_object_unref(mounts);

    gst_rtsp_server_attach(server, NULL);

    char *local_ip = obter_ip_local();
    if (local_ip != NULL) {
        g_print("Stream disponivel em rtsp://%s:8554/mjpeg/1\n", local_ip);
        free(local_ip);
    } else {
        g_print("Falha ao obter o IP local\n");
    }

    iniciar_loop_principal(loop);

    return 0;
}

