// Definiciones para el sistema operativo MS-Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if (defined __WIN32__) || (defined _WIN32)
#define LATINO_BUILD_AS_DLL
#define LATINO_LIB

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_PORT "3000"
#define DEFAULT_BUFLEN 512
#endif
// Fin MS-Windows

#include "latino.h"

#define LIB_WEBSOCKET_NAME "socketc"

// static void lat_conectar(lat_mv *mv)
// {
//     printf("%s\n", "hello world");
// };

static void lat_iniciar_server(lat_mv *mv)
{
    lat_objeto *host = latC_desapilar(mv);
    lat_objeto *puerto = latC_desapilar(mv);

#pragma comment(lib, "Ws2_32.lib")

    // Iniciar Winsock
    WSADATA wsa;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    printf("%s\n", "Initialising Winsock...");
    if (iResult != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        latC_apilar_int(mv, 1);
    }
    printf("%s\n", "Initialised.");

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo fallo: %d\n", iResult);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Crea un socket para escuchar consulta de la conexion.
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Escuchar las consultas
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    SOCKET ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // No longer need server socket
    closesocket(ListenSocket);

    do
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                // return 1;
                latC_apilar_int(mv, 1);
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            // return 1;
            latC_apilar_int(mv, 1);
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    // return 0;
    latC_apilar_int(mv, 0);
}

// CLIENTE

static void lat_iniciar_cliente(lat_mv *mv)
{

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "this is a test";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Validate the parameters
    // if (argc != 2) {
    //     printf("usage: %s server-name\n", argv[0]);
    //     // return 1;
    //     latC_apilar_int(mv, 1);
    // }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        // return 1;
        latC_apilar_int(mv, 1);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            // return 1;
            latC_apilar_int(mv, 1);
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Receive until the peer closes the connection
    do
    {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    // return 0;
    latC_apilar_int(mv, 0);
}

typedef struct LatSocket
{
    SOCKET ListenSocket;
    struct addrinfo *result;
} LatSocket;

static void lat_socket(lat_mv *mv)
{
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")
    // Iniciar Winsock
    WSADATA wsa;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa);
    printf("%s\n", "Initialising Winsock...");
    if (iResult != 0)
    {
        // printf("Failed. Error Code : %d", WSAGetLastError());
        latC_error(mv, "Failed. Error Code : %d", WSAGetLastError());
    }
    printf("%s\n", "Initialised.");
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    lat_objeto *puerto = latC_desapilar(mv);
    lat_objeto *host = latC_desapilar(mv);
    char *str_host = latC_checar_cadena(mv, host);
    char *c_puerto = latC_astring(mv, puerto);
    iResult = getaddrinfo(str_host, c_puerto, &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo fallo: %d\n", iResult);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }
    // Crea un socket para escuchar consulta de la conexion.
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        // printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        latC_error(mv, "Error at socket(): %ld\n", WSAGetLastError());
    }
    LatSocket *latSocket = (LatSocket *)malloc(sizeof(LatSocket));
    latSocket->ListenSocket = ListenSocket;
    latSocket->result = result;
    lat_objeto *lat_socket = latC_crear_cdato(mv, latSocket);
    latC_apilar(mv, lat_socket);
}

static void lat_enlazar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    // Setup the TCP listening socket
    int iResult = bind(latSocket->ListenSocket, latSocket->result->ai_addr, (int)latSocket->result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        // printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(latSocket->result);
        closesocket(latSocket->ListenSocket);
        WSACleanup();
        latC_error(mv, "bind failed with error: %d\n", WSAGetLastError());
    }
}

static void lat_escuchar(lat_mv *mv)
{
    lat_objeto *max_conex = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int int_max_conex = (int)latC_checar_numerico(mv, max_conex);
    if (listen(latSocket->ListenSocket, int_max_conex) == SOCKET_ERROR)
    {
        // printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(latSocket->ListenSocket);
        WSACleanup();
        latC_error(mv, "Listen failed with error: %ld\n", WSAGetLastError());
    }
}

static void lat_aceptar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    SOCKET ClientSocket = INVALID_SOCKET;
    // Accept a client socket
    ClientSocket = accept(latSocket->ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        // printf("accept failed: %d\n", WSAGetLastError());
        closesocket(latSocket->ListenSocket);
        WSACleanup();
        latC_error(mv, "accept failed: %d\n", WSAGetLastError());
    }
    // No longer need server socket
    // closesocket(latSocket->ListenSocket);
    LatSocket *latSocketClient = (LatSocket *)malloc(sizeof(LatSocket));
    latSocketClient->ListenSocket = ClientSocket;
    latSocketClient->result = latSocket->result;
    lat_objeto *client_socket = latC_crear_cdato(mv, latSocketClient);
    latC_apilar(mv, client_socket);
}

static void lat_recv(lat_mv *mv)
{
    printf("lat_recv\n");
    lat_objeto *tam_buffer = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int int_tam_buffer = (int)latC_checar_numerico(mv, tam_buffer);
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iSendResult = 0;
    int iResult = recv(latSocket->ListenSocket, recvbuf, recvbuflen, 0);
    printf("iResult = %i\n", iResult);
    do
    {
        if (iResult > 0)
        {
            printf("Bytes received: %d\n", iResult);
            printf("received: %s\n", recvbuf);
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
        }
        else
        {
            // printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(latSocket->ListenSocket);
            WSACleanup();
            latC_error(mv, "recv failed with error: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);
}

static void lat_enviar(lat_mv *mv)
{
    lat_objeto *objStr = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    char *mensaje = latC_checar_cadena(mv, objStr);
    int iResult = 1000;
    int iSendResult = send(latSocket->ListenSocket, mensaje, iResult, 0);
    if (iSendResult == SOCKET_ERROR)
    {
        // printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(latSocket->ListenSocket);
        WSACleanup();
        latC_error(mv, "send failed with error: %d\n", WSAGetLastError());
    }
    printf("Bytes sent: %d\n", iSendResult);
}

static void lat_conectar(lat_mv *mv)
{
    printf("lat_conectar\n");
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    struct addrinfo *ptr;
    SOCKET ConnectSocket = INVALID_SOCKET;
    for (ptr = latSocket->result; ptr != NULL; ptr = ptr->ai_next)
    {
        // // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        printf("ConnectSocket = socket\n");
        if (ConnectSocket == INVALID_SOCKET)
        {
            // printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            latC_error(mv, "socket failed with error: %ld\n", WSAGetLastError());
        }
        // Connect to server.
        int iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        printf("iResult = %i\n", iResult);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    // freeaddrinfo(latSocket->result);
    if (ConnectSocket == INVALID_SOCKET)
    {
        // printf("Unable to connect to server!\n");
        WSACleanup();
        latC_error(mv, "Unable to connect to server!\n");
    }
}

static void lat_cerrar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int iResult = shutdown(latSocket->ListenSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        // printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(latSocket->ListenSocket);
        WSACleanup();
        latC_error(mv, "shutdown failed with error: %d\n", WSAGetLastError());
    }
}

static const lat_CReg libsocketc[] = {
    // {"conectar", lat_conectar, 0},
    // {"iniciar_server", lat_iniciar_server, 2},
    // {"iniciar_cliente", lat_iniciar_cliente, 2},

    {"socket", lat_socket, 2},
    {"enlazar", lat_enlazar, 1},
    {"escuchar", lat_escuchar, 2},
    {"aceptar", lat_aceptar, 1},
    {"recv", lat_recv, 2},
    {"enviar", lat_enviar, 2},
    {"conectar", lat_conectar, 1},
    {"cerrar", lat_cerrar, 1},
    {NULL, NULL, 0}};

LATINO_API void latC_abrir_liblatino_socketc(lat_mv *mv)
{
    latC_abrir_liblatino(mv, LIB_WEBSOCKET_NAME, libsocketc);
}

// codigo latino
// incluir("websocket")
// server = socket.iniciar_server(3000)
// server.escuchar()
// server.cerrar_conexion()

// cliente = socket.inciar_cliente(3000)
// cliente.enviar_mensaje("hola mundo!")
// cliente.cerrar_conexion()

// funcion cliente.cerrar_conexion()
// código de Latino
// fin