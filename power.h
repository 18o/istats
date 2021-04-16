
#ifndef __POWER_H__
#define __POWER_H__

#include <iostream>
#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/ps/IOPowerSources.h>

#define VERSION               "0.01"


#define UNKNOWN "Unknown"


class Power {
public:
    Power();

    ~Power();

    const char *getBatteryHealth();

    uint32_t getDesignCycleCount();
    static uint32_t getCycleCount();

    u_char getBatteryCharge();

    uint32_t getDesignCapacity();
    uint32_t getCapacity();

private:
    CFDictionaryRef powerSourceInformation{};
    CFTypeRef powerInfo{};


    void powerSourceInfo();

    static void dumpDict(CFDictionaryRef Dict);
};

#endif // __POWER_H__