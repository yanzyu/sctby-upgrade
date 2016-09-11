
#include "appConfig.h"

const moduleInfo_t moduleTab[MODULE_ID_COUNT] = {
    { INIT_RO_Base,    INIT_RO_Size,  ((INIT_Ver_MAJOR << 8) | INIT_Ver_MINOR)    },
    { DESC_RO_Base,    DESC_RO_Size,  ((DESC_Ver_MAJOR << 8) | DESC_Ver_MINOR)    },
    { BSW_RO_Base,     BSW_RO_Size,   ((BSW_Ver_MAJOR << 8) | BSW_Ver_MINOR)      },
    { LED_RO_Base,     LED_RO_Size,   ((LED_Ver_MAJOR << 8) | LED_Ver_MINOR)      }
};

const moduleDesc_t desc __attribute__((at(DESC_RO_Base))) = {
    MODULE_ID_COUNT,
    (moduleInfo_t*) moduleTab
};



