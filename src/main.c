#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Invalid number of arguments.\n");
  }

  char *urlToGet = argv[1];

  printf("Fetching %s\n...\n", urlToGet);

  struct addrinfo hints = {
    .ai_family = AF_UNSPEC,
    .ai_socktype = SOCK_STREAM,
  };

  struct addrinfo *result; // linked list of results

  int status = getaddrinfo(urlToGet, "http", &hints, &result);
  if (status != 0) {
    // fail
    printf("getaddrinfo error: %s\n", gai_strerror(status));
    return 1;
  }

  struct addrinfo *currResult = result;
  while (1) {
    if (currResult == NULL) break;

    void* addr;
    char ipString[INET6_ADDRSTRLEN];

    if (currResult->ai_family == AF_INET) {
      // IPv4
      addr = &((struct sockaddr_in *)currResult->ai_addr)->sin_addr;
    } else {
      addr = &((struct sockaddr_in6 *)currResult->ai_addr)->sin6_addr;
    }

    inet_ntop(currResult->ai_family, addr, ipString, sizeof ipString);
    printf("%s\n", ipString);
    currResult = currResult->ai_next;
  }
  
  return 0;
}
