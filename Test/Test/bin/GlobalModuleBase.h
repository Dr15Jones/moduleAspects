#ifndef GLOBALMODULEBASE_H_356915FI
#define GLOBALMODULEBASE_H_356915FI

#include <vector>
#include <memory>
#include "IDs.h"
#include "ModuleAbilityEnums.h"

class GlobalModuleBase {
public:
   //These constants are used by Worker<T> to decide
   // if it is necessary to call the corresponding function
   static constexpr AbilitiesType kAbilities = 0;
      
   virtual void setNumberOfStreams( unsigned int ) {}
   
   virtual void beginJob() const {}
   virtual void endJob() const {}
protected:
   void doGlobalBeginRun(RunIndex const&) const {}
   void doStreamEndRun(StreamID const& iID, RunIndex const& iIndex) const {}
   void doGlobalEndRun(RunIndex const& iIndex) const {}
   
};


namespace global {
   template<typename T, typename B>
   class StreamCacheHolder : public B {
   public:
      static constexpr AbilitiesType kAbilities = B::kAbilities | AbilityBits::kStreamCache;
      
      typedef StreamCacheHolder<T,B> StreamCacheBaseType;
      
      ~StreamCacheHolder(){
         for(T* x: m_cache){
            delete x;
         }
      }
      
      void setNumberOfStreams(unsigned int iNum) override {
         m_cache.resize(iNum);
         B::setNumberOfStreams(iNum);
      }
      
      void doBeginStream(StreamID const& iID) {
         m_cache[iID.id()] = beginStream(iID).release();
      }
      
      void doStreamBeginRun(StreamID const& iID, RunIndex const& iIndex) const { streamBeginRun(iID,iIndex);}
      void doStreamBeginLumi(StreamID const& iID, LumiIndex const& iIndex) const { streamBeginLumi(iID,iIndex);}

      void doStreamEndRun(StreamID const& iID, RunIndex const& iIndex) const { streamEndRun(iID,iIndex);}
      void doStreamEndLumi(StreamID const& iID, LumiIndex const& iIndex) const { streamEndLumi(iID,iIndex);}

      void doEndStream(StreamID const& iID) { endStream(iID);}

      virtual void streamBeginRun(StreamID const&, RunIndex const&) const {}
      virtual void streamBeginLumi(StreamID const&, LumiIndex const&) const {}

      virtual void streamEndRun(StreamID const&, RunIndex const&) const {}
      virtual void streamEndLumi(StreamID const&, LumiIndex const&) const {}
      
      void virtual endStream(StreamID const&) const {};
      
   protected:
      //since  can only get access to a stream cache by being in a 
      // stream it means the cache can be modified 
      T * streamCache(StreamID const& iID) const {
         return m_cache[iID.id()];
      }
   private:
      virtual std::unique_ptr<T> beginStream(StreamID const&) const = 0;
      std::vector<T*> m_cache; 
   };


   template<typename T, typename B>
   class RunCacheHolder : public B {
   public:
      template<typename... VArgs>
      RunCacheHolder(VArgs... iArgs):B(iArgs...){}
      
      //RunCacheHolder() {}
      //RunCacheHolder(PSet const& iPSet):B(iPSet){}
      static constexpr AbilitiesType kAbilities = B::kAbilities | AbilityBits::kRunCache;
      void doGlobalBeginRun(RunIndex const& iIndex) {
         // We first need to release it to let the
         // system know we are no longer using it 
         // so it could be reused
         m_caches[iIndex.index()].reset();
         m_caches[iIndex.index()]=globalBeginRun(iIndex);
      }
      
      void doGlobalEndRun(RunIndex const& iIndex) const {
         globalEndRun(iIndex);
      }
      
      void setNumberOfStreams(unsigned int iNum) override {
         m_caches.resize(iNum);
         B::setNumberOfStreams(iNum);
      }
      
   protected:
      T const* runCache(RunIndex const& iID) const { return m_caches[iID.index()].get();}
   private:
      virtual std::shared_ptr<T> globalBeginRun(RunIndex const&) const = 0;
      virtual void globalEndRun(RunIndex const&) const {}
      std::vector<std::shared_ptr<T>> m_caches;
   };
   
   template<typename T, typename B>
   class LumiCacheHolder : public B {
   public:
      template<typename... VArgs>
      LumiCacheHolder(VArgs... iArgs):B(iArgs...){}
      //LumiCacheHolder() {}
      //LumiCacheHolder(PSet const& iPSet):B(iPSet){}

      static constexpr AbilitiesType kAbilities = B::kAbilities | AbilityBits::kLuminosityBlockCache;
      void doGlobalBeginLumi(LumiIndex const& iIndex) {
         // We first need to release it to let the
         // system know we are no longer using it 
         // so it could be reused
         m_caches[iIndex.index()].reset();
         m_caches[iIndex.index()]=globalBeginLumi(iIndex);
      }
      
      void setNumberOfStreams(unsigned int iNum) override {
         m_caches.resize(iNum);
         B::setNumberOfStreams(iNum);
      }
      
   protected:
      T const* lumiCache(LumiIndex const& iID) const { return m_caches[iID.index()].get();}
   private:
      virtual std::shared_ptr<T> globalBeginLumi(LumiIndex const&) const = 0;
      std::vector<std::shared_ptr<T>> m_caches;
   };
   
   
   template<typename T> class EndRunProducerEnabler;
   //NOTE: If have RunCache and/or StreamCache then RunSummaryCacheHolder must inherit from
   //  RunCacheHolder and/or StreamCacheHolder in order to guarantee correct calling order
   template<typename T, typename B>
   class RunSummaryCacheHolder : public B {
   public:
      template<typename... VArgs>
      RunSummaryCacheHolder(VArgs... iArgs):B(iArgs...){}
      
      static constexpr AbilitiesType kAbilities = B::kAbilities | AbilityBits::kRunSummaryCache;
      void doGlobalBeginRun(RunIndex const& iIndex) {
         // We first need to release it to let the
         // system know we are no longer using it 
         // so it could be reused
         m_caches[iIndex.index()].reset();
         B::doGlobalBeginRun(iIndex);
         m_caches[iIndex.index()]=globalBeginRunSummary(iIndex);
      }
      
      void doStreamEndRun(StreamID const& iID, RunIndex const& iIndex) const {
         //NOTE: instead of calling directly, maybe should use boost::mpl::if_c?
         B::doStreamEndRun(iID,iIndex);
         
         //would put this on a task within a task queue to guarantee synchronization
         //NOTE: so once 'stream end run' finishes we still need to be sure all tasks
         // associated with that have finished before doing the 'global end run' transtion
         {
            streamEndRunSummary(iID,iIndex,m_caches[iIndex.index()].get());
         }
      }
      
      void doGlobalEndRun(RunIndex const& iIndex) const {
         globalEndRunSummary(iIndex,m_caches[iIndex.index()].get());
         
         //we guarantee that globalEndRunSummary is called first
         B::doGlobalEndRun(iIndex);
      }
      
      void setNumberOfStreams(unsigned int iNum) override {
         m_caches.resize(iNum);
         B::setNumberOfStreams(iNum);
      }
      
   private:
      template<typename U> friend class EndRunProducerEnabler; //needs access to summary
      const T* summary(RunIndex const& iIndex) const {return m_caches[iIndex.index()].get();}
      
      virtual std::shared_ptr<T> globalBeginRunSummary(RunIndex const&) const = 0;
      virtual void streamEndRunSummary(StreamID const& iID, RunIndex const&,T*) const = 0;
      virtual void globalEndRunSummary(RunIndex const&, T*) const = 0;
      std::vector<std::shared_ptr<T>> m_caches;
   };
   
   template<typename B>
   class EndRunProducerEnabler : public B {
   public:
      static constexpr AbilitiesType kAbilities = B::kAbilities | AbilityBits::kEndRunProducer;
      
      template<typename... VArgs>
      EndRunProducerEnabler(VArgs... iArgs):B(iArgs...){}
      
      void doGlobalEndRun(RunIndex const& iIndex) const {
         B::doGlobalEndRun(iIndex);
         globalEndRunProduce(iIndex);
      }
      
   private:
      virtual void globalEndRunProduce(RunIndex const&) const = 0;
   };


   template<typename T, typename B>
   class EndRunProducerEnabler<RunSummaryCacheHolder<T,B>> : public RunSummaryCacheHolder<T,B> {
   public:
      static constexpr AbilitiesType kAbilities = RunSummaryCacheHolder<T,B>::kAbilities | AbilityBits::kEndRunProducer;
      
      template<typename... VArgs>
      EndRunProducerEnabler(VArgs... iArgs):RunSummaryCacheHolder<T,B>(iArgs...){}
      
      void doGlobalEndRun(RunIndex const& iIndex) const {
         RunSummaryCacheHolder<T,B>::doGlobalEndRun(iIndex);
         //Safe to call summary here since all work has finished on the summary object
         globalEndRunProduce(iIndex,RunSummaryCacheHolder<T,B>::summary(iIndex));
      }
      
   private:
      virtual void globalEndRunProduce(RunIndex const&, T const*) const = 0;
   };

}

#endif /* end of include guard: GLOBALMODULEBASE_H_356915FI */
