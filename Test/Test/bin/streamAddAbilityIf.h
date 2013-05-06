#ifndef STREAMADDABILITYIF_H_H4EEJ77H
#define STREAMADDABILITYIF_H_H4EEJ77H

#include "boost/mpl/if.hpp"
#include "ModuleAbilities.h"
#include "StreamModuleBase.h"

namespace stream {
   template<typename BASE, typename... VArgs>
   struct AddGlobalCacheIf {
      typedef edm::CheckAbility<Abilities::kGlobalCache,VArgs...> Checker;
      
      typedef typename boost::mpl::if_c<Checker::kHasIt,
                                        GlobalCacheHolder<typename Checker::Type,BASE>,
                                        BASE>::type type;
   };

   template<typename BASE, typename... VArgs>
   struct AddRunCacheIf {
      typedef edm::CheckAbility<Abilities::kRunCache,VArgs...> Checker;
      typedef typename AddGlobalCacheIf<BASE,VArgs...>::type AddNextIf;
      
      typedef typename boost::mpl::if_c<Checker::kHasIt,
                                        RunCacheHolder<typename Checker::Type,AddNextIf>,
                                        AddNextIf>::type type;
   };

   template<typename BASE, typename... VArgs>
   struct AddRunSummaryCacheIf {
      typedef edm::CheckAbility<Abilities::kRunSummaryCache,VArgs...> Checker;
      typedef typename AddRunCacheIf<BASE,VArgs...>::type AddNextIf;
      
      typedef typename boost::mpl::if_c<Checker::kHasIt,
                                        RunSummaryCacheHolder<typename Checker::Type,AddNextIf>,
                                        AddNextIf>::type type;
   };

   template<typename BASE, typename... VArgs>
   struct AddEndRunProducerIf {
      typedef edm::CheckAbility<Abilities::kEndRunProducer,VArgs...> Checker;
      typedef typename AddRunSummaryCacheIf<BASE,VArgs...>::type AddNextIf;
      
      typedef typename boost::mpl::if_c<Checker::kHasIt,
                                        EndRunProducerEnabler<AddNextIf>,
                                        AddNextIf>::type type;
   };


   template<typename BASE, typename... VArgs>
   struct AddLuminosityBlockCacheIf {
      typedef edm::CheckAbility<Abilities::kLuminosityBlockCache,VArgs...> Checker;
      typedef typename AddEndRunProducerIf<BASE,VArgs...>::type AddNextIf;
      
      typedef typename boost::mpl::if_c<Checker::kHasIt,
                                        LumiCacheHolder<typename Checker::Type,AddNextIf>,
                                        AddNextIf>::type type;
   };
   
   template<typename BASE, typename... VArgs>
   using AddAbilitiesIf = AddLuminosityBlockCacheIf<BASE,VArgs...>;

}
#endif /* end of include guard: STREAMADDABILITYIF_H_H4EEJ77H */
