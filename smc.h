#ifndef SMC_SMC_H
#define SMC_SMC_H

#include <IOKit/IOKitLib.h>
#include "iostream"

#define DATATYPE_FPE2         "fpe2"
#define DATATYPE_UINT8        "ui8 "
#define DATATYPE_UINT16       "ui16"
#define DATATYPE_UINT32       "ui32"
#define DATATYPE_SP78         "sp78"
#define DATATYPE_FLT          "flt "



// key values
#define SMC_KEY_CPU_TEMP      "TC0P"
#define SMC_KEY_FAN_SPEED     "F%dAc"
#define SMC_KEY_MIN_FAN_SPEED "F%dMn"
#define SMC_KEY_MAX_FAN_SPEED "F%dMx"
#define SMC_KEY_FAN_NUM       "FNum"
#define SMC_KEY_BATTERY_TEMP  "TB0T"


#define KERNEL_INDEX_SMC      2

#define SMC_CMD_READ_BYTES    5
#define SMC_CMD_WRITE_BYTES   6
#define SMC_CMD_READ_INDEX    8
#define SMC_CMD_READ_KEYINFO  9
#define SMC_CMD_READ_PLIMIT   11
#define SMC_CMD_READ_VERS     12

typedef char UInt32Char_t[5];
typedef char SMCBytes_t[32];

typedef struct {
    UInt32Char_t key;
    UInt32 dataSize;
    UInt32Char_t dataType;
    SMCBytes_t bytes;
} SMCVal_t;

typedef struct {
    char major;
    char minor;
    char build;
    char reserved[1];
    UInt16 release;
} SMCKeyData_vers_t;

typedef struct {
    UInt16 version;
    UInt16 length;
    UInt32 cpuPLimit;
    UInt32 gpuPLimit;
    UInt32 memPLimit;
} SMCKeyData_pLimitData_t;

typedef struct {
    UInt32 dataSize;
    UInt32 dataType;
    char dataAttributes;
} SMCKeyData_keyInfo_t;


typedef struct {
    UInt32 key;
    SMCKeyData_vers_t vers;
    SMCKeyData_pLimitData_t pLimitData;
    SMCKeyData_keyInfo_t keyInfo;
    char result;
    char status;
    char data8;
    UInt32 data32;
    SMCBytes_t bytes;
} SMCKeyData_t;

typedef union data {
    float f;
    char b[4];
} fltUnion;


class Smc {
public:
    Smc();

    ~Smc();

    int GetFanNumber(const char *key);

    float GetFanSpeed(int fanNum);

    double GetTemperature(const char *key);

private:
    kern_return_t SMCOpen();

    kern_return_t ReadKey(const UInt32Char_t key, SMCVal_t *val);

    kern_return_t Call(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure) const;

    static void ultostr(char *str, UInt32 val);

    static UInt32 strtoul(const char *str, int size, int base);

    static float flttof(const unsigned char *str1);

    io_connect_t conn{};

};


#endif //SMC_SMC_H
