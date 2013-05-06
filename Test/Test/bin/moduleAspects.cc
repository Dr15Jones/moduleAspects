#include <memory>
#include <iostream>
#include <vector>
#include <map>
#include <cassert>

#include "IDs.h"

#include "GlobalModuleBase.h"
#include "GlobalModule.h"
#include "StreamModuleBase.h"
#include "StreamModule.h"
#include "StreamToGlobalAdaptor.h"
#include "Worker.h"
#include "Maker.h"

struct MyRunData {
   int data_;
};

//Used to count how many calls to module member functions have been made
static unsigned int s_callCount=0;


class MyRunGlobalModule : public global::RunCacheHolder<MyRunData,GlobalModuleBase> {
public:
   MyRunGlobalModule(PSet const&){}
   std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyRunGlobalModule::beginRun called"<<std::endl;
      return 0;}
};

class MyRunGlobalModule2 : public GlobalModule<edm::RunCache<MyRunData>> {
public:
   MyRunGlobalModule2(PSet const&){}
   std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyRunGlobalModule2::beginRun called"<<std::endl;
      return 0;}   
};

class MyNonRunGlobalModule: public GlobalModuleBase {
public:
   MyNonRunGlobalModule(PSet const&){}
};

class MySimpleGlobalModule: public GlobalModule<> {
public:
   MySimpleGlobalModule(PSet const&){}
};


class MyRunStreamModule : public stream::RunCacheHolder<MyRunData,StreamModuleBase> {
public:
   MyRunStreamModule(PSet const&){}
   static std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&, GlobalContext const* iContext) { 
      ++s_callCount;
      std::cout <<"  MyRunStreamModule::globalBeginRun called"<<std::endl;
      assert(nullptr==iContext);
      return 0;}
};

class MyRunStreamModule2 : public StreamModule<edm::RunCache<MyRunData>> {
public:
   MyRunStreamModule2(PSet const&){}
   static std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&, GlobalContext const* iContext) { 
      ++s_callCount;
      std::cout <<"  MyRunStreamModule2::globalBeginRun called"<<std::endl;
      assert(nullptr==iContext);
      return 0;}
};


class MyNonRunStreamModule: public StreamModuleBase {
public:
   MyNonRunStreamModule(PSet const&){}
};

class MyNonRunStreamModule2: public StreamModule<> {
public:
   MyNonRunStreamModule2(PSet const&){}
};


class MyAllTransStreamModule : public StreamModuleBase {
public:
   MyAllTransStreamModule(PSet const&){}

   void beginRun(RunIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyAllTransStreamModule::beginRun called"<<std::endl;
   }
   void beginLumi(LumiIndex const&) const override {
      ++s_callCount;
      std::cout <<"  MyAllTransStreamModule::beginLumi called"<<std::endl;      
   }

   void endRun(RunIndex const&) const override {
      ++s_callCount;
      std::cout <<"  MyAllTransStreamModule::endRun called"<<std::endl;      
   }
   void endLumi(LumiIndex const&) const override {
      ++s_callCount;
      std::cout <<"  MyAllTransStreamModule::endLumi called"<<std::endl;      
   }
};


//------------------------------------------------
struct MyLumiData {
   int data_;
};

class MyLumiGlobalModule : public global::LumiCacheHolder<MyLumiData,GlobalModuleBase> {
public:
   MyLumiGlobalModule(PSet const&){}
   std::shared_ptr<MyLumiData> globalBeginLumi(LumiIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyLumiGlobalModule::beginLumi called"<<std::endl;
      return 0;}
};


class MyLumiAndRunGlobalModule : public global::LumiCacheHolder<MyLumiData,global::RunCacheHolder<MyRunData,GlobalModuleBase>> {
public:
   MyLumiAndRunGlobalModule(PSet const&){}
   std::shared_ptr<MyLumiData> globalBeginLumi(LumiIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyLumiAndRunGlobalModule::beginLumi called"<<std::endl;
      return 0;
   }
      
   std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&) const override { 
      ++s_callCount;
      std::cout <<"  MyLumiAndRunGlobalModule::beginRun called"<<std::endl;
      return 0;
   }
};


class MyLumiStreamModule : public stream::LumiCacheHolder<MyLumiData,StreamModuleBase> {
public:
   MyLumiStreamModule(PSet const&){}
   static std::shared_ptr<MyLumiData> globalBeginLumi(LumiIndex const&, RunContext const*) { 
      ++s_callCount;
      std::cout <<"  MyLumiStreamModule::beginLumi called"<<std::endl;
      return 0;}
};


//------------------------------------------------
struct MyStreamCache {
   int data_;
};

class MyStreamCacheGlobalModule : public global::StreamCacheHolder<MyStreamCache,GlobalModuleBase> {
public:
   MyStreamCacheGlobalModule(PSet const& ){}
private:
   std::unique_ptr<MyStreamCache> beginStream(StreamID const&) const override {
      ++s_callCount;
      std::cout <<"  MyStreamCacheGlobalModule::beginStream called"<<std::endl;
      return std::unique_ptr<MyStreamCache>{ new MyStreamCache };
   }
   
};

class MyStreamCacheAllTransGlobalModule : public global::StreamCacheHolder<MyStreamCache,GlobalModuleBase> {
public:
   MyStreamCacheAllTransGlobalModule(PSet const& ){}
private:
   std::unique_ptr<MyStreamCache> beginStream(StreamID const&) const override {
      ++s_callCount;
      std::cout <<"  MyStreamCacheAllTransGlobalModule::beginStream called"<<std::endl;
      return std::unique_ptr<MyStreamCache>{ new MyStreamCache };
   }
public:
   void streamBeginRun(StreamID const& iID, RunIndex const&) const override {
      ++s_callCount;
      assert(0!=streamCache(iID));
      std::cout <<"  MyStreamCacheAllTransGlobalModule::streamBeginRun called"<<std::endl;
   }
   void streamBeginLumi(StreamID const& iID, LumiIndex const&) const override {
      ++s_callCount;
      assert(0!=streamCache(iID));
      std::cout <<"  MyStreamCacheAllTransGlobalModule::streamBeginLumi called"<<std::endl;      
   }

   void streamEndRun(StreamID const& iID, RunIndex const&) const override {
      ++s_callCount;
      assert(0!=streamCache(iID));
      std::cout <<"  MyStreamCacheAllTransGlobalModule::streamEndRun called"<<std::endl;
   }
   void streamEndLumi(StreamID const& iID, LumiIndex const&) const override {
      ++s_callCount;
      assert(0!=streamCache(iID));
      std::cout <<"  MyStreamCacheAllTransGlobalModule::streamEndLumi called"<<std::endl;
   }
   
   
};


//------------------------------------------------
struct MyGlobalCache {
   int data_;
};

class MyGlobalCacheStreamModule : public stream::GlobalCacheHolder<MyGlobalCache,StreamModuleBase> {
public:
   MyGlobalCacheStreamModule(PSet const&, std::shared_ptr<MyGlobalCache> iCache):
   stream::GlobalCacheHolder<MyGlobalCache,StreamModuleBase>(iCache) {}
   
   static std::shared_ptr<MyGlobalCache> initializeGlobalCache(PSet const&) {
      ++s_callCount;
      std::cout <<"  MyGlobalCacheStreamModule::initializeGlobalCache called"<<std::endl;
      return std::shared_ptr<MyGlobalCache>{new MyGlobalCache{}};
   }
   
   static void globalEndJob(MyGlobalCache const*) {
      ++s_callCount;
      std::cout <<"  MyGlobalCacheStreamModule::globalEndJob called"<<std::endl;
   }
   
};

class MyGlobalAndRunCacheStreamModule : public stream::RunCacheHolder<MyRunData,stream::GlobalCacheHolder<MyGlobalCache,StreamModuleBase>> {
public:
   MyGlobalAndRunCacheStreamModule(PSet const&, std::shared_ptr<MyGlobalCache> iCache):
   stream::RunCacheHolder<MyRunData,stream::GlobalCacheHolder<MyGlobalCache,StreamModuleBase>>(iCache)
   {}
   static std::shared_ptr<MyGlobalCache> initializeGlobalCache(PSet const&) {
      ++s_callCount;
      std::cout <<"  MyGlobalCacheStreamModule::initializeGlobalCache called"<<std::endl;
      return std::shared_ptr<MyGlobalCache>{new MyGlobalCache{}};
   }
   
   static std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&, GlobalContext const* iContext) { 
      ++s_callCount;
      std::cout <<"  MyRunStreamModule::globalBeginRun called"<<std::endl;
      assert(0!=iContext);
      return 0;}
};


//------------------------------------------------
struct MyRunSummaryCache {
   int data_;
};

class MyRunSummaryGlobalModule : public global::RunSummaryCacheHolder<MyRunSummaryCache,GlobalModuleBase> {
public:
   MyRunSummaryGlobalModule(PSet const&){} 
private:
   std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   void globalEndRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule::globalEndRunSummary called"<<std::endl;
   }
   
   void streamEndRunSummary(StreamID const&, RunIndex const&, MyRunSummaryCache*) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule::streamEndRunSummary called"<<std::endl;
   }
};

class MyRunSummaryGlobalModule2 : public GlobalModule<edm::RunSummaryCache<MyRunSummaryCache>> {
public:
   MyRunSummaryGlobalModule2(PSet const&){} 
private:
   std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule2::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   void globalEndRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule2::globalEndRunSummary called"<<std::endl;
   }
   
   void streamEndRunSummary(StreamID const&, RunIndex const&, MyRunSummaryCache*) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryGlobalModule2::streamEndRunSummary called"<<std::endl;
   }
};


class MyRunAndRunSummaryGlobalModule : public GlobalModule<edm::RunCache<MyRunData>,edm::RunSummaryCache<MyRunSummaryCache>> {
public:
   MyRunAndRunSummaryGlobalModule(PSet const&){}
private:
   mutable unsigned int m_orderCounter=0;
   std::shared_ptr<MyRunData> globalBeginRun(RunIndex const&) const override { 
      assert(m_orderCounter==0);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyRunAndRunSummaryGlobalModule::beginRun called"<<std::endl;
      return 0;
   }
   
   void globalEndRun(RunIndex const&) const override {
      ++s_callCount;
      assert(m_orderCounter==4);
      ++m_orderCounter;
      std::cout <<"  MyRunAndRunSummaryGlobalModule::globalEndRun called"<<std::endl;
   }
   
   std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&) const override {
      assert(m_orderCounter==1);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyRunAndRunSummaryGlobalModule::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   void globalEndRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      assert(m_orderCounter==3);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyRunAndRunSummaryGlobalModule::globalEndRunSummary called"<<std::endl;
   }
   
   void streamEndRunSummary(StreamID const&, RunIndex const&, MyRunSummaryCache*) const override {
      assert(m_orderCounter==2);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyRunAndRunSummaryGlobalModule::streamEndRunSummary called"<<std::endl;
   }

};


class MyRunSummaryStreamModule : public stream::RunSummaryCacheHolder<MyRunSummaryCache,StreamModuleBase> {
public:
   MyRunSummaryStreamModule(PSet const&){} 

   static std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&, RunContext * const) {
      ++s_callCount;
      std::cout <<"  MyRunSummaryStreamModule::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   static void globalEndRunSummary(RunIndex const&, RunContext* const, MyRunSummaryCache*) {
      ++s_callCount;
      std::cout <<"  MyRunSummaryStreamModule::globalEndRunSummary called"<<std::endl;
   }
   
   void endRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      ++s_callCount;
      std::cout <<"  MyRunSummaryMyRunSummaryStreamModuleGlobalModule::streamEndRunSummary called"<<std::endl;
   }
};

//------------------------------------------------
class MyEndRunProducer : public GlobalModule<edm::EndRunProducer> {
public:
   MyEndRunProducer(PSet const&) {}
private:
   void globalEndRunProduce(RunIndex const&) const override {
      ++s_callCount;
      std::cout<<" MyEndRunProducer::globalEndRunProduce called"<<std::endl;
   }
};

class MyEndRunSummaryProducer : public GlobalModule<edm::EndRunProducer, edm::RunSummaryCache<MyRunSummaryCache>> {
public:
   MyEndRunSummaryProducer(PSet const&) {}
private:
   
   mutable unsigned int m_orderCounter=0;
   
   std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&) const override {
      assert(m_orderCounter==0);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryProducer::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   void globalEndRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      assert(m_orderCounter==2);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryProducer::globalEndRunSummary called"<<std::endl;
   }
   
   void streamEndRunSummary(StreamID const&, RunIndex const&, MyRunSummaryCache*) const override {
      assert(m_orderCounter==1);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryProducer::streamEndRunSummary called"<<std::endl;
   }
   
   
   
   void globalEndRunProduce(RunIndex const&, MyRunSummaryCache const* ) const override {
      assert(m_orderCounter==3);
      ++m_orderCounter;
      ++s_callCount;
      std::cout<<" MyEndRunSummaryProducer::globalEndRunProduce called"<<std::endl;
   }
};

class MyEndRunStreamProducer : public stream::EndRunProducerEnabler<StreamModuleBase> {
public:
      MyEndRunStreamProducer(PSet const&){}
      
      static void globalEndRunProduce(RunIndex const&, RunContext const*) {
         ++s_callCount;
         std::cout<<" MyEndRunStreamProducer::globalEndRunProduce called"<<std::endl;
      }
};


class MyEndRunSummaryStreamProducer : public stream::EndRunProducerEnabler<stream::RunSummaryCacheHolder<MyRunSummaryCache,StreamModuleBase>> {
public:
   MyEndRunSummaryStreamProducer(PSet const&) {}
private:
   
   static unsigned int m_orderCounter;
   
public:
   static std::shared_ptr<MyRunSummaryCache> globalBeginRunSummary(RunIndex const&, RunContext const*){
      assert(m_orderCounter==0);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryStreamProducer::globalBeginRunSummary called"<<std::endl;
      return std::shared_ptr<MyRunSummaryCache>{new MyRunSummaryCache{0}};
   }
   static void globalEndRunSummary(RunIndex const&, RunContext const*, MyRunSummaryCache*)  {
      assert(m_orderCounter==2);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryStreamProducer::globalEndRunSummary called"<<std::endl;
   }
   
   void endRunSummary(RunIndex const&, MyRunSummaryCache*) const override {
      assert(m_orderCounter==1);
      ++m_orderCounter;
      ++s_callCount;
      std::cout <<"  MyEndRunSummaryStreamProducer::streamEndRunSummary called"<<std::endl;
   }
   
   
   
   static void globalEndRunProduce(RunIndex const&, RunContext const*, MyRunSummaryCache const* ){
      assert(m_orderCounter==3);
      ++m_orderCounter;
      ++s_callCount;
      std::cout<<" MyEndRunSummaryProducer::globalEndRunProduce called"<<std::endl;
   }
};

unsigned int MyEndRunSummaryStreamProducer::m_orderCounter=0;
//------------------------------------------------
void testBeginStream(WorkerBase& iWorker, const char* iName) {
   StreamID sid{ StreamIDMaker::make(0)};
   std::cout <<iName<<" needsBeginStreamTransition="<<iWorker.needsBeginStreamTransition()<<std::endl;
   iWorker.beginStream(sid);
}

void testBeginRun(WorkerBase& iWorker, const char* iName) {
   RunIndex runIndex(0);
   std::cout <<iName<<" needsBeginRunTransition="<<iWorker.needsBeginRunTransition()<<std::endl;
   iWorker.beginRun(runIndex);
   
   StreamID sid{StreamIDMaker::make(0)};
   iWorker.streamBeginRun(sid,runIndex);
   
}

void testBeginLumi(WorkerBase& iWorker, const char* iName) {
   std::cout <<iName<<" needsBeginLumiTransition="<<iWorker.needsBeginLumiTransition()<<std::endl;   
   LumiIndex lumiIndex(RunIndex(0),0);
   iWorker.beginLumi(lumiIndex);

   StreamID sid{StreamIDMaker::make(0)};
   iWorker.streamBeginLumi(sid,lumiIndex);
}

void testEndLumi(WorkerBase& iWorker, const char* iName) {
   std::cout <<iName<<" "<<std::endl;
   LumiIndex lumiIndex(RunIndex(0),0);
   StreamID sid{StreamIDMaker::make(0)};
   iWorker.streamEndLumi(sid,lumiIndex);
   
   iWorker.endLumi(lumiIndex);
}

void testEndRun(WorkerBase& iWorker, const char* iName) {
   RunIndex runIndex(0);
   std::cout <<iName<<std::endl;
   StreamID sid{StreamIDMaker::make(0)};
   iWorker.streamEndRun(sid,runIndex);

   iWorker.endRun(runIndex);
   
   
}


void testEnd(WorkerBase& iWorker, const char* iName) {
   std::cout <<iName<<" needsEndStreamTransition="<<iWorker.needsEndStreamTransition()<<std::endl;
   StreamID sid{StreamIDMaker::make(0)};
   iWorker.endStream(sid);

   iWorker.endJob();
   
   
}

/* Still To Test
   -stream(begin/end)(Lumi/Run) for Global module
   -stream(begin/end)(Lumi/Run) for Stream module
*/

int
main()
{
   PSet pSet;
   
   std::map<std::string, std::shared_ptr<WorkerBase>> workers;
   workers["MyRunGlobalModule"]=Maker<MyRunGlobalModule>::make(pSet);
   workers["MyRunGlobalModule2"]=Maker<MyRunGlobalModule2>::make(pSet);
   workers["MyNonRunGlobalModule"]=Maker<MyNonRunGlobalModule>::make(pSet);
   workers["MySimpleGlobalModule"]=Maker<MyNonRunGlobalModule>::make(pSet);
   workers["MyLumiGlobalModule"]=Maker<MyLumiGlobalModule>::make(pSet);
   workers["MyStreamCacheGlobalModule"]=Maker<MyStreamCacheGlobalModule>::make(pSet);
   workers["MyStreamCacheAllTransGlobalModule"]=Maker<MyStreamCacheAllTransGlobalModule>::make(pSet);
   workers["MyRunSummaryGlobalModule"]=Maker<MyRunSummaryGlobalModule>::make(pSet);
   workers["MyRunSummaryGlobalModule2"]=Maker<MyRunSummaryGlobalModule2>::make(pSet);
   workers["MyRunAndRunSummaryGlobalModule"]=Maker<MyRunAndRunSummaryGlobalModule>::make(pSet);
   workers["MyEndRunProducer"]=Maker<MyEndRunProducer>::make(pSet);
   workers["MyEndRunSummaryProducer"]=Maker<MyEndRunSummaryProducer>::make(pSet);
   
   workers["MyRunStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyRunStreamModule>>::make(pSet);
   workers["MyRunStreamModule2"]=Maker<StreamToGlobalAdaptorModule<MyRunStreamModule2>>::make(pSet);
   workers["MyLumiStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyLumiStreamModule>>::make(pSet);
   workers["MyNonRunStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyNonRunStreamModule>>::make(pSet);
   workers["MyNonRunStreamModule2"]=Maker<StreamToGlobalAdaptorModule<MyNonRunStreamModule2>>::make(pSet);
   workers["MyGlobalCacheStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyGlobalCacheStreamModule>>::make(pSet);
   workers["MyAllTransStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyAllTransStreamModule>>::make(pSet);
   workers["MyRunSummaryStreamModule"]=Maker<StreamToGlobalAdaptorModule<MyRunSummaryStreamModule>>::make(pSet);
   workers["MyEndRunStreamProducer"]=Maker<StreamToGlobalAdaptorModule<MyEndRunStreamProducer>>::make(pSet);
   workers["MyEndRunSummaryStreamProducer"]=Maker<StreamToGlobalAdaptorModule<MyEndRunSummaryStreamProducer>>::make(pSet);

   assert(s_callCount ==1);
   s_callCount=0;
   for(auto const& w : workers){
      w.second->setNumberOfStreams(1);
   }
   std::cout <<"==BeginStream=="<<std::endl;
   for(auto const& w : workers){
      testBeginStream(*(w.second), w.first.c_str());
   }
   //NOTE: all Stream modules are constructed at begin stream but don't see a 'beginStream' transition
   assert(s_callCount ==1+1);
   s_callCount=0;

   std::cout <<"==BeginRun=="<<std::endl;
   for(auto const& w : workers){
      testBeginRun(*(w.second), w.first.c_str());
   }
   assert(s_callCount ==9+1+1+1+1);
   s_callCount = 0;
   
   std::cout <<"==BeginLumi=="<<std::endl;
   for(auto const& w : workers){
      testBeginLumi(*(w.second), w.first.c_str());
   }
   assert(s_callCount ==4);
   s_callCount = 0;

   std::cout <<"==EndLumi=="<<std::endl;
   for(auto const& w : workers){
      testEndLumi(*(w.second), w.first.c_str());
   }
   assert(s_callCount ==2);
   s_callCount = 0;

   std::cout <<"==EndRun=="<<std::endl;
   for(auto const& w : workers){
      testEndRun(*(w.second), w.first.c_str());
   }
   assert(s_callCount ==2+2+2+3+1+3+2+1+3);
   s_callCount = 0;
      
   std::cout <<"==End=="<<std::endl;
   for(auto const& w : workers){
      testEnd(*(w.second), w.first.c_str());
   }


   return 0;
}