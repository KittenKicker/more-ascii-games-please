#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
#define HOSTLEN 256
#define BACKLOG 1
int make_server_socket_q(int , int );

int make_server_socket(int portnum) 
{
 return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog) 
{
    struct sockaddr_in saddr; 
    struct hostent *hp;
    char hostname[HOSTLEN]; 
    int sock_id;
    sock_id = socket(PF_INET, SOCK_STREAM, 0);
    if ( sock_id == -1 ) 
    {
        return -1;
    }
    bzero((void *)&saddr, sizeof(saddr));
    // gethostname(hostname, HOSTLEN); // still bogus
    // hp = gethostbyname(hostname);
    // bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr,
    // hp->h_length);
    saddr.sin_addr.s_addr = INADDR_ANY; // NOTE FIX!!!!
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET ;
    if ( bind(sock_id, (struct sockaddr *)&saddr,
        sizeof(saddr)) != 0 ) 
    {
        return -1;
    }
    if ( listen(sock_id, backlog) != 0 ) 
    {
        return -1;
    }
    return sock_id;
}

process_request(int fd) 
{
    time_t now;
    char *cp;
    time(&now);
    cp = ctime(&now);
    write(fd, cp,
    strlen(cp));
}

main() 
{
    int sock;
    int fd;
    sock = make_server_socket(5123);
    if (sock == -1) 
    {
        exit(1);
    }
    while(1) 
    {
        fd = accept(sock, NULL, NULL);
        if ( fd == -1) break;
        process_request(fd);
        close(fd);
    }
} 