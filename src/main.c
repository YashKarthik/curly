#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#define MAXDATASIZE 100

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Invalid number of arguments.\n");
  }

  char *urlToGet = argv[1];

  printf("Fetching %s ...\n", urlToGet);

  struct addrinfo hints = {
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM,
  };

  struct addrinfo *result; // linked list of results

  int status = getaddrinfo(urlToGet, "http", &hints, &result);
  if (status != 0) {
    printf("getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }

  struct addrinfo *currResult;
  int connectedSockfd;
  struct addrinfo *connectedAddr;
  for (currResult = result; result != NULL; currResult = currResult->ai_next) {
    if (currResult == NULL) break;

    void* addr;
    char ipString[INET6_ADDRSTRLEN];

    if (currResult->ai_family == AF_INET) {
      addr = &((struct sockaddr_in *)currResult->ai_addr)->sin_addr;
    } else {
      addr = &((struct sockaddr_in6 *)currResult->ai_addr)->sin6_addr;
    }

    inet_ntop(currResult->ai_family, addr, ipString, sizeof ipString);
    printf("%s\n", ipString);

    int sockfd = socket(currResult->ai_family, currResult->ai_socktype, currResult->ai_protocol);
    if (sockfd == -1) {
      printf("socket error\n");
      continue;
    }

    if (connect(sockfd, currResult->ai_addr, currResult->ai_addrlen)) {
      close(sockfd);
      printf("Connection failed: %s", ipString);
      printf("Reason: %s", gai_strerror(errno));
    }

    printf("Connected to %s\n", ipString);
    connectedSockfd = sockfd;
    connectedAddr = currResult;
    break;
  }

  if (currResult == NULL) {
    printf("Failed to connect");
    return 1;
  }

  char sendBuff[MAXDATASIZE];
  snprintf(sendBuff, MAXDATASIZE, "GET / HTTP/1.0\r\nHost: %s\r\n", urlToGet);
  send(connectedSockfd, sendBuff, sizeof(sendBuff) - 1, 0);

  char recvBuff[MAXDATASIZE];
  int bytesRecvd = recv(connectedSockfd, recvBuff, MAXDATASIZE-1, 0);
  if (bytesRecvd == -1) {
    printf("Failed recv\n");
    return 1;
  }

  recvBuff[bytesRecvd] = '\0';
  printf("Received:\n%s", recvBuff);

  freeaddrinfo(result);
  close(connectedSockfd);
;  return 0;
}
