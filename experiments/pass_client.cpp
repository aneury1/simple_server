#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#define UNIX_SOCKET_PATH "/tmp/master_to_backend.sock"

void send_fd(int unix_socket, int fd_to_send) {
    struct msghdr msg = {0};
    char buf[CMSG_SPACE(sizeof(int))];
    memset(buf, 0, sizeof(buf));

    struct iovec io = { .iov_base = (void*)"FD", .iov_len = 2 };
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;

    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *((int*)CMSG_DATA(cmsg)) = fd_to_send;

    if (sendmsg(unix_socket, &msg, 0) < 0) {
        perror("Failed to send file descriptor");
    }
}

int main() {
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in master_addr{};

    master_addr.sin_family = AF_INET;
    master_addr.sin_addr.s_addr = INADDR_ANY;
    master_addr.sin_port = htons(8080);

    bind(master_socket, (struct sockaddr*)&master_addr, sizeof(master_addr));
    listen(master_socket, SOMAXCONN);

    // UNIX socket to backend server
    int unix_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un unix_addr{};
    unix_addr.sun_family = AF_UNIX;
    strcpy(unix_addr.sun_path, UNIX_SOCKET_PATH);
    connect(unix_socket, (struct sockaddr*)&unix_addr, sizeof(unix_addr));

    std::cout << "Master Server listening on 8080...\n";

    while (true) {
        int client_fd = accept(master_socket, nullptr, nullptr);
        std::cout << "Accepted client, transferring socket...\n";

        send_fd(unix_socket, client_fd);
        close(client_fd); // Close the file descriptor after sending
    }

    close(master_socket);
    close(unix_socket);
    return 0;
}
