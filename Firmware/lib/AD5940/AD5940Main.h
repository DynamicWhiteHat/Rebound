#ifndef AD5940MAIN_H
#define AD5940MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* One-time AD5940 clock/FIFO/interrupt/GPIO setup. Call once from
 * setup(), after AD5940() (the port init) has run. */
int32_t AD5940PlatformCfg(void);

/* Sets AppBIACfg parameters (ODR, DFT length, sweep, etc). Call once
 * from setup(), before AppBIAInit(). Edit the values inside this
 * function (in AD5940Main.c) to tune your measurement, not here. */
void AD5940BIAStructInit(void);

/* Call from loop() after AppBIAISR() to print/log a batch of results.
 * Feel free to replace the body of this with your own data handling -
 * it's an example, not something you're required to keep using. */
int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount);

#ifdef __cplusplus
}
#endif

#endif // AD5940MAIN_H