#include <string.h>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


using namespace std;

int main()
{

    int bufSize = 4092;

    char buffer[bufSize];
    struct sockaddr_in server_addr;

    int listen_socket = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP); // TCP protocol
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8008); // port of localhost
    server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    cout << "Binding"<< endl;
    //bind the host
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        return 0;
    }

    cout << "Listening"<< endl;
    listen(listen_socket,5); // number of tcp connections

while(true)
    {
    cout << "Accept"<< endl;
    int client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("ERROR on accept");
        continue; //return 0;
    }
    cout << "Go to http://127.0.0.1:8008/" << endl;
    int result = recv(client_socket, buffer, bufSize, 0);
    int k = 0;
    string s(buffer);
    s = s.substr(4,bufSize);
    while(s[k] != ' '){
      k+=1;
    }

    string filen = s.substr(0,k);
    stringstream response;
    stringstream response_body;

    if (result < 0) {
        perror("ERROR on recv");
        continue; // return 0;
    } else if (result == 0) {
        cout << "connection closed...\n";
    } else if (result > 0) {
        cout << "Response: " << result << endl;
        buffer[result] = '\0'; // in the end because we know the lenght
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        string p(cwd);

        char *buff;
        long size;
        ifstream file(p+filen, ios::in|ios::binary|ios::ate);
        size = file.tellg();
        file.seekg (0, ios::beg);
        buff = new char[size];
        file.read(buff, size);
        buff[file.gcount()] = '\0';
        file.close();
        cout <<buffer<<endl;
        response_body << buff;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Version: HTTP/1.1\r\n"
                 << "Content-Type: text/html; charset=utf-8\r\n"
                 << "Content-Length: " << response_body.str().length()
                 << "\r\n\r\n"
                 << response_body.str();
        result = send(client_socket, response.str().c_str(),
                      response.str().length(), 0);

        if (result < 0) {
            perror("Error send failed");
        }
      }
        shutdown(client_socket, SHUT_WR);
        char buf[64];
        while (read(client_socket, buf, 64) > 0);
        close(client_socket);
    }
    shutdown(listen_socket, SHUT_RD);
    char buf[64];
    while (read(listen_socket, buf, 64) > 0);
    close(listen_socket);
    return 0;
}

