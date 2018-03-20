// テストケース記述ファイル
#include "gtest/gtest.h" 
#include "gmock/gmock.h"

// テスト対象関数を呼び出せるようにするのだが
// extern "C"がないとCと解釈されない
extern "C" {
#include "timer_driver.h"
#include "stm32f303x8.h"
#include "intr_driver.h"
}

using ::testing::_;
using ::testing::Invoke;

class MockIo{
	public:
		MOCK_METHOD1(RegClear, void (uint32_t* ));
		MOCK_METHOD2(RegWrite, void (uint32_t*, uint32_t ));
		MOCK_METHOD1(RegRead, uint32_t (uint32_t* ));

		void FakeClear(uint32_t* address){
			*address = 0;
		}
		void FakeWrite(uint32_t* address, uint32_t data){
			*address |= data;
		}
	
		void DelegateToVirtual() {
			ON_CALL(*this, RegClear(_)).WillByDefault(Invoke(this, FakeClear)); //RegClearのcall時に実行される
			ON_CALL(*this, RegWrite(_, _)).WillByDefault(Invoke(this, FakeWrite)); //RegWriteのcall時に実行される
		}
};

MockIo *mock;

extern "C" {

void RegClear(uint32_t* address){
	mock->RegClear(address);
}

void RegWrite(uint32_t* address, uint32_t data){
	mock->RegWrite(address, data);
}

uint32_t RegRead(uint32_t* address){
    return mock->RegRead(address);
}

void IntrHandlerSet(intr_type_t type, intr_handler_t handler){
}

}

RCC_TypeDef *virtualRCC;
TIM_TypeDef *virtualTIM;

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class TimerTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
			mock = new MockIo();
    		virtualRCC = new RCC_TypeDef();
    		virtualTIM = new TIM_TypeDef();
    		TimerCreate(virtualRCC, virtualTIM, TIM6_DAC1_IRQn);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
			delete mock;
        }
};

// テストケース
TEST_F(TimerTest, Init)
{
}
