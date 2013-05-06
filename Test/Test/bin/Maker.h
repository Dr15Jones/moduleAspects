#ifndef MAKER_H_BQNVX4LS
#define MAKER_H_BQNVX4LS

#include "Worker.h"

template<typename T>
struct DoMake {
   static std::unique_ptr<WorkerBase> make(PSet const& iPSet){
      return std::unique_ptr<WorkerBase>{ new Worker<T>{ new T{iPSet}}};
   }
};

template<typename T>
class Maker {
private:
   typedef DoMake<T> MakerType;
public:
   static std::unique_ptr<WorkerBase> make(PSet const& iPSet) {
      return MakerType::make(iPSet);
   }
 
};

#endif /* end of include guard: MAKER_H_BQNVX4LS */
