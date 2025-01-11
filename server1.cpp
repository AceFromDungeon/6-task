#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <arpa/inet.h>

using namespace std;

void logMessage(const sockaddr_in& clientAddress, const string& message) {
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddress.sin_port);
    cout << clientIP << ":" << clientPort << " -> " << message << endl;
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Ошибка создания сокета." << endl;
        return 1;
    }
    cout << "Серверный сокет создан." << endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Ошибка привязки сокета." << endl;
        close(serverSocket);
        return 1;
    }
    cout << "Сокет привязан к адресу." << endl;

    if (listen(serverSocket, 5) == -1) {
        cerr << "Ошибка прослушивания сокета." << endl;
        close(serverSocket);
        return 1;
    }
    cout << "Сервер ожидает подключений на порту 8080..." << endl;

    sockaddr_in clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLen);
    if (clientSocket == -1) {
        cerr << "Ошибка подключения клиента." << endl;
        close(serverSocket);
        return 1;
    }

    cout << "Клиент подключился." << endl;

    srand(time(0));
    int numberToGuess = rand() % 100 + 1;
    int attempts = 0;
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int received = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (received == -1) {
            cerr << "Ошибка приема данных." << endl;
            break;
        }

        string clientMessage(buffer);
        logMessage(clientAddress, "Клиент прислал: " + clientMessage);

        int guess;
        try {
            guess = stoi(clientMessage);
        } catch (...) {
            string errorMessage = "Пожалуйста, отправьте корректное число.";
            send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
            logMessage(clientAddress, "Сервер отправил: " + errorMessage);
            continue;
        }

        attempts++;

        if (guess < numberToGuess) {
            string response = "Больше!";
            send(clientSocket, response.c_str(), response.length(), 0);
            logMessage(clientAddress, "Сервер отправил: " + response);
        } else if (guess > numberToGuess) {
            string response = "Меньше!";
            send(clientSocket, response.c_str(), response.length(), 0);
            logMessage(clientAddress, "Сервер отправил: " + response);
        } else {
            string successMessage = "Поздравляем! Вы угадали число за " + to_string(attempts) + " попыток.";
            send(clientSocket, successMessage.c_str(), successMessage.length(), 0);
            logMessage(clientAddress, "Сервер отправил: " + successMessage);
            break;
        }
    }

    close(clientSocket);
    close(serverSocket);
    cout << "Сервер завершил работу." << endl;

    return 0;
}
