// テストケース記述ファイル
#include "gtest/gtest.h" // googleTestを使用するおまじないはこれだけでOK
#include "gmock/gmock.h"
// テスト対象関数を呼び出せるようにするのだが
// extern "C"がないとCと解釈されない、意外とハマりがち。
extern "C" {
#include "usart_driver.h"
}

class MockIo{
	public:
		MOCK_METHOD1(RegClear, void (uint32_t*));
		MOCK_METHOD2(RegWrite, void (uint32_t*, uint32_t));
		MOCK_METHOD3(RegRead, uint32_t (uint32_t*));
};

MockIo *mock;

void RegClear(uint32_t* address){
	mock->RegClear(address);
}

void RegWrite(uint32_t* address, uint32_t data){
	mock->RegWrite(address, data);
}

uint32_t RegRead(uint32_t* address){
	return mock->RegRead(address);
}

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class UsartTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
			mock = new MockIo();
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
			delete mock;
        }
};

// テストケース。細かい説明はGoogleTestのマニュアルを見てね。
TEST_F(UsartTest, Init)
{
    RCC_TypeDef virtualRcc = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    GPIO_TypeDef virtualGpio = {0, 0, 0, 0, 0, 0, 0, 0, {0,0}, 0};
    USART_TypeDef virtualUsart = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    UsartCreate(&virtualRcc, &virtualGpio, &virtualUsart);
    UsartInit();
    EXPECT_EQ(RCC_AHBENR_GPIOAEN, virtualRcc.AHBENR);
    EXPECT_EQ(GPIO_MODER_MODER2_1|GPIO_MODER_MODER15_1, virtualGpio.MODER);
    EXPECT_EQ(0x700, virtualGpio.AFR[0]);
    EXPECT_EQ(0x70000000, virtualGpio.AFR[1]);
    EXPECT_EQ(RCC_APB1ENR_USART2EN, virtualRcc.APB1ENR);
    EXPECT_EQ(8000000L/115200L, virtualUsart.BRR);
    EXPECT_EQ(USART_CR1_RE|USART_CR1_TE|USART_CR1_UE, virtualUsart.CR1);
}

using ::testing::Return;
using ::testing::Pointee;

TEST_F(UsartTest, Read)
{
	//EXPECT_CALL(*mock, RegRead(::testing::_)).WillOnce(testing::Return(USART_ISR_RXNE));
}
