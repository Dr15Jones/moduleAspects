#ifndef MODULEABILITYENUMS_H_DNR3OD13
#define MODULEABILITYENUMS_H_DNR3OD13


typedef  unsigned char AbilitiesType;

enum class Abilities {
   kGlobalCache,
   kStreamCache,
   kRunCache,
   kLuminosityBlockCache,
   kRunSummaryCache,
   kLuminosityBlockSummaryCache,
   kBeginRunProducer,
   kEndRunProducer
};

namespace AbilityBits { 
   enum Bits {
      kGlobalCache=1,
      kStreamCache=2,
      kRunCache=4,
      kLuminosityBlockCache=8,
      kRunSummaryCache=16,
      kLuminosityBlockSummaryCache=32,
      kBeginRunProducer=64,
      kEndRunProducer=128
   };
}

namespace AbilityToTransitions {
   enum Bits {
      kBeginStream=AbilityBits::kStreamCache,
      kEndStream=AbilityBits::kStreamCache,

      kGlobalBeginRun=AbilityBits::kRunCache|AbilityBits::kRunSummaryCache,
      kGlobalEndRun=AbilityBits::kRunCache|AbilityBits::kRunSummaryCache|AbilityBits::kEndRunProducer,
      kStreamBeginRun=AbilityBits::kStreamCache,
      kStreamEndRun=AbilityBits::kStreamCache|AbilityBits::kRunSummaryCache,

      kGlobalBeginLuminosityBlock=AbilityBits::kLuminosityBlockCache|AbilityBits::kLuminosityBlockSummaryCache,
      kGlobalEndLuminosityBlock=AbilityBits::kLuminosityBlockCache|AbilityBits::kLuminosityBlockSummaryCache,
      kStreamBeginLuminosityBlock=AbilityBits::kStreamCache|AbilityBits::kLuminosityBlockSummaryCache,
      kStreamEndLuminosityBlock=AbilityBits::kStreamCache|AbilityBits::kLuminosityBlockSummaryCache

   };
}

#endif /* end of include guard: MODULEABILITYENUMS_H_DNR3OD13 */
