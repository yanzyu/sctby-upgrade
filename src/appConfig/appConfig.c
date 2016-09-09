
#include "appConfig.h"

const moduleInfo_t moduleTab[MODULE_ID_COUNT] = {

};

moduleDesc_t desc __attribute__((at(DESC_Base))) = {
    MODULE_ID_COUNT,
    (moduleInfo_t*) moduleTab
};






