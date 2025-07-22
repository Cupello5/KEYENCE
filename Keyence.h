#pragma once
#include <vector>
#include <Windows.h>

typedef struct ProfilePoint
{
    double x;              // x-coordinat in mm - position along laser line
    double z;              // z-coordinat in mm - heingt (Reference system in Laser orign light)
    unsigned char intensity;
} ProfilePoint;

class Keyence {
public:
    Keyence(int num_point, double origin_height);
    ~Keyence();

    bool connect();
    bool acquire(std::vector<ProfilePoint>& profileData);

private:
    LONG lDeviceId;
    uint32_t MAX_PROFILE_POINTS;
    double HEIGHT_ORIGIN_LASER;
}; 