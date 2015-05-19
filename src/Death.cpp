
#include <g2log.hpp>
#include <unistd.h>
#include <iostream>
#include "Death.h"

/**
 * Singleton Instance Method
 * @return 
 */
Death& Death::Instance() {
   static Death gInstance;

   return gInstance;
}

Death::Death() : mReceived(false), mMessage {""}, mEnableDefaultFatal(false)
{

}

/**
 * The most common use case is for ZMQ sockets to want their files cleared on 
 *   fatal exits
 * @param binding
 */
void Death::DeleteIpcFiles(const DeathCallbackArg& binding) {
   auto realPathStart = binding.find("ipc://");
   if (realPathStart != std::string::npos) {
      std::string realPath(binding,realPathStart+6); // start at the end of the matched string
      unlink(realPath.c_str());
   }
}

/**
 * In order to re-enable the default handler you must re-supply the worker 
 * @param loggerWorker
 */
void Death::EnableDefaultFatalCall() {
   Death::Instance().mEnableDefaultFatal = true;
   Death::SetupExitHandler();


}
/// @param death message with any captured death details

void Death::Received(g2::internal::FatalMessage death) {

   thread_local bool recursiveDeathDetect = false;

   // lambda for quick exit
   auto clearCallbacksThenFatalExit = [&](g2::internal::FatalMessage death) {
      if (Death::Instance().mEnableDefaultFatal) {
         ClearExits();
         g2::internal::fatalCallToLogger(death);
      }
      recursiveDeathDetect = false; // reset for test purposes
   };

   // Recursive fatal was discovered
   if (Death::Instance().mReceived  && recursiveDeathDetect) {
      std::cerr << "Recursive crash detected. Aborting death-hook calls" << std::endl;
      clearCallbacksThenFatalExit(death);
      return;
   }


   std::lock_guard<std::mutex> glock(Death::Instance().mListLock);
   Death::Instance().mReceived = true;
   Death::Instance().mMessage = death.message_;
   recursiveDeathDetect = true;
   for (const auto& deathFunction : Death::Instance().mShutdownFunctions) {
      // semi-dangerous in case one function would trigger another FATAL
      // as long as it is in the same thread then we will capture that above
      (deathFunction.first)(deathFunction.second); 
   }
   clearCallbacksThenFatalExit(death);
}

/**
 * Register a DeathCallback into the set of functions that will be called
 * @return 
 */
void Death::RegisterDeathEvent(DeathCallbackType deathFunction, const DeathCallbackArg& deathArg) {
   std::lock_guard<std::mutex> glock(Death::Instance().mListLock);
   Death::Instance().mShutdownFunctions.push_back(std::make_pair(deathFunction,deathArg));
}

bool Death::WasKilled() {
   return Death::Instance().mReceived;
}

/// Please call this if you plan on doing DEATH tests. 

void Death::SetupExitHandler() {
   g2::internal::changeFatalInitHandler(Death::Received);
}

void Death::ClearExits() {
   Death::Instance().mReceived = false;
   Death::Instance().mMessage = "";
   Death::Instance().mShutdownFunctions.clear();
}

 std::string Death::Message() {
    return Death::Instance().mMessage;
 }

