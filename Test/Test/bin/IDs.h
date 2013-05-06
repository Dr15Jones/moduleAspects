#ifndef IDS_H_H02RKVHE
#define IDS_H_H02RKVHE
struct PSet {
};

class StreamIDMaker;

class StreamID {
public:
   StreamID(StreamID &&) = default;
   
   unsigned int id() const { return m_id;}
   
   bool operator==(StreamID const& iRHS) const {
      return m_id == iRHS.m_id;
   }
   
   bool operator<(StreamID const& iRHS) const {
      return m_id < iRHS.m_id;
   }
private:
   StreamID& operator=(StreamID const &) = delete;
   StreamID(StreamID const&) = delete;

   friend class StreamIDMaker;
   StreamID(unsigned int iID): m_id(iID) {}

   unsigned int m_id;   
};

class StreamIDMaker {
public:
   static StreamID make(unsigned int iID) {
      return StreamID(iID);
   }
};

class RunIndex {
public:
   RunIndex(unsigned int iIndex): m_index(iIndex){}
   unsigned int index() const { return m_index;}
private:
   unsigned int m_index;
};

//Allowed to access Run products from the Lumi transitions
class LumiIndex {
public:
   LumiIndex(RunIndex const& iRunIndex, unsigned int iIndex): m_runIndex{iRunIndex},m_index(iIndex){}
   RunIndex const& runIndex() const { return m_runIndex;}
   unsigned int index() const { return m_index;}
private:
   RunIndex m_runIndex;
   unsigned int m_index;
};



#endif /* end of include guard: IDS_H_H02RKVHE */
