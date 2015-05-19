#include <thread>
#include <future>

#include "DeathTest.h"
#include <Death.h>
#include <FileIO.h>
#include <cassert>

extern std::shared_ptr<g2LogWorker> g2logger;
bool DeathTest::ranEcho(false);
std::vector<Death::DeathCallbackArg> DeathTest::stringsEchoed;
int DeathTest::ranTimes(0);
TEST(DeathTest, DeleteIpcFilesRemovesIPCFiles) {
   DeathTest::ranEcho = false;
   DeathTest::ranTimes = 0;
   DeathTest::stringsEchoed.clear();
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();
   Death::Instance().RegisterDeathEvent(&Death::DeleteIpcFiles, "ipc:///tmp/test.ipc");
   unlink("/tmp/test.ipc");
   ASSERT_FALSE(FileIO::DoesFileExist("/tmp/test.ipc"));
   ASSERT_FALSE(FileIO::WriteAsciiFileContent("/tmp/test.ipc", "test").HasFailed());
   ASSERT_TRUE(FileIO::DoesFileExist("/tmp/test.ipc"));
   CHECK(false);
   ASSERT_FALSE(FileIO::DoesFileExist("/tmp/test.ipc"));
}

TEST(DeathTest, DeleteIpcFilesDoesntRemoveNonIPCFiles) {
   DeathTest::ranEcho = false;
   DeathTest::ranTimes = 0;
   DeathTest::stringsEchoed.clear();
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();
   Death::Instance().RegisterDeathEvent(&Death::DeleteIpcFiles, "tcp:///tmp/test.deathtest");
   unlink("/tmp/test.deathtest");
   ASSERT_FALSE(FileIO::DoesFileExist("/tmp/test.deathtest"));
   ASSERT_FALSE(FileIO::WriteAsciiFileContent("/tmp/test.deathtest", "test").HasFailed());
   ASSERT_TRUE(FileIO::DoesFileExist("/tmp/test.deathtest"));
   CHECK(false);
   ASSERT_TRUE(FileIO::DoesFileExist("/tmp/test.deathtest"));
   unlink("/tmp/test.deathtest");
}
void DeathTest::EchoTheString(const std::string& theString) {
   stringsEchoed.push_back(theString);
   ranEcho = true;
}

void DeathTest::RaceTest(const Death::DeathCallbackArg& theString) {
   if (!ranEcho) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(rand() % 16000));
      ranEcho = !ranEcho;
      stringsEchoed.push_back(theString);
      
   }
   ranTimes++;
}

TEST(DeathTest, VerifySingleton) {
   Death & instance1(Death::Instance());
   Death & instance2(Death::Instance());

   ASSERT_EQ(&instance1, &instance2);
}

TEST(DeathTest, VerifyReceiveCheck) {
   RaiiDeathCleanup cleanup;
   Death::ClearExits();

   Death::Instance().SetupExitHandler();

   EXPECT_FALSE(Death::Instance().WasKilled());
   CHECK(false);
   EXPECT_TRUE(Death::Instance().WasKilled());
}

TEST(DeathTest, VerifyReceiveSignal) {
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();

   EXPECT_FALSE(Death::Instance().WasKilled());
   raise(SIGSEGV);
   EXPECT_TRUE(Death::Instance().WasKilled());
}

namespace {
   size_t gDeathCounter = 0;
}

TEST(DeathTest, VerifyRecursiveCrash) {
   RaiiDeathCleanup cleanup;
   Death::ClearExits();
   Death::Instance().SetupExitHandler();
   gDeathCounter = 0;
   
      auto deathCounter = [](const Death::DeathCallbackArg& arg) {
      std::cout << "Death message: " << arg << std::endl;
      ++gDeathCounter;
      assert(gDeathCounter < 10); // just sanity check in case logic changes
      CHECK(false);
   };

   Death::Instance().RegisterDeathEvent(deathCounter, "test");

   EXPECT_FALSE(Death::Instance().WasKilled());
   CHECK(false);
   EXPECT_TRUE(Death::Instance().WasKilled());
   EXPECT_EQ(gDeathCounter, 1);
}


TEST(DeathTest, ResetWorks) {
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();

   EXPECT_FALSE(Death::Instance().WasKilled());
   CHECK(false);
   EXPECT_TRUE(Death::Instance().WasKilled());
   Death::Instance().ClearExits();
   EXPECT_FALSE(Death::Instance().WasKilled());
}

TEST(DeathTest, RegisterSomething) {
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();
   Death::Instance().RegisterDeathEvent(&DeathTest::EchoTheString, "test");
   EXPECT_FALSE(DeathTest::ranEcho);
   EXPECT_TRUE(DeathTest::stringsEchoed.empty());
   CHECK(false);
   EXPECT_TRUE(DeathTest::ranEcho);
   EXPECT_FALSE(DeathTest::stringsEchoed.empty());
   EXPECT_EQ("test", DeathTest::stringsEchoed[0]);
}

TEST(DeathTest, ThreadSafeTest) {
   DeathTest::ranEcho = false;
   DeathTest::ranTimes = 0;
   DeathTest::stringsEchoed.clear();
   RaiiDeathCleanup cleanup;
   Death::Instance().SetupExitHandler();
   
   EXPECT_FALSE(DeathTest::ranEcho);
   EXPECT_TRUE(DeathTest::stringsEchoed.empty());
   
   auto ManyThreads = [&]() {
      Death::Instance().RegisterDeathEvent(&DeathTest::RaceTest, "race");
   };
   std::vector<std::future<void>> waitingPromises;
   for (int i = 0; i < 10000; i++) {
      waitingPromises.push_back(std::async(std::launch::async, ManyThreads));
   }
   for (auto& waitFor : waitingPromises) {
      waitFor.get();
   }
   CHECK(false);

   EXPECT_TRUE(DeathTest::ranEcho);
   EXPECT_FALSE(DeathTest::stringsEchoed.empty());
   EXPECT_EQ(10000, DeathTest::ranTimes);
   EXPECT_EQ(1,DeathTest::stringsEchoed.size());
   EXPECT_EQ("race", DeathTest::stringsEchoed[0]);
}


// 
TEST(DeathTest, DISABLED_VerifyReceiveSignalAndExitForReal) {
   std::cout << "Running this test will kill the test process ... keep it disabled if possible" << std::endl;
   std::cout << "If the test succeeds then another printout will come that says 'Death message: SUCCESS'" << std::endl;
   RaiiDeathCleanup cleanup;
   auto deathTestCallback = [](const Death::DeathCallbackArg& arg) {
      std::cout << "Death message: " << arg << std::endl;
      CHECK(false); // make it a recursive death for good measure
   };

   Death::EnableDefaultFatalCall();
   std::string msg = {"SUCCESS"};
   Death::RegisterDeathEvent(deathTestCallback, msg);

   EXPECT_FALSE(Death::Instance().WasKilled());
   raise(SIGSEGV);
   EXPECT_TRUE(Death::Instance().WasKilled());
}


