import socket
connexion_avec_serveur = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
connexion_avec_serveur.connect(('192.168.4.1', 15555))
message = "This is a message"
connexion_avec_serveur.sendto(message.encode('utf-8'), ('192.168.4.1', 15555))
connexion_avec_serveur.close()
