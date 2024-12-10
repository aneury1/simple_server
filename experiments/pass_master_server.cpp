#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define UNIX_SOCKET_PATH "/tmp/master_to_backend.sock"

int recv_fd(int unix_socket) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(int))];
    memset(buf, 0, sizeof(buf));

    struct iovec io = { .iov_base = (void*)" ", .iov_len = 1 };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    if (recvmsg(unix_socket, &msg, 0) < 0) {
        perror("Failed to receive file descriptor");
        return -1;
    }

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    return *((int*)CMSG_DATA(cmsg));
}

int main() {
    int unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un unix_addr{};
    unix_addr.sun_family = AF_UNIX;
    unlink(UNIX_SOCKET_PATH); // Remove previous socket file
    strcpy(unix_addr.sun_path, UNIX_SOCKET_PATH);

    bind(unix_socket, (struct sockaddr*)&unix_addr, sizeof(unix_addr));
    listen(unix_socket, SOMAXCONN);

    std::cout << "Backend Server waiting for connections...\n";

    while (true) {
        int conn = accept(unix_socket, nullptr, nullptr);
        int client_fd = recv_fd(conn);

        if (client_fd >= 0) {
            std::cout << "Backend Server took over client connection!\n";

            char buffer[1024];
            int bytesRead = read(client_fd, buffer, sizeof(buffer));
            if (bytesRead > 0) {
                std::cout << "Received from client: " << std::string(buffer, bytesRead) << "\n";
                send(client_fd, "Handled by Backend\n", 19, 0);
            }

            close(client_fd);
        }
        close(conn);
    }

    close(unix_socket);
    return 0;
}
