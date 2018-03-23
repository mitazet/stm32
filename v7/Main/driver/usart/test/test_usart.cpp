// テストケース記述ファイル
#include "gtest/gtest.h"
#include "gmock/gmock.h"

// テスト対象関数を呼び出せるようにするのだが
// extern "C"がないとCと解釈されない
extern "C" {
#include "usart_driver.h"
#include "stm32f303x8.h"
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
			ON_CALL(*this, RegClear(_)).WillByDefault(Invoke(this, &MockIo::FakeClear));
			ON_CALL(*this, RegWrite(_, _)).WillByDefault(Invoke(this, &MockIo::FakeWrite));
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
}

RCC_TypeDef *virtualRcc;
GPIO_TypeDef *virtualGpio;
USART_TypeDef *virtualUsart;

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class UsartTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
			mock = new MockIo();
    		virtualRcc = new RCC_TypeDef();
    		virtualGpio = new GPIO_TypeDef();
    		virtualUsart = new USART_TypeDef();
    		UsartCreate(virtualRcc, virtualGpio, virtualUsart);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
			delete mock;
        }
};

// テストケース
TEST_F(UsartTest, Init)
{
	mock->DelegateToVirtual();

	EXPECT_CALL(*mock, RegWrite(_, _)).Times(7); //回数は問題ではないので微妙だがWarningがでるため
	EXPECT_CALL(*mock, RegClear(_)).Times(3);

    UsartInit();

    EXPECT_EQ(RCC_AHBENR_GPIOAEN, virtualRcc->AHBENR);
    EXPECT_EQ(GPIO_MODER_MODER2_1|GPIO_MODER_MODER15_1, virtualGpio->MODER);
    EXPECT_EQ(0x700, virtualGpio->AFR[0]);
    EXPECT_EQ(0x70000000, virtualGpio->AFR[1]);
    EXPECT_EQ(RCC_APB1ENR_USART2EN, virtualRcc->APB1ENR);
    EXPECT_EQ(8000000L/115200L, virtualUsart->BRR);
    EXPECT_EQ(USART_CR1_RE|USART_CR1_TE|USART_CR1_UE, virtualUsart->CR1);
}

using ::testing::Return;

TEST_F(UsartTest, IsReadEnable)
{
	EXPECT_CALL(*mock, RegRead(_)).WillOnce(Return(0));
    EXPECT_EQ(0, UsartIsReadEnable());
	EXPECT_CALL(*mock, RegRead(_)).WillOnce(Return(USART_ISR_RXNE));
    EXPECT_EQ(USART_ISR_RXNE, UsartIsReadEnable());
}

TEST_F(UsartTest, IsWriteEnable)
{
	EXPECT_CALL(*mock, RegRead(_)).WillOnce(Return(0));
    EXPECT_EQ(0, UsartIsWriteEnable());
	EXPECT_CALL(*mock, RegRead(_)).WillOnce(Return(USART_ISR_TXE));
    EXPECT_EQ(USART_ISR_TXE, UsartIsWriteEnable());
}

TEST_F(UsartTest, Read)
{
	EXPECT_CALL(*mock, RegRead((uint32_t*)&virtualUsart->ISR)).WillRepeatedly(Return(0));
	EXPECT_CALL(*mock, RegRead((uint32_t*)&virtualUsart->ISR)).WillRepeatedly(Return(USART_ISR_RXNE));
	EXPECT_CALL(*mock, RegRead((uint32_t*)&virtualUsart->RDR)).WillRepeatedly(Return('a'));
	
    EXPECT_EQ('a', UsartRead());
}

TEST_F(UsartTest, Write)
{
	mock->DelegateToVirtual();

	char c = 's';

	EXPECT_CALL(*mock, RegRead((uint32_t*)&virtualUsart->ISR)).WillRepeatedly(Return(0));
	EXPECT_CALL(*mock, RegRead((uint32_t*)&virtualUsart->ISR)).WillRepeatedly(Return(USART_ISR_TXE));
	EXPECT_CALL(*mock, RegWrite((uint32_t*)&virtualUsart->TDR, c));

	UsartWrite(c);
	EXPECT_EQ(c, virtualUsart->TDR);
}
