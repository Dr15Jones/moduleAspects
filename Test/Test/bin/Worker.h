#ifndef WORKER_H_35Z4BV3E
#define WORKER_H_35Z4BV3E

#include "IDs.h"
#include "ModuleAbilityEnums.h"

class WorkerBase {
public:
   virtual void beginRun(RunIndex const&) =0;
   virtual void endRun(RunIndex const&) =0;
   virtual bool needsBeginRunTransition() const = 0;
   
   virtual void beginLumi(LumiIndex const&)=0;
   virtual void endLumi(LumiIndex const&)=0;
   virtual bool needsBeginLumiTransition() const = 0;

   virtual void beginStream(StreamID const&)=0;
   virtual void endStream(StreamID const&)=0;
   virtual bool needsBeginStreamTransition() const=0;
   virtual bool needsEndStreamTransition() const=0;
   
   virtual void setNumberOfStreams(unsigned int)=0;
   
   virtual void streamBeginRun(StreamID const& iID, RunIndex const& iIndex)=0;
   virtual void streamBeginLumi(StreamID const& iID, LumiIndex const& iIndex)=0;
   virtual void streamEndRun(StreamID const& iID, RunIndex const& iIndex)=0;
   virtual void streamEndLumi(StreamID const& iID, LumiIndex const& iIndex)=0;
   
   virtual void beginJob() = 0;
   virtual void endJob() = 0;
};


template<typename T>
struct DoCallBeginRun {
   static void doit(T* iModule, RunIndex const& iIndex) {
      std::cout<<"  DoCallBeginRun"<<std::endl;
      iModule->doGlobalBeginRun(iIndex);
   }
};

template<typename T>
struct DoCallBeginLumi {
   static void doit(T* iModule, LumiIndex const& iIndex) {
      std::cout<<"  DoCallBeginLumi"<<std::endl;
      iModule->doGlobalBeginLumi(iIndex);
   }
};

template<typename T>
struct DoCallBeginStream {
   static void doit(T* iModule, StreamID const& iIndex) {
      std::cout<<"  DoCallBeginStream"<<std::endl;
      iModule->doBeginStream(iIndex);
   }
};

template<typename T>
struct DoCallEndStream {
   static void doit(T* iModule, StreamID const& iIndex) {
      std::cout<<"  DoCallEndStream"<<std::endl;
      iModule->doEndStream(iIndex);
   }
};


template<typename T>
struct DoCallStreamBeginRun {
   static void doit(T* iModule, StreamID const& iIndex, RunIndex const& iRI) {
      std::cout<<"  DoCallStreamBeginRun"<<std::endl;
      iModule->doStreamBeginRun(iIndex, iRI);
   }
};

template<typename T>
struct DoCallStreamEndRun {
   static void doit(T* iModule, StreamID const& iIndex, RunIndex const& iRI) {
      std::cout<<"  DoCallStreamEndRun"<<std::endl;
      iModule->doStreamEndRun(iIndex, iRI);
   }
};

template<typename T>
struct DoCallStreamBeginLumi {
   static void doit(T* iModule, StreamID const& iIndex, LumiIndex const& iLI) {
      std::cout<<"  DoCallStreamBeginLumi"<<std::endl;
      iModule->doStreamBeginLumi(iIndex, iLI);
   }
};

template<typename T>
struct DoCallStreamEndLumi {
   static void doit(T* iModule, StreamID const& iIndex, LumiIndex const& iLI) {
      std::cout<<"  DoCallStreamEndLumi"<<std::endl;
      iModule->doStreamEndLumi(iIndex, iLI);
   }
};

template<typename T>
struct DoCallEndRun {
   static void doit(T* iModule, RunIndex const& iIndex) {
      std::cout<<"  DoCallEndRun"<<std::endl;
      iModule->doGlobalEndRun(iIndex);
   }
};


template<typename T, typename U>
struct DoNothing {
   static void doit(T*, U){ //std::cout <<"DoNothing"<<std::endl;
   }
};

template<typename T, typename U, typename V>
struct DoNothing2 {
   static void doit(T*, U const&, V const&){ //std::cout <<"DoNothing"<<std::endl;
   }
};


template<typename T>
class Worker : public WorkerBase {
private:

   constexpr static bool usesTransition(unsigned int iTrans) { return (T::kAbilities & iTrans) != 0; }
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kGlobalBeginRun), 
                                    DoCallBeginRun<T>,
                                    DoNothing<T,RunIndex const&> >::type DoBeginRunCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kGlobalBeginLuminosityBlock), 
                                     DoCallBeginLumi<T>,
                                     DoNothing<T,LumiIndex const&> >::type DoBeginLumiCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kBeginStream), 
                                     DoCallBeginStream<T>,
                                     DoNothing<T,StreamID const&> >::type DoBeginStreamCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kEndStream), 
                                     DoCallEndStream<T>,
                                     DoNothing<T,StreamID const&> >::type DoEndStreamCall;

   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kStreamBeginRun), 
                                     DoCallStreamBeginRun<T>,
                                     DoNothing2<T,StreamID const&, RunIndex const&> >::type DoStreamBeginRunCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kStreamEndRun), 
                                     DoCallStreamEndRun<T>,
                                     DoNothing2<T,StreamID const&, RunIndex const&> >::type DoStreamEndRunCall;

   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kStreamBeginLuminosityBlock), 
                                     DoCallStreamBeginLumi<T>,
                                     DoNothing2<T,StreamID const&, LumiIndex const&> >::type DoStreamBeginLumiCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kStreamEndLuminosityBlock), 
                                     DoCallStreamEndLumi<T>,
                                     DoNothing2<T,StreamID const&, LumiIndex const&> >::type DoStreamEndLumiCall;
   typedef typename boost::mpl::if_c<usesTransition(AbilityToTransitions::kGlobalEndRun), 
                                  DoCallEndRun<T>,
                                  DoNothing<T,RunIndex const&> >::type DoEndRunCall;


public:
   Worker(T* iModule): m_module(iModule){}
    bool needsBeginRunTransition() const override { return usesTransition(AbilityToTransitions::kGlobalBeginRun); }
    bool needsBeginLumiTransition() const override { return usesTransition(AbilityToTransitions::kGlobalBeginLuminosityBlock); }
    bool needsBeginStreamTransition() const override { return usesTransition(AbilityToTransitions::kBeginStream); }
    bool needsEndStreamTransition() const override { return usesTransition(AbilityToTransitions::kEndStream); }
   
   void beginJob()  {m_module->beginJob();}
   void endJob()  {m_module->endJob();}
   

   void beginRun(RunIndex const& iIndex) override {  DoBeginRunCall::doit(m_module.get(),iIndex); } 
   void beginLumi(LumiIndex const& iIndex) override {  DoBeginLumiCall::doit(m_module.get(),iIndex); } 
   void beginStream(StreamID const& iIndex) override {DoBeginStreamCall::doit(m_module.get(),iIndex); }
   void endStream(StreamID const& iIndex) override {DoEndStreamCall::doit(m_module.get(),iIndex); }
   void endLumi(LumiIndex const& iIndex) override {  /*DoEndLumiCall::doit(m_module.get(),iIndex);*/ } 
   void endRun(RunIndex const& iIndex) override {  DoEndRunCall::doit(m_module.get(),iIndex); } 
   
   void streamBeginRun(StreamID const& iID, RunIndex const& iIndex) override {DoStreamBeginRunCall::doit(m_module.get(),iID,iIndex);}
   void streamBeginLumi(StreamID const& iID, LumiIndex const& iIndex) override {DoStreamBeginLumiCall::doit(m_module.get(),iID,iIndex);}
   void streamEndRun(StreamID const& iID, RunIndex const& iIndex) override {DoStreamEndRunCall::doit(m_module.get(),iID,iIndex);}
   void streamEndLumi(StreamID const& iID, LumiIndex const& iIndex) override {DoStreamEndLumiCall::doit(m_module.get(),iID,iIndex);}
   
   void setNumberOfStreams(unsigned int iNum) override {m_module->setNumberOfStreams(iNum);}
private:
   std::shared_ptr<T> m_module;
};



#endif /* end of include guard: WORKER_H_35Z4BV3E */
