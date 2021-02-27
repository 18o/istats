#include "smc.h"

Smc::Smc() {
    SMCOpen();
}

Smc::~Smc() {
    IOServiceClose(conn);
}

kern_return_t Smc::SMCOpen() {
    kern_return_t result;
    mach_port_t masterPort;
    io_iterator_t iterator;
    io_object_t device;

    IOMasterPort(MACH_PORT_NULL, &masterPort);

    CFMutableDictionaryRef matchingDictionary = IOServiceMatching("AppleSMC");
    result = IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
    if (result != kIOReturnSuccess) {
        printf("Error: IOServiceGetMatchingServices() = %08x\n", result);
        return 1;
    }

    device = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (device == 0) {
        printf("Error: no SMC found\n");
        return 1;
    }

    result = IOServiceOpen(device, mach_task_self(), 0, &conn);
    IOObjectRelease(device);
    if (result != kIOReturnSuccess) {
        printf("Error: IOServiceOpen() = %08x\n", result);
        return 1;
    }

    return kIOReturnSuccess;
}


int Smc::GetFanNumber(const char *key) {
    SMCVal_t val;
    ReadKey(key, &val);
    return strtoul((char *) val.bytes, val.dataSize, 10);
}

kern_return_t Smc::ReadKey(const UInt32Char_t key, SMCVal_t *val) {
    kern_return_t result;
    SMCKeyData_t inputStructure;
    SMCKeyData_t outputStructure;

    memset(&inputStructure, 0, sizeof(SMCKeyData_t));
    memset(&outputStructure, 0, sizeof(SMCKeyData_t));
    memset(val, 0, sizeof(SMCVal_t));

    inputStructure.key = strtoul(key, 4, 16);
    inputStructure.data8 = SMC_CMD_READ_KEYINFO;

    result = Call(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;

    val->dataSize = outputStructure.keyInfo.dataSize;
    ultostr(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = SMC_CMD_READ_BYTES;

    result = Call(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;

    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));

    return kIOReturnSuccess;
}

kern_return_t Smc::Call(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure) const {
    size_t structureInputSize;
    size_t structureOutputSize;

    structureInputSize = sizeof(SMCKeyData_t);
    structureOutputSize = sizeof(SMCKeyData_t);

    return IOConnectCallStructMethod(conn, index, inputStructure, structureInputSize, outputStructure,
                                     &structureOutputSize);
}

float Smc::GetFanSpeed(int fanNum) {
    SMCVal_t val;
    kern_return_t result;
    UInt32Char_t key;
    sprintf(key, SMC_KEY_FAN_SPEED, fanNum);

    result = ReadKey(key, &val);
    if (result == kIOReturnSuccess) {
        // read succeeded - check returned value
        if (val.dataSize > 0) {
            if (strcmp(val.dataType, DATATYPE_FPE2) == 0) {
                // convert fpe2 value to rpm
                int intValue = (unsigned char) val.bytes[0] * 256 + (unsigned char) val.bytes[1];
                return intValue / 4.0;
            } else if (strcmp(val.dataType, DATATYPE_FLT) == 0) {
                // 2018 models have the ftp type for this key
                return flttof((unsigned char *) val.bytes);
            }
        }
    }
    // read failed
    return 0;
}

double Smc::GetTemperature(const char *key) {
    SMCVal_t val;
    kern_return_t result;

    result = ReadKey(key, &val);
    if (result == kIOReturnSuccess) {
        // read succeeded - check returned value
        if (val.dataSize > 0) {
            if (strcmp(val.dataType, DATATYPE_SP78) == 0) {
                // convert fp78 value to temperature
                int intValue = (val.bytes[0] * 256 + val.bytes[1]) >> 2;
                return intValue / 64.0;
            }
        }
    }
    // read failed
    return 0.0;
}


void Smc::ultostr(char *str, UInt32 val) {
    str[0] = '\0';
    sprintf(str, "%c%c%c%c",
            (unsigned int) val >> 24,
            (unsigned int) val >> 16,
            (unsigned int) val >> 8,
            (unsigned int) val);
}


UInt32 Smc::strtoul(const char *str, int size, int base) {
    UInt32 total = 0;
    int i;

    for (i = 0; i < size; i++) {
        if (base == 16)
            total += str[i] << (size - 1 - i) * 8;
        else
            total += (unsigned char) (str[i] << (size - 1 - i) * 8);
    }
    return total;
}

float Smc::flttof(const unsigned char *str1) {
    fltUnion flt;
    flt.b[0] = str1[0];
    flt.b[1] = str1[1];
    flt.b[2] = str1[2];
    flt.b[3] = str1[3];

    return flt.f;
}