import os
import socket
import threading

# Define some constants
HOST = '0.0.0.0'
PORT = 1100
STORAGE_DIR = 'maildir'
USER = 'user'
PASS = 'pass'

# Create storage directory if it doesn't exist
if not os.path.exists(STORAGE_DIR):
    os.makedirs(STORAGE_DIR)

def list_messages():
    return os.listdir(STORAGE_DIR)

def retrieve_message(message_num):
    message_list = list_messages()
    if message_num < 1 or message_num > len(message_list):
        return None
    with open(os.path.join(STORAGE_DIR, message_list[message_num - 1]), 'r') as f:
        return f.read()

def delete_message(message_num):
    message_list = list_messages()
    if message_num < 1 or message_num > len(message_list):
        return False
    os.remove(os.path.join(STORAGE_DIR, message_list[message_num - 1]))
    return True

def handle_client(client_socket):
    client_socket.send(b'+OK POP3 server ready\r\n')

    authenticated = False
    while True:
        command = client_socket.recv(1024).decode('utf-8').strip()
        if not command:
            break
        print(command)

        if command.startswith('USER'):
            user = command.split(' ')[1]
            if user == USER:
                client_socket.send(b'+OK User accepted\r\n')
            else:
                client_socket.send(b'-ERR Invalid user\r\n')

        elif command.startswith('PASS'):
            password = command.split(' ')[1]
            if password == PASS:
                authenticated = True
                client_socket.send(b'+OK Pass accepted\r\n')
            else:
                client_socket.send(b'-ERR Invalid password\r\n')

        elif command == 'STAT':
            if authenticated:
                message_list = list_messages()
                num_messages = len(message_list)
                total_size = sum(os.path.getsize(os.path.join(STORAGE_DIR, msg)) for msg in message_list)
                client_socket.send(f'+OK {num_messages} {total_size}\r\n'.encode('utf-8'))
            else:
                client_socket.send(b'-ERR Not authenticated\r\n')

        elif command == 'LIST':
            if authenticated:
                message_list = list_messages()
                response = f'+OK {len(message_list)} messages\r\n'
                for i, msg in enumerate(message_list):
                    response += f'{i+1} {os.path.getsize(os.path.join(STORAGE_DIR, msg))}\r\n'
                response += '.\r\n'
                client_socket.send(response.encode('utf-8'))
            else:
                client_socket.send(b'-ERR Not authenticated\r\n')

        elif command.startswith('RETR'):
            if authenticated:
                message_num = int(command.split(' ')[1])
                message = retrieve_message(message_num)
                if message:
                    client_socket.send(f'+OK {len(message)} octets\r\n'.encode('utf-8'))
                    client_socket.send(message.encode('utf-8'))
                    client_socket.send(b'\r\n.\r\n')
                else:
                    client_socket.send(b'-ERR No such message\r\n')
            else:
                client_socket.send(b'-ERR Not authenticated\r\n')

        elif command.startswith('DELE'):
            if authenticated:
                message_num = int(command.split(' ')[1])
                if delete_message(message_num):
                    client_socket.send(b'+OK Message deleted\r\n')
                else:
                    client_socket.send(b'-ERR No such message\r\n')
            else:
                client_socket.send(b'-ERR Not authenticated\r\n')

        elif command == 'QUIT':
            client_socket.send(b'+OK Goodbye\r\n')
            break

        else:
            client_socket.send(b'-ERR Unknown command\r\n')

    client_socket.close()

def start_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen(5)
    print(f'Server started on {HOST}:{PORT}')

    while True:
        client_socket, addr = server_socket.accept()
        print(f'Accepted connection from {addr}')
        client_handler = threading.Thread(target=handle_client, args=(client_socket,))
        client_handler.start()

if __name__ == '__main__':
    start_server()
