#include <iostream>
#include <windows.h>
#include <process.h>
#include <string>
#include <fstream>
CRITICAL_SECTION cs;
const size_t MAX_MESSAGE_SIZE = 20;
int main() {

    InitializeCriticalSection(&cs);
    char* fileName = new char[MAX_MESSAGE_SIZE];
    std::cout << "Enter name of file:" << std::endl;
    std::cin >> fileName;

    int sendersNumber;
    std::cout << "Enter amount of senders:" << std::endl;
    std::cin >> sendersNumber;

    HANDLE* _events = new HANDLE[sendersNumber];
    HANDLE* _senders = new HANDLE[sendersNumber];
    STARTUPINFO* si = new STARTUPINFO[sendersNumber];
    PROCESS_INFORMATION* pi = new PROCESS_INFORMATION[sendersNumber];
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), 0, TRUE };
    HANDLE _startEvent = CreateEvent(&sa, FALSE, FALSE, reinterpret_cast<LPCSTR>(LPCWSTR("startEvent")));

    for (int i = 0; i < sendersNumber; i++) {
        char* dataForSender = new char[MAX_MESSAGE_SIZE];
        strcpy(dataForSender, "");
        strcat(dataForSender, "Sender.exe");
        strcat(dataForSender, fileName);
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
        char* tmp = new char[20];
        char p[10];
        strcpy(tmp, "Event");
        strcat(tmp, itoa(i, p, 10));
        strcat(dataForSender, " ");
        strcat(dataForSender, tmp);
        _events[i] = CreateEvent(&sa, FALSE, FALSE, reinterpret_cast<LPCSTR>(LPCWSTR(tmp)));
        if (!CreateProcess(nullptr, reinterpret_cast<LPSTR>(LPWSTR(dataForSender)),
                           nullptr, nullptr, TRUE,
                           CREATE_NEW_CONSOLE, nullptr,
                           nullptr, &si[i], &pi[i])) {
            std::cout << "Error! The process is not created" << std::endl;;
            return GetLastError();
        }
        _senders[i] = pi[i].hProcess;
    }

    WaitForMultipleObjects(sendersNumber, _events, TRUE, INFINITE);
    SetEvent(_startEvent);

    while (WaitForMultipleObjects(sendersNumber, _senders, TRUE, 0) == WAIT_TIMEOUT) {
        EnterCriticalSection(&cs);
        std::cout << "Input 'r' for try to read the file: " << std::endl;
        char* command;
        std::cin >> command;
        if (strcmp(command, "r")) {
            break;
        }
        std::ifstream fin;
        if (fin.is_open() != false) {
            fin.open(fileName, std::fstream::binary);
            char* message;
            for (int i = 0;i < 2;i++) {
                fin.read(message, sizeof(message));
                std::cout << message << " ";
            }
            fin.close();
        }
        else {
            std::cout << "Error! The file is not open" << std::endl;
        }
        LeaveCriticalSection(&cs);
    }

    for (int i = 0; i < sendersNumber; i++) {
        CloseHandle(pi[i].hThread);
        CloseHandle(pi[i].hProcess);
        CloseHandle(_events[i]);
    }

    CloseHandle(_startEvent);
    return 0;
}