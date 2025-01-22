#ifndef TCP_HANDLER_DEFINED
#define TCP_HANDLER_DEFINED
#include "defines.h"
#include <vector>
#include <stdint.h>

 


 

struct tcp_handler{
    virtual ~tcp_handler(){}
    virtual tcp_handler_status create_server() = 0;
    virtual tcp_handler_status accept_and_process_message() = 0;
};

static inline void debug_socket_handler()
{
#if 0
/// SOL_SOCKET OPTIONS
SO_REUSEADDR	Allow reuse of local addresses.	int
SO_REUSEPORT	Allow reuse of the same port.	int
SO_KEEPALIVE	Enable periodic keep-alive probes.	int
SO_BROADCAST	Enable sending broadcast messages.	int
SO_LINGER	Control socket close behavior.	struct linger
SO_RCVBUF	Set receive buffer size.	int
SO_SNDBUF	Set send buffer size.	int
SO_RCVTIMEO	Set receive timeout.	struct timeval
SO_SNDTIMEO	Set send timeout.	struct timeval
SO_ERROR	Get socket error status.	int
/// IPPROTO_TCP (TCP-specific options)
Option	Description	Type
TCP_NODELAY	Disable Nagle s algorithm.	int
TCP_MAXSEG	Set maximum segment size.	int
TCP_KEEPIDLE	Idle time before keep-alive probes.	int
TCP_KEEPINTVL	Interval between keep-alive probes.	int
TCP_KEEPCNT	Number of keep-alive probes before drop.	int
///IPPROTO_IP (IPv4-specific options)
Option	Description	Type
IP_TTL	Set Time-To-Live for outgoing packets.	int
IP_MULTICAST_TTL	Set multicast TTL.	int
IP_ADD_MEMBERSHIP	Join multicast group.	struct ip_mreq
IP_DROP_MEMBERSHIP	Leave multicast group.	struct ip_mreq
#endif
}



#endif