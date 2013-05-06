#ifndef GLOBALMODULEABILITIES_H_83813OZE
#define GLOBALMODULEABILITIES_H_83813OZE

#include "ModuleAbilityEnums.h"

namespace edm {
   //Used in the case where ability is not available
   struct Empty{};
   
   template<typename T>
   struct GlobalCache {
      static constexpr Abilities kAbilities=Abilities::kGlobalCache;
      typedef T Type;
   };

   template<typename T>
   struct StreamCache {
      static constexpr Abilities kAbilities=Abilities::kStreamCache;
      typedef T Type;
   };

   template<typename T>
   struct RunCache {
      static constexpr Abilities kAbilities=Abilities::kRunCache;
      typedef T Type;
   };

   template<typename T>
   struct LuminosityBlockCache {
      static constexpr Abilities kAbilities=Abilities::kLuminosityBlockCache;
      typedef T Type;
   };
   
   template<typename T>
   struct RunSummaryCache {
      static constexpr Abilities kAbilities=Abilities::kRunSummaryCache;
      typedef T Type;      
   };
   
   struct EndRunProducer {
      static constexpr Abilities kAbilities=Abilities::kEndRunProducer;
      typedef Empty Type;
   };
   
   
   //Recursively checks VArgs template arguments looking for the ABILITY
   template<Abilities ABILITY, typename... VArgs> struct CheckAbility;

   template<Abilities ABILITY, typename T, typename... VArgs>
   struct CheckAbility<ABILITY,T,VArgs...> {
      static constexpr bool kHasIt = (T::kAbilities==ABILITY) | CheckAbility<ABILITY,VArgs...>::kHasIt;
      typedef typename boost::mpl::if_c<(T::kAbilities==ABILITY),
                                        typename T::Type,
                                        typename CheckAbility<ABILITY,VArgs...>::Type>::type Type;
   };

   //End of the recursion
   template<Abilities ABILITY>
   struct CheckAbility<ABILITY> {
      static constexpr bool kHasIt=false;
      typedef Empty Type;
   };
}


#endif /* end of include guard: GLOBALMODULEABILITIES_H_83813OZE */
