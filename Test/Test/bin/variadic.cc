#include "boost/mpl/if.hpp"
#include <cassert>
#include <iostream>

//NOTE: Instead I could use a typed enum to denote the possible states
enum class Abilities { kA,kB,kC};
namespace AbilityBits { enum Bits { kA=1,kB=2,kC=4}; }


template<typename T>
struct A {  
   typedef T Type;
   static const Abilities kAbility=Abilities::kA;
   };
template<typename T>
struct B {
   typedef T Type;
   static const Abilities kAbility=Abilities::kB;
};
template<typename T>
struct C {
   typedef T Type;
   static const Abilities kAbility=Abilities::kC;
};

struct Empty {};

template<Abilities ABILITY, typename... VArgs> struct CheckIt;

template<Abilities ABILITY, typename T, typename... VArgs>
struct CheckIt<ABILITY,T,VArgs...> {
   static const bool kHasIt = (T::kAbility==ABILITY) | CheckIt<ABILITY,VArgs...>::kHasIt;
   typedef typename boost::mpl::if_c<(T::kAbility==ABILITY),
                                     T,
                                     typename CheckIt<ABILITY,VArgs...>::Type>::type Type;
};

template<Abilities ABILITY>
struct CheckIt<ABILITY> {
   static const bool kHasIt=false;
   typedef Empty Type;
};

template <typename T, typename BASE>
struct DoA : public BASE {
   static const unsigned char kDoesIt = BASE::kDoesIt | AbilityBits::kA;
};

template <typename T, typename BASE>
struct DoB : public BASE {
   static const unsigned char kDoesIt = BASE::kDoesIt | AbilityBits::kB;
};

template <typename T, typename BASE>
struct DoC : public BASE {
   static const unsigned char kDoesIt = BASE::kDoesIt | AbilityBits::kC;
};

struct Base {
   static const unsigned char kDoesIt = 0;
};

template<typename BASE, typename... VArgs>
struct LookForA {
   typedef CheckIt<Abilities::kA,VArgs...> Checker;
   
    typedef typename boost::mpl::if_c<Checker::kHasIt,
                                      DoA<typename Checker::Type,
                                          BASE>,
                                      BASE>::type type;
}; 


template<typename BASE, typename... VArgs>
struct LookForB {
   typedef CheckIt<Abilities::kB,VArgs...> Checker;
   typedef typename LookForA<BASE,VArgs...>::type LookForNext;
   typedef typename boost::mpl::if_c<Checker::kHasIt,
                                   DoB<typename Checker::Type,
                                       LookForNext>,
                                   LookForNext>::type type;

}; 


template<typename BASE, typename... VArgs>
struct LookForC {
   typedef CheckIt<Abilities::kC,VArgs...> Checker;
   typedef typename LookForB<BASE,VArgs...>::type LookForNext;
   typedef typename boost::mpl::if_c<Checker::kHasIt,
                                     DoC<typename Checker::Type,
                                         LookForNext>,
                                     LookForNext>::type type;
}; 

template<typename... VArgs>
struct DoStuff : public LookForC<Base,VArgs...>::type {
   typedef typename LookForC<Base,VArgs...>::type BaseClass;
   static constexpr bool doesA() { return BaseClass::kDoesIt & AbilityBits::kA;}
   static constexpr bool doesB() { return BaseClass::kDoesIt & AbilityBits::kB;}
   static constexpr bool doesC() { return BaseClass::kDoesIt & AbilityBits::kC;}
};

template<typename T>
void test(char const* iName, bool iDoesA, bool iDoesB, bool iDoesC)
{
   std::cout <<iName<<" doesA:"<<iDoesA<<"="<<T::doesA()<<" doesB:"<<iDoesB<<"="<<T::doesB()<<" doesC:"<<iDoesC<<"="<<T::doesC()<<std::endl;
   assert(iDoesA == T::doesA());
   assert(iDoesB == T::doesB());
   assert(iDoesC == T::doesC());
}

#define DOTEST(TYPE_,DOESA_,DOESB_,DOESC_) test<TYPE_>(#TYPE_, DOESA_,DOESB_,DOESC_)

int main()
{
   DOTEST(DoStuff<>,false,false,false);

   typedef DoStuff<A<int>> DoStuffA;
   DOTEST(DoStuffA,true,false,false);

   typedef DoStuff<B<int>> DoStuffB;
   DOTEST(DoStuffB,false,true,false);

   typedef DoStuff<C<int>> DoStuffC;
   DOTEST(DoStuffC,false,false,true);

   typedef DoStuff<A<int>,B<int>> DoStuffAB;
   DOTEST(DoStuffAB,true,true,false);
   typedef DoStuff<B<int>,A<int>> DoStuffBA;
   DOTEST(DoStuffBA,true,true,false);

   typedef DoStuff<A<int>,C<int>> DoStuffAC;
   DOTEST(DoStuffAC,true,false,true);
   typedef DoStuff<C<int>,A<int>> DoStuffCA;
   DOTEST(DoStuffCA,true,false,true);

   typedef DoStuff<B<int>,C<int>> DoStuffBC;
   DOTEST(DoStuffBC,false,true,true);
   typedef DoStuff<C<int>,B<int>> DoStuffCB;
   DOTEST(DoStuffCB,false,true,true);


   typedef DoStuff<A<int>,B<int>,C<int>> DoStuffABC;
   DOTEST(DoStuffABC,true,true,true);
   typedef DoStuff<A<int>,C<int>,B<int>> DoStuffACB;
   DOTEST(DoStuffACB,true,true,true);
   typedef DoStuff<B<int>,A<int>,C<int>> DoStuffBAC;
   DOTEST(DoStuffBAC,true,true,true);
   typedef DoStuff<B<int>,C<int>,A<int>> DoStuffBCA;
   DOTEST(DoStuffBCA,true,true,true);
   typedef DoStuff<C<int>,B<int>,A<int>> DoStuffCBA;
   DOTEST(DoStuffCBA,true,true,true);
   typedef DoStuff<C<int>,A<int>,B<int>> DoStuffCAB;
   DOTEST(DoStuffCAB,true,true,true);

   return 0;
}
                                          