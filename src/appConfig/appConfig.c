
#include "appConfig.h"
#include "scatterConfig.h"

const ModuleInfo_t moduleTab[MODULE_ID_COUNT] = {
    { DESC_RO_BASE,    DESC_RO_SIZE,  ((DESC_Ver_MAJOR << 8) | DESC_Ver_MINOR) },
    { INIT_RO_BASE,    INIT_RO_SIZE,  ((INIT_Ver_MAJOR << 8) | INIT_Ver_MINOR) },
    { BSW_RO_BASE,     BSW_RO_SIZE,   ((BSW_Ver_MAJOR << 8) | BSW_Ver_MINOR)   },
    { LED_RO_BASE,     LED_RO_SIZE,   ((LED_Ver_MAJOR << 8) | LED_Ver_MINOR)   }
};

const ModuleDesc_t desc __attribute__((at(DESC_RO_BASE))) = {
    ((VER_MAJOR << 8) | VER_MINOR),
    APP_ENTRY,
    FIRM_UNCHECK,
    FIRM_UNCHECK,
    MODULE_ID_COUNT,
    (ModuleInfo_t*) moduleTab
};




