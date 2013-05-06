#ifndef STREAMTOGLOBALADAPTOR_H_V0YAT8HQ
#define STREAMTOGLOBALADAPTOR_H_V0YAT8HQ

#include <memory>
#include "boost/mpl/if.hpp"

#include "GlobalModuleBase.h"
#include "StreamModuleBase.h"

namespace s2gadaptor {
   template<class T>
   class StreamAdapterModule : public global::StreamCacheHolder<T,GlobalModuleBase> {
   public:
      //If the base class implementation is not overridden, we have no reason to call the function
      // The constants are used by Worker<T> to decide if the functions should be called
      //static const bool kHasStreamBeginRun=(&T::beginRun != &StreamModuleBase::beginRun);
      //static const bool kHasStreamEndRun=(&T::endRun != &StreamModuleBase::endRun);
      //static const bool kHasStreamBeginLumi=(&T::beginLumi != &StreamModuleBase::beginLumi);
      //static const bool kHasStreamEndLumi=(&T::endLumi != &StreamModuleBase::endLumi);
      
      StreamAdapterModule(PSet const& iPSet): m_pset(iPSet) {}

      void streamBeginRun(StreamID const& iStreamID, RunIndex const& iRunID) const override {
         T* module = this->streamCache(iStreamID);
         module->beginRun(iRunID);
      }
      void streamBeginLumi(StreamID const& iStreamID, LumiIndex const& iRunID) const override {
         T* module = this->streamCache(iStreamID);
         module->beginLumi(iRunID);         
      }

      void streamEndRun(StreamID const& iStreamID, RunIndex const& iLumiID) const override {
         T* module = this->streamCache(iStreamID);
         module->endRun(iLumiID);         
      }
      void streamEndLumi(StreamID const& iStreamID, LumiIndex const& iLumiID) const override {
         T* module = this->streamCache(iStreamID);
         module->endLumi(iLumiID);
      }
      
      void endStream(StreamID const& iStreamID) const override {
         T* module = this->streamCache(iStreamID);
         module->beginStream();
      }
      
   private:
      virtual std::unique_ptr<T> beginStream(StreamID const&) const override {
         return build(m_pset);
      }
      
   
      virtual std::unique_ptr<T> build(PSet const& ) const = 0;
      PSet m_pset;
   };


   template< class T>
   constexpr bool hasAbility(unsigned int iAbilityBits) {return (T::kAbilities & iAbilityBits) !=0;}
   
   template <class T>
   class AdaptGlobalCache : public StreamAdapterModule<T> {
   public:
      AdaptGlobalCache(PSet const& iPSet): StreamAdapterModule<T>(iPSet),
      m_globalCache(T::initializeGlobalCache(iPSet)) {}
   
      void endJob() const override { T::globalEndJob(m_globalCache.get());}
   protected:
      typename T::GlobalCacheType const* globalCache() const {return m_globalCache.get();}
   private:
      virtual std::unique_ptr<T> build(PSet const& iPSet) const override {
         return std::unique_ptr<T>{ new T(iPSet,m_globalCache)};
      }
      std::shared_ptr<typename T::GlobalCacheType> m_globalCache;
   };

   template <class T>
   class AdaptNoGlobalCache : public StreamAdapterModule<T> {
   public:
      AdaptNoGlobalCache(PSet const& iPSet): StreamAdapterModule<T>(iPSet)
      {}
   
   protected:
      stream::EmptyCache const* globalCache() const { return nullptr;}
   private:
      virtual std::unique_ptr<T> build(PSet const& iPSet) const override {
         return std::unique_ptr<T>{ new T(iPSet)};
      }
   };

   template <class T>
   using AdaptCheckGlobalCache =  typename boost::mpl::if_c<hasAbility<T>(AbilityBits::kGlobalCache),
                                                            AdaptGlobalCache<T>,
                                                            AdaptNoGlobalCache<T>>::type;


   template <class T>
   class AdaptNoRunCache : public AdaptCheckGlobalCache<T> {
   public:
      AdaptNoRunCache(PSet const& iPSet): AdaptCheckGlobalCache<T>(iPSet)
      {}
   
   protected:
      stream::EmptyCache const* runCache() const { return nullptr;}
   };


   template <class T>
   class AdaptRunCache : public global::RunCacheHolder<typename T::RunCacheType, AdaptCheckGlobalCache<T> > {
   public:
      
      typedef global::RunCacheHolder<typename T::RunCacheType, AdaptCheckGlobalCache<T> > BaseClass;
      AdaptRunCache(PSet const& iPSet): BaseClass(iPSet){}

      void streamBeginRun(StreamID const& iStreamID, RunIndex const& iRun) const override {
         T* module = this->streamCache(iStreamID);
         module->setCache(this->runCache(iRun));
         if(&T::beginRun != &StreamModuleBase::beginRun) {
            //if the base class function never overridden, we have no reason to call it
            this->BaseClass::streamBeginRun(iStreamID,iRun);
         }
      }

   private:
      std::shared_ptr<typename T::RunCacheType> globalBeginRun(RunIndex const& iIndex) const override {
         return T::globalBeginRun(iIndex,this->globalCache());
      }         
   };

   template <class T>
   using AdaptCheckRunCache = typename boost::mpl::if_c<hasAbility<T>(AbilityBits::kRunCache),
                              AdaptRunCache<T>,
                              AdaptNoRunCache<T> >::type;
   
   
   template <class T>
   using AdaptNoRunSummaryCache = AdaptCheckRunCache<T>;
   

   template <typename T, typename B>
   class AdaptRunSummaryCacheImpl : public B {
   public:
      
      typedef B BaseClass;
      AdaptRunSummaryCacheImpl(PSet const& iPSet): BaseClass(iPSet){}


   private:
      virtual std::shared_ptr<typename T::RunSummaryCacheType> globalBeginRunSummary(RunIndex const&iIndex) const override {
         typename T::RunContext rc(this->globalCache(),this->runCache());
         return T::globalBeginRunSummary(iIndex,&rc);
      }
      void streamEndRunSummary(StreamID const& iStreamID, RunIndex const& iRun, typename T::RunSummaryCacheType* iSummary) const override {
         T* module = this->streamCache(iStreamID);
         module->endRunSummary(iRun,iSummary);
      }
      virtual void globalEndRunSummary(RunIndex const& iIndex, typename T::RunSummaryCacheType* iSummary) const override {
         typename T::RunContext rc(this->globalCache(),this->runCache());
         T::globalEndRunSummary(iIndex,&rc,iSummary);
      }
   };
   
   template <class T>
   class AdaptRunSummaryCache : public AdaptRunSummaryCacheImpl<T,global::RunSummaryCacheHolder<typename T::RunSummaryCacheType, AdaptNoRunSummaryCache<T> >> {
   public:
      typedef AdaptRunSummaryCacheImpl<T,global::RunSummaryCacheHolder<typename T::RunSummaryCacheType, AdaptNoRunSummaryCache<T> >> BaseClass;
      AdaptRunSummaryCache(PSet const& iPSet): BaseClass(iPSet){}
      
   };
   

   template <class T>
   using  AdaptCheckRunSummaryCache = typename boost::mpl::if_c<hasAbility<T>(AbilityBits::kRunSummaryCache),
                              AdaptRunSummaryCache<T>,
                              AdaptNoRunSummaryCache<T> >::type;


   template <typename T> using AdaptNoEndRunProducer = AdaptCheckRunSummaryCache<T>;
   
   
   template <class T>
   class AdaptEndRunProducer: public global::EndRunProducerEnabler<AdaptNoEndRunProducer<T>> {
   public:
      
      typedef global::EndRunProducerEnabler<AdaptNoEndRunProducer<T>> BaseClass;
      AdaptEndRunProducer(PSet const& iPSet): BaseClass(iPSet){}


   private:
      virtual void globalEndRunProduce(RunIndex const& iIndex) const override {
         typename T::RunContext rc(this->globalCache(),this->runCache());
         T::globalEndRunProduce(iIndex,&rc);
      }
   };
   
   template <class T>
   using  AdaptCheckEndRunProducer = typename boost::mpl::if_c<hasAbility<T>(AbilityBits::kEndRunProducer),
                              AdaptEndRunProducer<T>,
                              AdaptNoEndRunProducer<T> >::type;


   template <class T> using AdaptNoEndRunWithSummaryProducer=AdaptCheckEndRunProducer<T>;

   template <class T>
   class AdaptEndRunWithSummaryProducer: public AdaptRunSummaryCacheImpl<T,global::EndRunProducerEnabler<global::RunSummaryCacheHolder<typename T::RunSummaryCacheType, AdaptNoRunSummaryCache<T> >>> {
   public:
      
      typedef AdaptRunSummaryCacheImpl<T,global::EndRunProducerEnabler<global::RunSummaryCacheHolder<typename T::RunSummaryCacheType, AdaptNoRunSummaryCache<T> >>> BaseClass;
      AdaptEndRunWithSummaryProducer(PSet const& iPSet): BaseClass(iPSet){}


   private:
      virtual void globalEndRunProduce(RunIndex const& iIndex, typename T::RunSummaryCacheType const* iSummary) const override {
         typename T::RunContext rc(this->globalCache(),this->runCache());
         T::globalEndRunProduce(iIndex,&rc,iSummary);
      }
   };

   template <class T>
   using  AdaptCheckEndRunWithSummaryProducer = typename boost::mpl::if_c<(hasAbility<T>(AbilityBits::kEndRunProducer) && hasAbility<T>(AbilityBits::kRunSummaryCache)),
                              AdaptEndRunWithSummaryProducer<T>,
                              AdaptNoEndRunWithSummaryProducer<T> >::type;


  template <class T> using AdaptNoLumiCache = AdaptCheckEndRunWithSummaryProducer<T>;

   template <class T>
   class AdaptLumiCache : public global::LumiCacheHolder<typename T::LumiCacheType, AdaptNoLumiCache<T> > {
   public:
      
      typedef global::LumiCacheHolder<typename T::LumiCacheType, AdaptCheckRunSummaryCache<T> > BaseClass;
      AdaptLumiCache(PSet const& iPSet): BaseClass(iPSet){}

      void streamBeginLumi(StreamID const& iStreamID, LumiIndex const& iLumi) const override {
         T* module = this->streamCache(iStreamID);
         module->setCache(this->lumiCache(iLumi));
         if(&T::beginLumi != &StreamModuleBase::beginLumi) {
            //if the base class function never overridden, we have no reason to call it
            this->BaseClass::streamBeginLumi(iStreamID,iLumi);
         }
      }

   private:
      std::shared_ptr<typename T::LumiCacheType> globalBeginLumi(LumiIndex const& iIndex) const override {
         typename T::RunContext rc(this->globalCache(),this->runCache());
         return T::globalBeginLumi(iIndex,&rc);
      }
   };

   
   template <class T>
   using AdaptCheckLumiCache = typename boost::mpl::if_c<hasAbility<T>(AbilityBits::kLuminosityBlockCache),
                                                         AdaptLumiCache<T>,
                                                         AdaptNoLumiCache<T> >::type;
   
}

template <class T>
class StreamToGlobalAdaptorModule : public s2gadaptor::AdaptCheckLumiCache<T> {
public:
   StreamToGlobalAdaptorModule(PSet const& iPSet): s2gadaptor::AdaptCheckLumiCache<T>(iPSet){}
};

#endif /* end of include guard: STREAMTOGLOBALADAPTOR_H_V0YAT8HQ */
