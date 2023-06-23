/*
 * Copyright ©2022 Dylan Hartono.  All rights reserved.
 */

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

extern "C" {
  #include "libpart1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;  // IPv6 or IPv4 mapped to IPv6
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocolhints.ai_family = AF_INET6;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  struct addrinfo *result;
  int res = getaddrinfo(nullptr, std::to_string(port_).c_str(),
                        &hints, &result);
  if (res != 0) {
    std::cerr << "getaddrinfo failed";
    return -1;
  }
  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  int li = -1;
  for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next) {
    li = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (li == -1) {
      std::cerr << "socket failed" << std::endl;
      li = -1;
      continue;
    }
    // to make the port available again as soon as we exit
    int optval = 1;
    setsockopt(li, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    // try binding
    if (bind(li, rp->ai_addr, rp->ai_addrlen) == 0) {
      // successful
      sock_family_ = rp->ai_family;
      break;
    }
    // failure
    close(li);
    li = -1;
  }
  // bind failed, return failure
  if (li == -1)
    return false;
  // bind successful, mark this to be a listening socket
  if (listen(li, SOMAXCONN) != 0) {
    std::cerr << "socket not listening";
    close(li);
    return false;
  }
  listen_sock_fd_ = li;
  *listen_fd = li;

  // clean up and return
  free(result);
  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:
  int client_fd;
  struct sockaddr_storage caddr;
  socklen_t caddr_len = sizeof(caddr);
  struct sockaddr* addr = reinterpret_cast<struct sockaddr *>(&caddr);
  while (1) {
    client_fd = accept(listen_sock_fd_, addr, &caddr_len);
    if (client_fd < 0) {
      if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
        continue;
      std::cerr << "Failure on accept";
      return false;
    }
    break;
  }

  *accepted_fd = client_fd;

  if (addr->sa_family == AF_INET) {
    // IPV4 stuff
    char str[INET_ADDRSTRLEN];
    struct sockaddr_in *soc = reinterpret_cast<struct sockaddr_in *>(addr);
    inet_ntop(AF_INET, &(soc->sin_addr), str, INET_ADDRSTRLEN);
    *client_addr = str;
    *client_port = ntohs(soc->sin_port);
  } else if (addr->sa_family == AF_INET6) {
    // IPV6 stuff
    char str[INET6_ADDRSTRLEN];
    struct sockaddr_in6 *soc = reinterpret_cast<struct sockaddr_in6 *>(addr);
    inet_ntop(AF_INET6, &(soc->sin6_addr), str, INET6_ADDRSTRLEN);
    *client_addr = str;
    *client_port = ntohs(soc->sin6_port);
  }

  char host_name[1024];
  if (getnameinfo(addr, caddr_len, host_name, 1024, nullptr, 0, 0) != 0) {
    return false;
  }

  *client_dns_name = host_name;

  char name[1024];
  name[0] = '\0';
  if (sock_family_ == AF_INET) {
    // server an IPv4 address.
    struct sockaddr_in soc;
    socklen_t socl = sizeof(soc);
    char addr[INET_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr *) &soc, &socl);
    inet_ntop(AF_INET, &soc.sin_addr, addr, INET_ADDRSTRLEN);
    *server_addr = addr;
    // if dns lookup fails, we'll return it's ip address
    getnameinfo((const struct sockaddr *) &soc, socl, name,
                1024, nullptr, 0, 0);
    *server_dns_name = name;
  } else {
    // server has an IPv6 address.
    struct sockaddr_in6 soc6;
    socklen_t socl6 = sizeof(soc6);
    char addr[INET6_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr *) &soc6, &socl6);
    inet_ntop(AF_INET6, &soc6.sin6_addr, addr, INET6_ADDRSTRLEN);
    *server_addr = addr;
    // if dns lookup fails, we'll return it's ip address
    getnameinfo((const struct sockaddr *) &soc6, socl6, name,
                1024, nullptr, 0, 0);
    *server_dns_name = name;
  }

  return true;
}

}  // namespace hw4
