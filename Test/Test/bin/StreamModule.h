#ifndef STREAMMODULE_H_N15KSLWP
#define STREAMMODULE_H_N15KSLWP

#include "streamAddAbilityIf.h"

template<typename... VArgs>
class StreamModule : public stream::AddAbilitiesIf<StreamModuleBase,VArgs...>::type {
   
};

#endif /* end of include guard: STREAMMODULE_H_N15KSLWP */
