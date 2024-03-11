#include <iostream>
#include <fstream>
#include <thread>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AtLeast;

extern "C" {
  #include "test_main.h"
}

/* mock for Driver */
class MockDriver {
  public:
    MOCK_METHOD0(Clock_ComEsp32Config, void());
};

MockDriver* mock;

extern "C" {
  void Clock_ComEsp32Config(){
      mock->Clock_ComEsp32Config();
  }
}
/* mock for Driver */

/* setup */
class TestFreeRTOS : public ::testing::Test {
    protected:
        virtual void SetUp(){
          mock = new MockDriver();
        }

        virtual void TearDown(){
          delete mock;
        }
};

/* case */
TEST_F(TestFreeRTOS, test1) {
  /* check call for driver */
  EXPECT_CALL(*mock, Clock_ComEsp32Config()).Times(1);

  /* run FreeRTOS */
  std::thread threadFreeRtos(vTestMain);
  threadFreeRtos.detach();

  /* wait for FreeRTOS Task running */
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/* start test */
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
