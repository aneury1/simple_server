#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <string>
#include <stdio.h>

namespace TCPClientSender
{
    void debug_address(in_addr addr, int i = 0, const char *prev = nullptr)
    {
        if (prev == nullptr)
            printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
        else
            printf("%s %d %s \n", prev, i, inet_ntoa(addr));
    }

    enum
    {
        INVALID_CONFIGURATION = -10,
        CANT_GET_THE_ENDPOINT_ROUTE = -12,
        CONNECTED = 0,
        NO_CONNECTED = 99
    };

    struct socket_stream
    {
        std::vector<char> datapacket_sent;
        std::vector<char> datapacket_received;
        std::string field_separator;
        bool capture_logs;
        int socket;
        int status;
        typedef void (*midleware_function)(socket_stream, char *, int);
        std::vector<midleware_function> observers;
        std::vector<in_addr> address_to_connect;
    };

    socket_stream _stream_point;
    void close_to()
    {
        shutdown(_stream_point.socket, SHUT_RDWR);
        _stream_point.socket = -1;
        _stream_point.status = NO_CONNECTED;
    }

    void set_not_blocking()
    {
        int arg;
        arg |= O_NONBLOCK;
        if (fcntl(_stream_point.socket, F_SETFL, arg) < 0)
        {
            ////  fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno));
            exit(0);
        }
    }

   



    void connect_to(const char *uri, short port)
    {
        struct hostent *remoteHost;
        struct in_addr addr;
        remoteHost = gethostbyname(uri);

        if (remoteHost == nullptr)
        {
            printf("Null Remote?");
        }

        int i = 0;

        if (_stream_point.status == CONNECTED)
            close_to();

        while (remoteHost->h_addr_list[i] != 0)
        {
            addr.s_addr = *(u_long *)remoteHost->h_addr_list[i++];
            _stream_point.address_to_connect.emplace_back(addr);
            debug_address(addr, i);
        }
        _stream_point.socket = socket(AF_INET, SOCK_STREAM, 0);
        if (_stream_point.socket == -1)
        {
            _stream_point.status = INVALID_CONFIGURATION;
            return;
        }

        for (auto addr_2 : _stream_point.address_to_connect)
        {
            sockaddr_in address;
            address.sin_addr = addr_2;
            address.sin_port = htons(port);
            address.sin_family = AF_INET;
            debug_address(addr, i, "trying to connected to: ");
            ///set_not_blocking();
            if (connect(_stream_point.socket, (sockaddr *)&address, sizeof(address)) != 0)
            {
                _stream_point.status = CANT_GET_THE_ENDPOINT_ROUTE;
            }
            else
            {
                break;
            }
        }
        /*
        sockaddr_in address;
        address.sin_addr = addr;
        address.sin_port = htons(port);
        address.sin_family = AF_INET;

        if (connect(_stream_point.socket, (sockaddr *)&address, sizeof(address)) != 0)
        {
            _stream_point.status = CANT_GET_THE_ENDPOINT_ROUTE;
            return;
        }*/
        printf("Socket connected = %d", _stream_point.status);

        _stream_point.status = CONNECTED;
    }

    int sendPacket(const char *buffer, int len)
    {
        if (_stream_point.status != CONNECTED)
        {
            return NO_CONNECTED;
        }
        int snt = send(_stream_point.socket, buffer, len, 0);
        if (snt > 0)
        {
            printf("Payload sent Status=> %d length=>%d\n", _stream_point.status, snt);
        }
        return snt;
    }

    int receivedPacket(char *outPacket, int max_len)
    {
        if (_stream_point.status != CONNECTED)
        {
            return NO_CONNECTED;
        }
        int rec = recv(_stream_point.socket, outPacket, max_len, 0);
        printf("\nrecv %d \n%s\n",rec, outPacket);
        return rec;
    }
};