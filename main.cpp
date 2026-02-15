#include <iostream>
#include <string>

int main() {
    std::string deviceName = "LED Screen Main";
    bool isPoweredOn = true;

    std::cout << "System Status Check..." << std::endl;

    if (isPoweredOn) {
        std::cout << deviceName << " is READY." << std::endl;
    } else {
        std::cout << deviceName << " is OFF." << std::endl;
    }

    return 0;
}