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

int main() {    
  struct sigaction act;
  int create_socket, new_socket;    
  socklen_t addrlen;    
  int bufsize = 2048, n;    
  struct sockaddr_in address;    
  char * buffer = malloc(bufsize);    
  char * data_buffer;

  act.sa_handler = exit_handler;
  sigaction(SIGINT, &act, NULL);

  if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0){    
    printf("The socket was created\n");
  }

  address.sin_family = AF_INET;    
  address.sin_addr.s_addr = INADDR_ANY;    
  address.sin_port = htons(15003);    

  if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0){    
    printf("Binding Socket\n");
  }


  data_buffer = "World!";
  n = strlen(data_buffer);

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
    if(buffer[0] == 'P'){
      data_buffer = strstr(buffer, "\r\n\r\n") + 4;
    }
    printf("%s\n", buffer);    
    printf("\n========\n\n");
    printf("%s\n", data_buffer);
    write(new_socket, "<!DOCTYPE html><html lang=\"en\"><head>", 37);
    write(new_socket, "<meta charset=\"UTF-8\">", 22); 
    write(new_socket, "<script src=\"http://ajax.aspnetcdn.com/ajax/jQuery/jquery-1.12.4.min.js\"></script>", 82);
    write(new_socket, "<title></title></head><body><h1>Hello ", 38);
    write(new_socket, data_buffer, strlen(data_buffer)); 
    write(new_socket, "</h1>", 5);
    write(new_socket, "<form action=\"http://localhost:15003\" method=\"POST\" >", 53);
    write(new_socket, "<input type=\"text\" name=\"word\" placeholder=\"enter name\"/>", 57);
    write(new_socket, "<input type=\"submit\" value=\"Submit\"/></form>", 44);
    write(new_socket, "</body></html>", 14);
    close( new_socket );
  }    

cleanup: exit(0);

         return 0;    
}
