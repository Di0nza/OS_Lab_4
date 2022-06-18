#include <windows.h>
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {

    HANDLE _event = OpenEvent(EVENT_ALL_ACCESS, FALSE, reinterpret_cast<LPCSTR>(L"message"));
    HANDLE _s1 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, reinterpret_cast<LPCSTR>(L"ready"));
    HANDLE _s2 = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, reinterpret_cast<LPCSTR>(L"filled"));
    ReleaseSemaphore(_s1, 1, NULL);

    while (true) {
        std::cout << "Enter 's' to send a message or 'e' to exit" << std::endl;
        char command;
        std::string message;
        std::string line;
        std::cin >> command;
        if (command == 's') {
            if (WaitForSingleObject(_s2, 0) != WAIT_OBJECT_0) {
                std::cout << "There is no space for a message." << std::endl;
                WaitForSingleObject(_s2, INFINITE);
            }

            std::cout << "Message: " << std::endl;
            do {
                std::cin >> line;
                if (line.size() > 20) {
                    std::cout << "string length must be less than 20\n";
                }
            } while (line.size() < 21);
            message = line;
            unsigned strLen = message.size();
            std::ofstream fout(argv[1], std::ios::binary | std::ios::out | std::ios::app);
            fout.write((char *) &strLen, sizeof(unsigned));
            fout.write((char *) message.c_str(), message.size());
            fout.close();
            PulseEvent(_event);
        } else if(command == 'e'){
            return 0;
        }else{
            continue;
        }
    }
}