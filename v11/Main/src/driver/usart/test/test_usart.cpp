// テストケース記述ファイル
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "usart_driver.h"
#include "stm32f303x8.h"

using ::testing::_;
using ::testing::Invoke;

class MockIo{
    public:
        MOCK_METHOD2(SetBit, void (__IO void*, uint32_t ));
        MOCK_METHOD2(ClearBit, void (__IO void*, uint32_t ));
        MOCK_METHOD2(ReadBit, uint32_t (__IO void*, uint32_t ));
        MOCK_METHOD1(ClearReg, void (__IO void* ));
        MOCK_METHOD2(WriteReg, void (__IO void*, uint32_t ));
        MOCK_METHOD1(ReadReg, uint32_t (__IO void* ));

        void FakeSetBit(__IO void* address, uint32_t bit){
            *((uint32_t*)address) |= bit;
        }

        void FakeClearBit(__IO void* address, uint32_t bit){
            *((uint32_t*)address) &= ~bit;
        }

        void FakeClearReg(__IO void* address){
            *((uint32_t*)address) = 0;
        }
        void FakeWriteReg(__IO void* address, uint32_t data){
            *((uint32_t*)address) = data;
        }

        void DelegateToVirtual() {
            ON_CALL(*this, SetBit(_, _)).WillByDefault(Invoke(this, &MockIo::FakeSetBit));
            ON_CALL(*this, ClearBit(_, _)).WillByDefault(Invoke(this, &MockIo::FakeClearBit));
            ON_CALL(*this, ClearReg(_)).WillByDefault(Invoke(this, &MockIo::FakeClearReg));
            ON_CALL(*this, WriteReg(_, _)).WillByDefault(Invoke(this, &MockIo::FakeWriteReg));
        }
};

using ::testing::NiceMock;

NiceMock<MockIo> *mock;

extern "C" {
    void SetBit(__IO void* address, uint32_t data){
        mock->SetBit(address, data);
    }

    void ClearBit(__IO void* address, uint32_t data){
        mock->ClearBit(address, data);
    }

    void ReadBit(__IO void* address, uint32_t data){
        mock->ReadBit(address, data);
    }

    void ClearReg(__IO void* address){
        mock->ClearReg(address);
    }

    void WriteReg(__IO void* address, uint32_t data){
        mock->WriteReg(address, data);
    }

    uint32_t ReadReg(__IO void* address){
        return mock->ReadReg(address);
    }
}

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class UsartTest : public ::testing::Test {
    public:
        RCC_TypeDef *virtualRcc;
        GPIO_TypeDef *virtualGpio;
        USART_TypeDef *virtualUsart;
        UsartDriver& UsartDrv = UsartDriver::GetInstance();

    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new NiceMock<MockIo>();
            virtualRcc = new RCC_TypeDef();
            virtualGpio = new GPIO_TypeDef();
            virtualUsart = new USART_TypeDef();
            UsartDrv.SetBase(virtualRcc, virtualGpio, virtualUsart);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualRcc;
            delete virtualGpio;
            delete virtualUsart;
        }
};

// テストケース
TEST_F(UsartTest, Init)
{
    mock->DelegateToVirtual();

    UsartDrv.Init();

    EXPECT_EQ(RCC_AHBENR_GPIOAEN, virtualRcc->AHBENR & RCC_AHBENR_GPIOAEN);
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
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_RXNE)).WillOnce(Return(0));
    EXPECT_EQ(0, UsartDrv.IsReadEnable());
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_RXNE)).WillOnce(Return(USART_ISR_RXNE));
    EXPECT_EQ(USART_ISR_RXNE, UsartDrv.IsReadEnable());
}

TEST_F(UsartTest, IsWriteEnable)
{
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_TXE)).WillOnce(Return(0));
    EXPECT_EQ(0, UsartDrv.IsWriteEnable());
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_TXE)).WillOnce(Return(USART_ISR_TXE));
    EXPECT_EQ(USART_ISR_TXE, UsartDrv.IsWriteEnable());
}

TEST_F(UsartTest, Read)
{
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_RXNE)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_RXNE)).WillRepeatedly(Return(USART_ISR_RXNE));
    EXPECT_CALL(*mock, ReadReg(&virtualUsart->RDR)).WillRepeatedly(Return('a'));

    EXPECT_EQ('a', UsartDrv.Read());
}

TEST_F(UsartTest, Write)
{
    mock->DelegateToVirtual();

    char c = 's';

    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_TXE)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualUsart->ISR, USART_ISR_TXE)).WillRepeatedly(Return(USART_ISR_TXE));
    EXPECT_CALL(*mock, WriteReg(&virtualUsart->TDR, c));

    UsartDrv.Write(c);
    EXPECT_EQ(c, virtualUsart->TDR);
}
