#include "power.h"

using namespace std;

Power::Power() {
    powerSourceInfo();
}

void Power::dumpDict(CFDictionaryRef Dict) {
    CFDataRef xml = CFPropertyListCreateData(
            kCFAllocatorDefault,
            (CFPropertyListRef) Dict,
            kCFPropertyListXMLFormat_v1_0,
            1,
            nullptr
    );
    if (xml) {
        write(1, CFDataGetBytePtr(xml), CFDataGetLength(xml));
        CFRelease(xml);
    }
}


void Power::powerSourceInfo() {
    CFArrayRef powerSourcesList;
    powerInfo = IOPSCopyPowerSourcesInfo();
    powerSourcesList = IOPSCopyPowerSourcesList(powerInfo);

    // Should only get one source. But in practice, check for > 0 sources
    if (CFArrayGetCount(powerSourcesList)) {
        powerSourceInformation = IOPSGetPowerSourceDescription(
                powerInfo,
                CFArrayGetValueAtIndex(powerSourcesList, 0)
        );
    }

//    dumpDict(powerSourceInformation);
}

const char *Power::getBatteryHealth() {
    if (powerSourceInformation == nullptr)
        return UNKNOWN;

    auto batteryHealthRef = (CFStringRef) CFDictionaryGetValue(powerSourceInformation, CFSTR("BatteryHealth"));
    const char *batteryHealth = CFStringGetCStringPtr(batteryHealthRef, kCFStringEncodingMacRoman);
    if (batteryHealth == nullptr)
        return UNKNOWN;

    return batteryHealth;
}

uint32_t Power::getDesignCycleCount() {
    if (powerSourceInformation == nullptr) return 0;
    auto designCycleCountRef = (CFNumberRef) CFDictionaryGetValue(
            powerSourceInformation,
            CFSTR("DesignCycleCount")
    );


    uint32_t designCycleCount;
    if (!CFNumberGetValue(designCycleCountRef, kCFNumberSInt32Type, &designCycleCount))
        return 0;
    else
        return designCycleCount;
}

uint32_t Power::getCycleCount() {
    io_registry_entry_t entry;
    entry = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceNameMatching("AppleSmartBattery"));
    if (entry == IO_OBJECT_NULL) return 0;

    CFMutableDictionaryRef battery;
    IORegistryEntryCreateCFProperties(entry, &battery, nullptr, 0);
    auto cycleCountRef = (CFNumberRef) CFDictionaryGetValue(battery,CFSTR("CycleCount"));
    uint32_t cycleCount;
    CFNumberGetValue(cycleCountRef, kCFNumberSInt32Type, &cycleCount);
    CFRelease(battery);
    CFRelease(cycleCountRef);
    return cycleCount;
}

u_char Power::getBatteryCharge() {
    CFNumberRef currentCapacity;
    CFNumberRef maximumCapacity;

    int iCurrentCapacity;
    int iMaximumCapacity;

    if (powerSourceInformation == nullptr)
        return 0;

    currentCapacity = static_cast<CFNumberRef>(CFDictionaryGetValue(powerSourceInformation,
                                                                    CFSTR(kIOPSCurrentCapacityKey)));
    maximumCapacity = static_cast<CFNumberRef>(CFDictionaryGetValue(powerSourceInformation,
                                                                    CFSTR(kIOPSMaxCapacityKey)));

    CFNumberGetValue(currentCapacity, kCFNumberIntType, &iCurrentCapacity);
    CFNumberGetValue(maximumCapacity, kCFNumberIntType, &iMaximumCapacity);

    return (float)iCurrentCapacity / iMaximumCapacity * 100;
}

Power::~Power() {
    CFRelease(powerInfo);
    CFRelease(powerSourceInformation);
}

uint32_t Power::getDesignCapacity() {
    return 0;
}

uint32_t Power::getCapacity() {
    return 0;
}

