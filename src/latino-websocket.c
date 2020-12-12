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

#include <latino.h>


#define LIB_WEBSOCKET_NAME "websocket"

static void lat_conectar(lat_mv *mv) {
    printf("%s\n", "hello world");
};

static void lat_iniciar_server(lat_mv *mv) {

    #pragma comment(lib,"Ws2_32.lib")

    // Iniciar Winsock
    WSADATA wsa;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int iResult = WSAStartup(MAKEWORD(2,2),&wsa);
	printf("%s\n", "Initialising Winsock...");
	if (iResult != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
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
    if (iResult != 0) {
        printf("getaddrinfo fallo: %d\n", iResult);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Crea un socket para escuchar consulta de la conexion.
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Escuchar las consultas
    if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    SOCKET ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // No longer need server socket
    closesocket(ListenSocket);

    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

        // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
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
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            // return 1;
            latC_apilar_int(mv, 1);
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
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

static void lat_iniciar_cliente(lat_mv *mv) {

    // Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
    #pragma comment (lib, "Ws2_32.lib")
    #pragma comment (lib, "Mswsock.lib")
    #pragma comment (lib, "AdvApi32.lib")

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
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        // return 1;
        latC_apilar_int(mv, 1);
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            // return 1;
            latC_apilar_int(mv, 1);
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        // return 1;
        latC_apilar_int(mv, 1);
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    // return 0;
    latC_apilar_int(mv, 0);
}

static const lat_CReg libwebsocket[] = {
    {"conectar", lat_conectar, 0},
    {"iniciar_server", lat_iniciar_server, 0},
    {"iniciar_cliente", lat_iniciar_cliente, 0},
    {NULL, NULL, 0}
};

LATINO_API void latC_abrir_liblatino_websocket(lat_mv *mv) {
    latC_abrir_liblatino(mv, LIB_WEBSOCKET_NAME, libwebsocket);
}

// codigo latino
// incluir("websocket")
// server = socket.iniciar_server(3000)
// server.escuchar()
// server.cerrar_conexion()

// cliente = socket.inciar_cliente(3000)
// cliente.enviar_mensaje("hola mundo!")
// cliente.cerrar_conexion()

funcion cliente.cerrar_conexion()
// código de Latino
fin