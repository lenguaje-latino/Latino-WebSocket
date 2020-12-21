// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code

// Definiciones para el sistema operativo MS-Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if (defined __WIN32__) || (defined _WIN32)
#define LATINO_BUILD_AS_DLL
#define LATINO_LIB

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_PORT "3000"
#define DEFAULT_BUFLEN 512
#endif
// Fin MS-Windows

#include "latino.h"

#define LIB_WEBSOCKET_NAME "socketc"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

typedef struct LatSocket
{
    SOCKET socket;
    struct addrinfo *result;
} LatSocket;

static void lat_socket(lat_mv *mv)
{
    // Iniciar Winsock
    WSADATA wsaData;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        latC_error(mv, "Failed. Error Code : %d", WSAGetLastError());
    }
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    lat_objeto *puerto = latC_desapilar(mv);
    lat_objeto *host = latC_desapilar(mv);
    char *str_host = latC_checar_cadena(mv, host);
    char *c_puerto = latC_astring(mv, puerto);
    iResult = getaddrinfo(str_host, c_puerto, &hints, &result);
    if (iResult != 0)
    {
        WSACleanup();
        latC_error(mv, "getaddrinfo fallo: %d\n", iResult);
    }

    // Create a SOCKET for connecting to server
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        latC_error(mv, "Error at socket(): %ld\n", WSAGetLastError());
    }
    LatSocket *latSocket = (LatSocket *)malloc(sizeof(LatSocket));
    latSocket->socket = ListenSocket;
    latSocket->result = result;
    lat_objeto *lat_socket = latC_crear_cdato(mv, latSocket);
    latC_apilar(mv, lat_socket);
}

static void lat_enlazar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);

    // Setup the TCP listening socket
    int iResult = bind(latSocket->socket, latSocket->result->ai_addr, (int)latSocket->result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        freeaddrinfo(latSocket->result);
        closesocket(latSocket->socket);
        WSACleanup();
        latC_error(mv, "bind failed with error: %d\n", WSAGetLastError());
    }

    freeaddrinfo(latSocket->result);
}

static void lat_escuchar(lat_mv *mv)
{
    lat_objeto *max_conex = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int int_max_conex = (int)latC_checar_numerico(mv, max_conex);
    int iResult = listen(latSocket->socket, int_max_conex);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(latSocket->socket);
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
    ClientSocket = accept(latSocket->socket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        closesocket(latSocket->socket);
        WSACleanup();
        latC_error(mv, "accept failed: %d\n", WSAGetLastError());
    }
    LatSocket *latSocketClient = (LatSocket *)malloc(sizeof(LatSocket));
    latSocketClient->socket = ClientSocket;
    latSocketClient->result = latSocket->result;
    lat_objeto *client_socket = latC_crear_cdato(mv, latSocketClient);
    latC_apilar(mv, client_socket);
}

static void lat_recv(lat_mv *mv)
{
    lat_objeto *tam_buffer = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int int_tam_buffer = (int)latC_checar_numerico(mv, tam_buffer);
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iSendResult = 0;
    int iResult = recv(latSocket->socket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
        latC_apilar_string(mv, recvbuf);
    }
    else if (iResult == 0)
    {
        printf("Connection closing...\n");
    }
    else
    {
        closesocket(latSocket->socket);
        WSACleanup();
        latC_error(mv, "recv failed with error: %d\n", WSAGetLastError());
    }
}

static void lat_enviar(lat_mv *mv)
{
    lat_objeto *objStr = latC_desapilar(mv);
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    char *mensaje = latC_checar_cadena(mv, objStr);
    int iResult = 1000;
    int iSendResult = send(latSocket->socket, mensaje, iResult, 0);
    if (iSendResult == SOCKET_ERROR)
    {
        closesocket(latSocket->socket);
        WSACleanup();
        latC_error(mv, "send failed with error: %d\n", WSAGetLastError());
    }
}

static void lat_conectar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    struct addrinfo *ptr;
    SOCKET ConnectSocket = INVALID_SOCKET;
    for (ptr = latSocket->result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            WSACleanup();
            latC_error(mv, "socket failed with error: %i\n", WSAGetLastError());
        }
        // Connect to server.
        int iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    if (ConnectSocket == INVALID_SOCKET)
    {
        WSACleanup();
        latC_error(mv, "Unable to connect to server!\n");
    }

    LatSocket *latSocketClient = (LatSocket *)malloc(sizeof(LatSocket));
    latSocketClient->socket = ConnectSocket;
    latSocketClient->result = latSocket->result;
    lat_objeto *client_socket = latC_crear_cdato(mv, latSocketClient);
    latC_apilar(mv, client_socket);
}

static void lat_cerrar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    int iResult = shutdown(latSocket->socket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(latSocket->socket);
        WSACleanup();
        latC_error(mv, "shutdown failed with error: %d\n", WSAGetLastError());
    }
    // cleanup
    closesocket(latSocket->socket);
    WSACleanup();
}

static const lat_CReg libsocketc[] = {
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