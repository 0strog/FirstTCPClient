#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> /* getprotobyname */


int main(int argc, char **argv)
{
    char *server_reply = NULL;
    char *user_input = NULL;
    char buffer[BUFSIZ];
    in_addr_t in_addr;
    int filefd;
    int sockfd;
    ssize_t read_return;
    struct hostent *hostent;
    struct sockaddr_in sin;

    if(argc != 4)
    {
        printf("Write sending file, server's IP-addres and port: <file_name> <x.x.x.x> <x>");
        return 0;
    }
    printf("Simple TCP client v0.0\n");

    filefd = open(argv[1], O_RDONLY);
    if (filefd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    hostent = gethostbyname(argv[2]);
    if (hostent == NULL) {
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    unsigned short server_port = strtol(argv[3], NULL, 10);

    in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = in_addr;
    //inet_pton(AF_INET, "localhost", &sin.sin_addr);

    if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1)
    {
        printf("Socket created\n");

        if(connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) == -1)
        {
            perror("Connection fail\n");
            return EXIT_FAILURE;
        }
        while (1)
        {
            read_return = read(filefd, buffer, BUFSIZ);
            if (read_return == 0)
                break;
            else if (read_return == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }
            
            if (write(sockfd, buffer, read_return) == -1)
            {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }
        free(user_input);
        free(server_reply);
        close(filefd);
        exit(EXIT_SUCCESS);
    }
}
