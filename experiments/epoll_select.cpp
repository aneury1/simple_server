#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>
#include <vector>

#define PORT 8080 // Master server port
#define MAX_EVENTS 1000
#define BUFFER_SIZE 1024

// Backend server configuration
std::map<std::string, std::pair<std::string, int>> backendServers = {
    {"PROTOCOL1", {"127.0.0.1", 9001}}, // Route to server on 127.0.0.1:9001
    {"PROTOCOL2", {"127.0.0.1", 9002}}  // Route to server on 127.0.0.1:9002
};

// Function to set socket to non-blocking
void setNonBlocking(int sock) {
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}

// Forward data between two sockets
void forwardData(int clientSocket, int backendSocket) {
  fd_set readfds;
  char buffer[BUFFER_SIZE];

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(clientSocket, &readfds);
    FD_SET(backendSocket, &readfds);

    int maxFd = std::max(clientSocket, backendSocket) + 1;
    int activity = select(maxFd, &readfds, nullptr, nullptr, nullptr);

    if (activity > 0) {
      // Forward data from client to backend
      if (FD_ISSET(clientSocket, &readfds)) {
        int bytesRead = read(clientSocket, buffer, BUFFER_SIZE);
        if (bytesRead <= 0)
          break; // Client disconnected
        send(backendSocket, buffer, bytesRead, 0);
      }

      // Forward data from backend to client
      if (FD_ISSET(backendSocket, &readfds)) {
        int bytesRead = read(backendSocket, buffer, BUFFER_SIZE);
        if (bytesRead <= 0)
          break; // Backend disconnected
        send(clientSocket, buffer, bytesRead, 0);
      }
    }
  }

  close(clientSocket);
  close(backendSocket);
}

// Main Master Server logic
int main() {
  int masterSocket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in masterAddr {
  }, clientAddr{};
  socklen_t addrLen = sizeof(clientAddr);

  masterAddr.sin_family = AF_INET;
  masterAddr.sin_addr.s_addr = INADDR_ANY;
  masterAddr.sin_port = htons(PORT);

  bind(masterSocket, (struct sockaddr *)&masterAddr, sizeof(masterAddr));
  listen(masterSocket, SOMAXCONN);
  setNonBlocking(masterSocket);

  std::cout << "Master server listening on port " << PORT << "...\n";

  int epollFd = epoll_create1(0);
  struct epoll_event ev, events[MAX_EVENTS];
  ev.events = EPOLLIN;
  ev.data.fd = masterSocket;
  epoll_ctl(epollFd, EPOLL_CTL_ADD, masterSocket, &ev);

  while (true) {
    int ready = epoll_wait(epollFd, events, MAX_EVENTS, -1);
    for (int i = 0; i < ready; ++i) {
      if (events[i].data.fd == masterSocket) {
        // Accept new client
        int clientSocket =
            accept(masterSocket, (struct sockaddr *)&clientAddr, &addrLen);
        setNonBlocking(clientSocket);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = clientSocket;
        epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &ev);
      } else {
        int clientSocket = events[i].data.fd;
        char buffer[BUFFER_SIZE] = {0};

        // Peek at incoming data to identify the protocol
        ssize_t bytesPeeked = recv(clientSocket, buffer, BUFFER_SIZE, MSG_PEEK);
        if (bytesPeeked > 0) {
          std::string requestHeader(buffer, bytesPeeked);
          std::cout << "Received: " << requestHeader << "\n";

          // Determine the backend server
          std::string protocol =
              "PROTOCOL1"; // Dummy logic: Parse actual protocol here
          auto backend = backendServers[protocol];

          // Connect to the backend server
          int backendSocket = socket(AF_INET, SOCK_STREAM, 0);
          struct sockaddr_in backendAddr {};
          backendAddr.sin_family = AF_INET;
          inet_pton(AF_INET, backend.first.c_str(), &backendAddr.sin_addr);
          backendAddr.sin_port = htons(backend.second);

          if (connect(backendSocket, (struct sockaddr *)&backendAddr,
                      sizeof(backendAddr)) == 0) {
            std::cout << "Connected to backend server: " << protocol << "\n";
            std::thread(forwardData, clientSocket, backendSocket).detach();
          } else {
            std::cerr << "Failed to connect to backend server.\n";
            close(clientSocket);
            close(backendSocket);
          }

          // Remove clientSocket from epoll after forwarding
          epoll_ctl(epollFd, EPOLL_CTL_DEL, clientSocket, nullptr);
        } else {
          close(clientSocket);
        }
      }
    }
  }

  close(masterSocket);
  return 0;
}
