import socket
import base64
import hashlib
import struct 


def receive_frame(client_socket):

    frame_header = client_socket.recv(2)
    
    if not frame_header:
        return None

    byte1, byte2 = struct.unpack("!BB", frame_header)

    fin = (byte1 >> 7) & 1
    opcode = byte1 & 0x0F
    masked = (byte2 >> 7) & 1
    payload_length = byte2 & 0x7F

    if payload_length  == 126:
        payload_length = struct.unpack("!H", client_socket.recv(2))[0]
    elif payload_length == 127:
        payload_length = struct.unpack("!Q", client_socket.recv(8))[0]

    if masked:
        masking_key = client_socket.recv(4)
        payload = client_socket.recv(payload_length)
        decoded = bytearray(b ^ masking_key[i % 4] for i, b in enumerate(payload))
    else:
        decoded = client_socket.recv(payload_length)

    return decoded.decode("utf-8")


def send_frame(client_socket, data):
    frame = [0x81]
    payload = data.encode("utf-8")
    payload_length = len(payload)

    if payload_length <= 125:
        frame.append(payload_length)
    elif payload_length <= 65535:
        frame.append(126)
        frame.extend(struct.pack("!H", payload_length))
    else:
        frame.append(127)
        frame.extend(struct.pack("!Q", payload_length))

    frame_to_send = bytearray(frame) + payload
    client_socket.send(frame_to_send)



def perform_handshake(client_socket):
    request = client_socket.recv(1024).decode('utf-8')
    print("Request received\n", request)

    headers={}
    for line in request.split("\r\n"):
        if ": " in line:
            key, value = line.split(": ", 1)
            headers[key] = value

    sec_websocket_key = headers.get("Sec-WebSocket-Key")
    if not sec_websocket_key:
        print("Not Websocket Key found closing connection")
        client_socket.close()
        return False

    magic_string = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    accept_key = base64.b64encode(
            hashlib.sha1((sec_websocket_key + magic_string).encode()).digest()
    ).decode()
    
    response = (
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            f"Sec-Websocket-Accept: {accept_key}\r\n"
            "\r\n"        
    )
    
    client_socket.send(response.encode())
    print("HS Sent")
    return True




HOST= '0.0.0.0'
PORT= 8991

server_socket= socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST,PORT))
server_socket.listen(1)

print(f"listening on port{HOST}:{PORT}")

while True:
    client_socket, addr = server_socket.accept()

    print(f"client connection from address")

    if perform_handshake(client_socket):
       while True:

           try:
               message = receive_frame(client_socket)
               if message:
                   print(f"Message received {message}")
                   send_frame(client_socket, f"echo: {message}")
               else:
                    break

           except Exception as e:
                print("Error:  ", e)
                break
    client_socket.close()

