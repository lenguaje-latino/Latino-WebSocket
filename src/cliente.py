import socket

mi_socket = socket.socket()
mi_socket.connect( ('localhost', 3000) )

mi_socket.send('Hola desde el cliente'.encode('utf-8'))
respuesta = mi_socket.recv(1024)

print (respuesta)
mi_socket.close()