#include "Keyence.h"
#include <iostream>

int main() {
    Keyence keyence(3200, 276.8);
    if (!keyence.connect()) return -1;

    std::vector<ProfilePoint> profile_data;
    if (!keyence.acquire(profile_data)) return -1;
    

    // DEBUG
    for (size_t i = 0; i < profile_data.size(); ++i) {
        std::cout << "Punto " << i << ": x=" << profile_data[i].x
                  << " mm, z=" << profile_data[i].z
                  << " mm, intensity=" << static_cast<int>(profile_data[i].intensity) << std::endl;
    }

    return 0;
}