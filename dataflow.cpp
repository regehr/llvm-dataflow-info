#include "llvm/Analysis/DemandedBits.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/LazyValueInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/ConstantRange.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/InitializePasses.h"

#include <string>

using namespace llvm;

namespace {

std::string bitString(APInt V) {
  std::string s = "";
  for (int i = 0; i < V.getBitWidth(); i++) {
    if (V.isNegative())
      s += "1";
    else
      s += "0";
    V <<= 1;
  }
  return s;
}

std::string bitString(KnownBits Known) {
  std::string s = "";
  for (int x = 0; x < Known.getBitWidth(); ++x) {
    if (Known.Zero.isSignBitSet())
      s += "0";
    else if (Known.One.isSignBitSet())
      s += "1";
    else
      s += "?";
    Known.Zero <<= 1;
    Known.One <<= 1;
  }
  return s;
}

struct HelloWorld : public FunctionPass {
  static char ID;
  HelloWorld() : FunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &Info) const {
    Info.addRequired<LazyValueInfoWrapperPass>();
    Info.addRequired<ScalarEvolutionWrapperPass>();
    Info.addRequired<DemandedBitsWrapperPass>();
  }

  bool runOnFunction(Function &F) override {
    auto LVI = &getAnalysis<LazyValueInfoWrapperPass>().getLVI();
    if (!LVI)
      report_fatal_error("LVI initialization failed");
    auto SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
    if (!SE)
      report_fatal_error("SCEV initialization failed");
    DemandedBits *DB = &getAnalysis<DemandedBitsWrapperPass>().getDemandedBits();
    if (!DB)
      report_fatal_error("demanded bits initialization failed");
    Module *M = F.getParent();
    errs() << "function: ";
    errs().write_escaped(F.getName()) << '\n';
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (!I.getType()->isIntegerTy())
          continue;
        errs() << I << "\n";

        errs() << "    ";
        KnownBits Known = computeKnownBits(&I, M->getDataLayout());
        errs() << "known: " << bitString(Known) << "\n";

        errs() << "    ";
        errs() << "isKnownToBePowerOfTwo = " <<
          isKnownToBeAPowerOfTwo(&I, M->getDataLayout()) << "\n";

        errs() << "    LVI: ";
        errs() << LVI->getConstantRange(&I, &BB);
        errs() << "\n";

        auto SC = SE->getSCEV(&I);
        errs() << "    SCEV: ";
        SC->print(errs());
        errs() << "  " << SE->getSignedRange(SC);
        errs() << "  " << SE->getUnsignedRange(SC);
        errs() << "\n";

        auto Demanded = DB->getDemandedBits(&I);
        errs() << "    demanded: " << bitString(Demanded) << "\n";
      }
    }
    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char HelloWorld::ID = 0;

static RegisterPass<HelloWorld> X("helloworld", "Hello World Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

namespace llvm {
void initializeHelloWorldPass(llvm::PassRegistry &);
}

INITIALIZE_PASS_BEGIN(HelloWorld, "hello",
                      "Lazy Value Info Printer Pass", false, false)
INITIALIZE_PASS_DEPENDENCY(LazyValueInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DemandedBitsWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_END(HelloWorld, "hello",
                    "Lazy Value Info Printer Pass", false, false)
