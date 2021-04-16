#include "power.h"
#include "smc.h"

int main() {
    Power p;
    Smc s;

    int i, fans = s.GetFanNumber(SMC_KEY_FAN_NUM);
    double bat_temp = s.GetTemperature(SMC_KEY_BATTERY_TEMP);
    printf("--- CPU Stats ---\n");
    printf("CPU Temp:  %0.1f °C\n\n", s.GetTemperature(SMC_KEY_CPU_TEMP));

    printf("--- Fan Stats ---\n");
    for (i = 0; i < fans; i++)
        printf("FAN_%i Speed: %0.1f RPM\n", i, s.GetFanSpeed(i));
    printf("\n");


    printf("--- Battery Stats ---\n");
    printf("Battery Health: %s\n", p.getBatteryHealth());
    printf("Cycle Count: %i/%i\n", Power::getCycleCount(), p.getDesignCycleCount());
    printf("Current Charge %i%%\n", p.getBatteryCharge());
    printf("Battery Temp %0.1f °C\n", bat_temp);
    return 0;
}