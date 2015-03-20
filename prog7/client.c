int connect_to_server(char *host, int portnum) 
{
    int sock;
    struct sockaddr_in servadd;
    struct hostent *hp;
    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if ( sock == -1 ) 
    {
        return -1;
    }
    bzero( &servadd, sizeof(servadd) );
    hp = gethostbyname( host );
    if (hp == NULL)
    { 
        return -1;
    }
    bcopy(hp->h_addr, (struct sockaddr *)&servadd.sin_addr,
    hp->h_length);
    servadd.sin_port = htons(portnum);
    servadd.sin_family = AF_INET ;
    if ( connect(sock,(struct sockaddr *)&servadd,
        sizeof(servadd)) !=0) 
    {
        return -1;
    }
    return sock;
}

talk_with_server(int fd) 
{
    char buf[LEN];
    int n;
    n = read(fd, buf, LEN);
    write(1, buf, LEN);
}

main(int argv, char *argc[]) 
{
    int fd;
    fd = connect_to_server(argc[1], 5123);
    if (fd == -1) 
    {
        exit(1);
    }
    talk_with_server(fd);
    close (fd);
}