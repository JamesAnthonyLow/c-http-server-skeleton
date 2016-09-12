#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int closeServer = 0;

void exit_handler(int r){
  closeServer++;
}

void shutdown_please( int socket ) {
  int optname= 1;
  setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&optname,sizeof(int));
  shutdown( socket, 2 );
  close( socket );
}

int main() {
  struct sigaction act;
  int create_socket, new_socket;
  socklen_t addrlen;
  int bufsize = 2048;
  struct sockaddr_in address;
  char * buffer = malloc(bufsize);
  char * data_buffer = malloc(64);

  act.sa_handler = exit_handler;
  sigaction(SIGINT, &act, NULL);

  if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){
    printf("The socket was created\n");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(15000);

  if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){
    printf("Binding Socket\n");
  }


  sprintf(data_buffer, "%s", "Hello World!");

  while (1) {
    memset(buffer, 0, bufsize);
    if(closeServer)
      goto cleanup;

    if (listen(create_socket, 10) < 0) {
      perror("server: listen");
      exit(1);
    }

    if ((new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen)) < 0) {
      perror("server: accept");
      exit(1);
    }

    if (new_socket > 0){
      printf("The Client is connected...\n");
    }

    recv(new_socket, buffer, bufsize, 0);
    // if(buffer[0] == 'P'){
    //   // memset(data_buffer, 0, 64);
    //   read(new_socket, data_buffer, 64);
    // }
    printf("%s\n", buffer);
    write(new_socket, "HTTP/1.1 200 OK\n", 16);
    write(new_socket, "Access-Control-Allow-Origin: *\n", 31);
    write(new_socket, "Content-Type: text/html\n\n", 25);
    write(new_socket, data_buffer, strlen(data_buffer));
    shutdown( new_socket, 2 );
    close( new_socket );
  }

cleanup: shutdown_please( new_socket ); shutdown_please( create_socket ); exit(0);

         return 0;
}
