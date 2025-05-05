#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
const int PORT = 12345;
const char* SERVER_IP = "127.0.0.1";
SOCKET Connection;
const int MAX_BUFFER_SIZE = 1024;
char buffer[MAX_BUFFER_SIZE];
std::string playerInfo;
struct State {
    int hp = 100;
    int mana = 100;
    int gold = 20;
};
void clearScreen() {
    system("cls");
}
char checkDamage(std::vector<std::string>& arenaElements)
{
    char damage = '0';
    for (size_t i = 0; i < arenaElements.size(); i++)
    {
        if (arenaElements[i] == "Казарма")
        {
            damage += 2;
        }
    }
    return damage;
}
int checkDefense(std::vector<std::string>& arenaElements)
{
    char def = '0';
    for (size_t i = 0; i < arenaElements.size(); i++)
    {
        if (arenaElements[i] == "Башня")
        {
            def += 2;
        }
    }
    return def;
}
void sendToClient(std::string playerInfo) {
    int msg_size = playerInfo.size();
    send(Connection, (char*)&msg_size, sizeof(int), NULL);
    send(Connection, playerInfo.c_str(), msg_size, NULL);
    Sleep(10);
}
void displayArena(const State& st, const std::vector<std::string>& arenaElements) {
    std::cout << "Игра Castle Fight\n"
        "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"
        "|                                                              |\n"
        "|   Замок(Хп: " << st.hp << ")                                             |\n";
    for (const std::string& element : arenaElements) {
        std::cout << "|    " << element;
        for (size_t i = element.length(); i < 63; ++i) {
            std::cout << " ";
        }
        std::cout << "|\n";
    }
    std::cout << "----------------------------------------------------------------\n"
        "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
        "\n мана: " << st.mana << "\n золото : " << st.gold << std::endl;
    std::cout << "Выбери что ты хочешь сделать в свой ход:\n"
        "1. Построить казарму (30 золота)\n"
        "2. Построить башню (20 золота)\n"
        "3. Снести здание (+10 золота)\n"
        "4. Заклинания\n"
        "5. Закончить ход\n";
}
int buildBuilding(std::vector<std::string>& arenaElements, int buildingIndex, const std::string& buildingName) {
    if (buildingIndex >= 0 && buildingIndex < arenaElements.size() && arenaElements[buildingIndex] == "Пусто" && buildingName != "Пусто") {
        arenaElements[buildingIndex] = buildingName;
        if (buildingName == "Казарма")
            return 30;
        else
            return 20;
    }
    else if (buildingName == "Пусто" && arenaElements[buildingIndex] != "Пусто") {
        arenaElements[buildingIndex] = buildingName;
        return 10;
    }
    else {
        return 0;
    }
}
void ClientHandler() {
    int msg_size;
    while (true) {
        recv(Connection, (char*)&msg_size, sizeof(int), NULL);
        char* msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        recv(Connection, msg, msg_size, NULL);
        playerInfo = msg;
        delete[] msg;
    }
}
int main() {
    State st;
    int choice, spell, temp;
    int msg_size;
    bool flag = true;
    std::vector<std::string> arenaElements(5, "Пусто");
    int buildingIndex;
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if (WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error" << std::endl;
        exit(1);
    }
    SOCKADDR_IN addr;
    int sizeofaddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
        std::cout << "Error: failed connect to server.\n";
        return 1;
    }
    std::cout << "Connected!\n";
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
    while (st.hp > 0) {
        if (playerInfo[0] == '0')
        {
            while (flag)
            {
                char attack = std::atoi(&playerInfo[1]);
                char def = std::atoi(&playerInfo[2]);
                st.hp -= abs((attack - def));
                flag = false;
            }
            displayArena(st, arenaElements);
            std::cout << "Введите ваш выбор: ";
            std::cin >> choice;
            if (std::cin.fail()) {
                std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
                std::cin.clear();
            }
            if (choice < 1 || choice > 5)
                std::cout << "Неверный выбор. Пожалуйста, выберите опцию из списка.\n";
            clearScreen();
            switch (choice) {
            case 1:
                if (st.gold >= 30)
                {
                    std::cout << "Введите номер здания для постройки казармы (0-" << arenaElements.size() - 1 << "): ";
                    std::cin >> buildingIndex;
                    if (std::cin.fail()) {
                        std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
                        std::cin.clear();
                    }
                    temp = buildBuilding(arenaElements, buildingIndex, "Казарма");
                    if (temp == 0)
                        std::cout << "Неверный номер здания.\n";
                    else
                        st.gold -= temp;
                    clearScreen();
                }
                else
                    std::cout << "Вам не хватает золота" << std::endl;
                break;
            case 2:
                if (st.gold >= 20)
                {
                    std::cout << "Введите номер здания для постройки башни (0-" << arenaElements.size() - 1 << "): ";
                    std::cin >> buildingIndex;
                    if (std::cin.fail()) {
                        std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
                        std::cin.clear();
                    }
                    temp = buildBuilding(arenaElements, buildingIndex, "Башня");
                    if (temp == 0)
                        std::cout << "Неверный номер здания.\n";
                    else
                        st.gold -= temp;
                    clearScreen();
                }
                else
                    std::cout << "Вам не хватает золота или эта ячейка уже занята" << std::endl;
                break;
            case 3:
                std::cout << "Введите номер здания для сноса (0-" << arenaElements.size() - 1 << "): ";
                std::cin >> buildingIndex;
                if (std::cin.fail()) {
                    std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
                    std::cin.clear();
                }
                temp = buildBuilding(arenaElements, buildingIndex, "Пусто");
                if (temp == 0)
                    std::cout << "Неверный номер здания.\n";
                else
                    st.gold += temp;
                clearScreen();
                break;
            case 4:
                std::cout << "Выберите заклинания: \n";
                std::cout << "1. Ремонт(40 маны) \n";
                std::cin >> spell;
                if (std::cin.fail()) {
                    std::cout << "Неверный ввод. Пожалуйста, введите число.\n";
                    std::cin.clear();
                }
                switch (spell)
                {
                case 1:
                    if (st.mana >= 40)
                    {
                        st.hp += 25;
                        st.mana -= 40;
                        clearScreen();
                    }
                    else
                        std::cout << "Недостаточно маны \n";
                    break;
                default:
                    break;
                }
                break;
            case 5:
                std::cout << "Ожидание другого игрока.\n";
                playerInfo[0] = '1';
                playerInfo[1] = checkDamage(arenaElements);
                playerInfo[3] = checkDefense(arenaElements);
                st.gold += 20;
                st.mana += 10;
                flag = true;
                sendToClient(playerInfo);
                break;
            default:
                std::cout << "Неизвестная команда.\n";
                break;
            }
        }
    }
    clearScreen();
    std::cout << "Вы проиграли\n";
    system("pause");
    return 0;
}
