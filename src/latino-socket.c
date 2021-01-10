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

/*********************************************************************************
*   Autor: Fundación Lenguaje Latino                                             *
*                                                                                *
* Website: Oficial       <https://lenguajelatino.org>                    2015-on *
* Website: Manual Latino <https://manual.lenguajelatino.com>             2020-on *
*    Repo: GitHub        <https://github.com/jorge2985/Latino-WebSocket> 2020-on *
*                                                                                *
*    Desc: Librería WebSocket para Latino                                        *
**********************************************************************************/

#include "lat_socket_compat.h"

#define LIB_WEBSOCKET_NAME "socket"

static void lat_socket(lat_mv *mv)
{
    // Resolve the server address and port
    lat_objeto *puerto = latC_desapilar(mv);
    lat_objeto *host = latC_desapilar(mv);
    char *str_host = latC_checar_cadena(mv, host);
    char *c_puerto = latC_astring(mv, puerto);
    
    printf("Empezamos lat_socket");
    LatSocket *lat_sock_result = lat_sock_crear(mv, str_host, c_puerto);
    latC_apilar(mv, lat_sock_result);
}

static void lat_enlazar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    lat_sock_bind(mv, latSocket);
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
        lat_sock_cerrar(mv, latSocket->socket);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Listen failed with error: %ld\n", WSAGetLastError());
#else
        latC_error(mv, "Listen failed with error");
#endif
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
        lat_sock_cerrar(mv, latSocket->socket);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Error al aceptar el socket: %d\n", WSAGetLastError());
#else
        latC_error(mv, "Error al aceptar el socket");
#endif
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
        printf("Cerrando conexion...\n");
    }
    else
    {
        lat_sock_cerrar(mv, latSocket->socket);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Fallo al recibir mensaje: %d\n", WSAGetLastError());
#else
        latC_error(mv, "Fallo al recibir mensaje");
#endif
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
        lat_sock_cerrar(mv, latSocket->socket);
#ifdef _WIN32
        WSACleanup();
        latC_error(mv, "Error al enviar mensaje: %d\n", WSAGetLastError());
#else
        latC_error(mv, "Error al enviar mensaje");
#endif
    }
}

static void lat_conectar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    lat_objeto *client_socket = lat_sock_conectar(mv, latSocket);
    latC_apilar(mv, client_socket);
}

static void lat_cerrar(lat_mv *mv)
{
    lat_objeto *socketc = latC_desapilar(mv);
    LatSocket *latSocket = (LatSocket *)latC_checar_cptr(mv, socketc);
    lat_sock_cerrar(mv, latSocket);
}

static const lat_CReg libsocket[] = {
    {"socket", lat_socket, 2},
    {"enlazar", lat_enlazar, 1},
    {"escuchar", lat_escuchar, 2},
    {"aceptar", lat_aceptar, 1},
    {"recibir", lat_recv, 2},
    {"recv", lat_recv, 2},
    {"enviar", lat_enviar, 2},
    {"conectar", lat_conectar, 1},
    {"cerrar", lat_cerrar, 1},
    {NULL, NULL, 0}};

LATINO_API void latC_abrir_liblatino_socket(lat_mv *mv)
{
    latC_abrir_liblatino(mv, LIB_WEBSOCKET_NAME, libsocket);
}