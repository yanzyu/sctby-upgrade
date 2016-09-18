#include "appConfig.h"
#include "scatterConfig.h"
#include "bsw.h"

void HalInit(void);

const Bsw_t Bsw __attribute__((at(BSW_RO_Base))) = {
    .HalInit    = HalInit,
    .ClkConfig  = clkConfig,
    .GetTick    = HAL_GetTick,
    .Uart1Init  = InitUart1,
    .UartSend   = uartSend_IT,
    .UartSendString = uartSendString,
    .UartReceive    = uartReceive_Pkt
};

void HalInit() {
    HAL_Init();
}

