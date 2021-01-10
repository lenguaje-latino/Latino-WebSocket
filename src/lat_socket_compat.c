/*********************************************************************************
* The MIT License (MIT)                                                          *
*                                                                                *
* Copyright (c) Latino - Lenguaje de Programacion                                *
*                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining          *
* a copy of this software and associated documentation files (the "Software"),   *
* to deal in the Software without restriction, including without limitation      *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,       *
* and/or sell copies of the Software, and to permit persons to whom the Software *
* is furnished to do so, subject to the following conditions:                    *
*                                                                                *
* The above copyright notice and this permission notice shall be included in     *
* all copies or substantial portions of the Software.                            *
*                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS        *
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL        *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER     *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN      *
* THE SOFTWARE.                                                                  *
**********************************************************************************/

#include "lat_socket_compat.h"

LATINO_API lat_objeto *lat_sock_crear(lat_mv *mv, char *str_host, char *c_puerto) {
    int iResult;
#if (defined __WIN32__) || (defined _WIN32)
    // Iniciar Winsock
    WSADATA wsaData;
    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen              =   DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult                     =   WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        latC_error(mv, "Fallo al iniciar socket. Codigo de error: %d\n", WSAGetLastError());
    }
#endif
    struct addrinfo *server_address = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family             =   AF_INET;
    hints.ai_socktype           =   SOCK_STREAM;
    hints.ai_protocol           =   IPPROTO_TCP;
    hints.ai_flags              =   AI_PASSIVE;

    iResult                     =   getaddrinfo(str_host, c_puerto, &hints, &server_address);
    if (iResult != 0) {
#ifdef _WIN32
        WSACleanup();
#endif
        latC_error(mv, "Error. getaddrinfo fallo: %d\n", iResult);
    }

    // Create a SOCKET for connecting to server
    SOCKET l_socket             =   INVALID_SOCKET;
    l_socket                    =   socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
    if (l_socket == INVALID_SOCKET) {
        freeaddrinfo(server_address);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Error at socket(): %ld\n", WSAGetLastError());
#else
        latC_error(mv, "Error at socket()\n");
#endif
    }
    LatSocket *latSocket        =   (LatSocket *)malloc(sizeof(LatSocket));
    latSocket->socket           =   l_socket;
    latSocket->result           =   server_address;
    lat_objeto *lat_socket      =   latC_crear_cdato(mv, latSocket);

    return lat_socket;
}

LATINO_API void lat_sock_bind(lat_mv *mv, LatSocket *latSocket) {
    // Setup the TCP listening socket
    int iResult                 =   bind(latSocket->socket, latSocket->result->ai_addr, (int)latSocket->result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(latSocket->result);
        lat_sock_cerrar(mv, latSocket->socket);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Error al enlazar el socket: %d\n", WSAGetLastError());
#else
        latC_error(mv, "Error al enlazar el socket\n");
#endif
    }
    freeaddrinfo(latSocket->result);
}

LATINO_API lat_objeto *lat_sock_conectar(lat_mv *mv, LatSocket *latSocket) {
    SOCKET ConnectSocket        =   INVALID_SOCKET;
    struct addrinfo *ptr;
    for (ptr = latSocket->result; ptr != NULL; ptr = ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket           =   socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
#ifdef _WIN32
            WSACleanup();
            latC_error(mv, "Error al crear el socket: %i\n", WSAGetLastError());
#else
            latC_error(mv, "Error al crear el socket\n");
#endif
        }
        // Connect to server.
        int iResult             =   connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            lat_sock_cerrar(mv, ConnectSocket);
            ConnectSocket       =   INVALID_SOCKET;
            continue;
        }
        break;
    }
    if (ConnectSocket == INVALID_SOCKET) {
#ifdef _WIN32
        WSACleanup();
#endif
        latC_error(mv, "Incapaz de conectarse al servidor!\n");
    }
    LatSocket *latSocketClient  =   (LatSocket *)malloc(sizeof(LatSocket));
    latSocketClient->socket     =   ConnectSocket;
    latSocketClient->result     =   latSocket->result;
    lat_objeto *client_socket   =   latC_crear_cdato(mv, latSocketClient);

    return client_socket;
}

LATINO_API void lat_sock_cerrar(lat_mv *mv, LatSocket *lat_sock) {
    int iResult                 =   shutdown(lat_sock->socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
#ifdef _WIN32
        closesocket(lat_sock->socket);
        WSACleanup();
        latC_error(mv, "Error al cerrar el socket: %d\n", WSAGetLastError());
#else
        close(lat_sock->socket);
        latC_error(mv, "Error al cerrar el socket\n");
#endif
    }
    // cleanup
#ifdef _WIN32
    closesocket(lat_sock->socket);
    WSACleanup();
#else
    close(lat_sock->socket);
#endif
}