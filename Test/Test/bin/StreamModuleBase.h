#ifndef STREAMMODULEBASE_H_OUQ48EGP
#define STREAMMODULEBASE_H_OUQ48EGP
#include "boost/mpl/if.hpp"

namespace stream {
   struct EmptyCache {};
}

class StreamModuleBase {
public:
   static const AbilitiesType kAbilities = 0;
   
   //static const bool kHasRunCache=false;
   //static const bool kHasGlobalCache=false;
   //static const bool kHasLumiCache=false;
   //static const bool kHasRunSummaryCache=false;
   typedef stream::EmptyCache GlobalCacheType;
   typedef stream::EmptyCache RunCacheType;
   StreamModuleBase(StreamModuleBase const&) = delete;
   StreamModuleBase& operator=(StreamModuleBase const&) = delete;
   StreamModuleBase() {}
   virtual ~StreamModuleBase() {}

   virtual void beginStream() const {}
   virtual void endStream() const {}

   virtual void beginRun(RunIndex const&) const {}
   virtual void beginLumi(LumiIndex const&) const {}

   virtual void endRun(RunIndex const&) const {}
   virtual void endLumi(LumiIndex const&) const {}

};

namespace stream {
   
   template< typename T, typename I>
   class GlobalCacheHolder : public I {
   public:
      static constexpr AbilitiesType kAbilities = I::kAbilities | AbilityBits::kGlobalCache;
      typedef T GlobalCacheType;

   //static std::unique_ptr<T> initializeGlobalCache(PSet const&);
   //static globalEndJob(T const*)
   protected:
      GlobalCacheHolder(std::shared_ptr<T> iGlobal): m_globalCache(iGlobal){}
   
      T const* globalCache() const { return m_globalCache.get();}
   private:
      std::shared_ptr<T> m_globalCache;
   };

   template< typename T, typename I>
   class RunCacheHolder : public I {
   public:
      typedef T RunCacheType;
      static constexpr AbilitiesType kAbilities = I::kAbilities | AbilityBits::kRunCache;
      
      void setCache(T const* iCache) {
         cache_ = iCache;
      }
      
      //Determine the proper GlobalContext type to pass
      typedef typename I::GlobalCacheType GlobalContext;
     
      //static std::shared_ptr<T> globalBeginRun(RunIndex const&, GlobalContext const*);
   protected:
      template<typename... VArgs>
      RunCacheHolder(VArgs... iArgs):I(iArgs...),cache_{nullptr}{}
   private:
      T const *  cache_;
   };

   template<typename G, typename R>
   struct GlobalAndRunContext {
     GlobalAndRunContext(G const* iGlobal, R const* iRun): global_{iGlobal},run_{iRun} {}
     
     G const* globalContext() const {return global_;}
     R const* runContext() const { return run_;}
   private:
     G const* global_;
     R const* run_;
   };


   template< typename T, typename I>
   class RunSummaryCacheHolder : public I {
   public:
      typedef T RunSummaryCacheType;
      static constexpr AbilitiesType kAbilities = I::kAbilities | AbilityBits::kRunSummaryCache;
      
      //Determine the proper RunContext to pass
      typedef GlobalAndRunContext<typename I::GlobalCacheType,
                                  typename I::RunCacheType> RunContext;                                   
      
      //static std::shared_ptr<T> globalBeginRunSummary(RunIndex const&, RunContext const*);
      virtual void endRunSummary(RunIndex const&, T* ) const = 0;

      //static void globalEndRunSummary(RunIndex const&, RunContext const*, T*);
   };

   template< typename T, typename I>
   class LumiCacheHolder : public I {
   public:
      typedef T LumiCacheType;
      static constexpr AbilitiesType kAbilities = I::kAbilities | AbilityBits::kLuminosityBlockCache;
      
      //Determine the proper RunContext to pass
      typedef GlobalAndRunContext<typename I::GlobalCacheType,
                                  typename I::RunCacheType> RunContext;                                   
      //static std::shared_ptr<T> globalBeginRun(LumiIndex const&, RunContext const*);
   
      void setCache(T const*  iCache) {
         cache_ = iCache;
      }
   private:
      T const* cache_;
   };

   template< typename I>
   class EndRunProducerEnabler : public I {
   public:
      static constexpr AbilitiesType kAbilities = I::kAbilities | AbilityBits::kEndRunProducer;
      //Determine the proper RunContext to pass
      typedef GlobalAndRunContext<typename I::GlobalCacheType,
                                  typename I::RunCacheType> RunContext;                                   
      
      //static void globalEndRunProduce(RunIndex const&, RunContext const*); or
      //static void globalEndRunProduce(RunIndex const&, RunContext const*, I::RunSummaryCacheType* );
   };
}

#endif /* end of include guard: STREAMMODULEBASE_H_OUQ48EGP */
