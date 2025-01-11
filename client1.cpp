#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <limits>

using namespace std;

int getGuessFromUser() {
    int guess;
    cout << "Введите число от 1 до 100: ";
    while (true) {
        cin >> guess;
        if (cin.fail() || guess < 1 || guess > 100) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Пожалуйста, введите число от 1 до 100: ";
        } else {
            break;
        }
    }
    return guess;
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Ошибка создания сокета." << endl;
        return 1;
    }
    cout << "Клиентский сокет создан." << endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Ошибка подключения к серверу." << endl;
        close(clientSocket);
        return 1;
    }
    cout << "Подключение к серверу успешно." << endl;

    while (true) {
        int guess = getGuessFromUser();

        send(clientSocket, to_string(guess).c_str(), to_string(guess).length(), 0);

        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Ответ от сервера: " << buffer << endl;

        if (strstr(buffer, "Поздравляем") != nullptr) {
            break;
        }
    }

    close(clientSocket);
    cout << "Клиент завершил работу." << endl;

    return 0;
}
