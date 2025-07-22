#include "Keyence.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "LJX8_IF.h"
#include "LJX8_ErrorCode.h"
#pragma comment(lib, "LJX8_IF.lib")


Keyence::Keyence(int num_point, double origin_height) {
    lDeviceId = 0; 
    MAX_PROFILE_POINTS = num_point;
    HEIGHT_ORIGIN_LASER = origin_height;
}

Keyence::~Keyence() {
    LJX8IF_CommunicationClose(lDeviceId);
    LJX8IF_Finalize();
}


// **********************************************************************************************************
bool Keyence::connect() {
    // --- CONFIGURATION CONTROLLER NETWORK---
    LJX8IF_ETHERNET_CONFIG config = {};
    config.abyIpAddress[0] = 192;
    config.abyIpAddress[1] = 168;
    config.abyIpAddress[2] = 0;
    config.abyIpAddress[3] = 1;
    config.wPortNo = 24691;

    // --- INIZIALIZATION LIBRARY ---
    if (LJX8IF_Initialize() != 0) {
        std::cerr << "Error initialization of DLL LJX8IF failed\n";
        return false;
    }
    std::cout << "Library initialized successfully." << std::endl;

    // --- OPEN ETHERNET CONNECTION ---
    if (LJX8IF_EthernetOpen(lDeviceId, &config) != 0) {
        std::cerr << "Error ethernet connection\n";
        LJX8IF_Finalize();
        return false;
    }
    std::cout << "Ethernet connection established." << std::endl;


    // --- GET LASER HEAD MODEL (OPTIONAL) ---
    CHAR headModel[256];
    LONG result_tmp = LJX8IF_GetHeadModel(lDeviceId, headModel);
    if (result_tmp == 0) {
        std::cout << "Connected to head model: " << headModel << std::endl;
    }

    // --- TURN ON LASER ---  
    if (LJX8IF_ControlLaser(lDeviceId, 1) != 0) {
        std::cerr << "Error turn ON laser\n";
        return false;
    }
    std::cout << "Laser ON.\n";

    return true;
}


// **********************************************************************************************************
bool Keyence::acquire(std::vector<ProfilePoint>& profileData) {
    
    std::cout << "CLEAN MEMORY...\n";
    LJX8IF_ClearMemory(lDeviceId);

    // Software trigger 
    std::cout << "Send software trigger...\n";
    LONG result_trigger = LJX8IF_Trigger(lDeviceId);
    if (result_trigger != 0) {
        std::cerr << "Error software trigger software: code 0x" << std::hex << result_trigger << std::endl;
        return false;
    }
    std::cout << "Software Trigger OK!\n";

    // --- PROFILE SETTING ---
    LJX8IF_GET_PROFILE_REQUEST req = {};
    req.byTargetBank = 0;          // 0 = bank active
    req.byPositionMode = 0;        // 0 = current profile
    req.dwGetProfileNo = 0;        // Ignored if byPositionMode=0
    req.byGetProfileCount = 1;     // Acquired only 1 profile
    req.byErase = 0;               // Not erase after reading 

    // --- BUFFER AND STRUCTURE RESPONCE ---
    const uint32_t withLumi = 1;    // 1: data lunimance yes
    LJX8IF_GET_PROFILE_RESPONSE rsp = {};
    LJX8IF_PROFILE_INFO profInfo = {};
    size_t dataSize = sizeof(LJX8IF_PROFILE_HEADER)
        + sizeof(LJX8IF_PROFILE_FOOTER)
        + sizeof(uint32_t) * MAX_PROFILE_POINTS * (1 + withLumi);
    dataSize *= req.byGetProfileCount; // if more profiles

    size_t dataNumIn4byte = dataSize / sizeof(uint32_t);
    std::vector<DWORD> profdata(dataNumIn4byte);


    // --- PROFILE REQUEST ---
    LONG result = LJX8IF_GetProfile(
        lDeviceId,
        &req,
        &rsp,
        &profInfo,
        profdata.data(),
        (DWORD)dataSize
    ); 

    std::cout << "LJX8IF_GetProfile: " << std::hex << result << std::dec << std::endl;
    if (result != 0) {
        std::cout << "Failed to get profile." << std::endl;
        return false;
    }

    size_t headerSize = sizeof(LJX8IF_PROFILE_HEADER);
    size_t addr_height = headerSize / sizeof(uint32_t);
    size_t addr_lumi = addr_height + profInfo.wProfileDataCount;
    profileData.clear();

    for (int i = 0; i < profInfo.wProfileDataCount; ++i) {
        double x_val_mm = (profInfo.lXStart + profInfo.lXPitch * i) / 100.0 / 1000.0;
        int32_t z_val = static_cast<int32_t>(profdata[addr_height + i]);
        if (z_val <= -2147483645) continue; // Skip point not valid 
        double z_val_mm = z_val / 100.0 / 1000.0;
        unsigned char intensity = 0;
        if (withLumi) {
            intensity = static_cast<unsigned char>(profdata[addr_lumi + i] & 0xFF);
        }
        ProfilePoint pt;
        pt.x = x_val_mm;
        pt.z = HEIGHT_ORIGIN_LASER - z_val_mm; // Reference system in Laser orign light
        pt.intensity = intensity;
        profileData.push_back(pt);
    }
    std::cout << "Acquired " << profileData.size() << " valid points." << std::endl;

    return true;
}