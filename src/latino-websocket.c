#ifdef _WIN32
    #define LATINO_BUILD_AS_DLL
#endif

#define LATINO_LIB

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <latino.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib,"Ws2_32.lib")

// #ifndef LIB_WEBSOCKET_NAME
#define LIB_WEBSOCKET_NAME "websocket"

static void lat_conectar(lat_mv *mv) {
	WSADATA wsa;
	printf("%s\n", "Initialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
	}
	printf("%s\n", "Initialised.");
};

static const lat_CReg libwebsocket[] = {
    {"conectar", lat_conectar, 0},
    {NULL, NULL, 0}
};

LATINO_API void latC_abrir_liblatino_websocket(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_WEBSOCKET_NAME, libwebsocket);
}