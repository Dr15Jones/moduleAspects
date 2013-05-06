#ifndef GLOBALMODULE_H_N15KSLWP
#define GLOBALMODULE_H_N15KSLWP

#include "globalAddAbilityIf.h"

template<typename... VArgs>
class GlobalModule : public global::AddAbilitiesIf<GlobalModuleBase,VArgs...>::type {
   
};

#endif /* end of include guard: GLOBALMODULE_H_N15KSLWP */
