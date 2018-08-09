#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "led_driver.h"

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

RCC_TypeDef *virtualRCC;
GPIO_TypeDef *virtualGPIO;
LedDriver& LedDrv = LedDriver::GetInstance();

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class LedTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new NiceMock<MockIo>();
            virtualRCC = new RCC_TypeDef();
            virtualGPIO = new GPIO_TypeDef();
            LedDrv.SetBase(virtualRCC, virtualGPIO);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualRCC;
            delete virtualGPIO;
        }
};

// テストケース
TEST_F(LedTest, Init)
{
    mock->DelegateToVirtual();

    LedDrv.Init();

    EXPECT_EQ(virtualRCC->AHBENR & RCC_AHBENR_GPIOBEN_Msk, RCC_AHBENR_GPIOBEN);
    EXPECT_EQ(virtualGPIO->MODER & GPIO_MODER_MODER3_Msk, GPIO_MODER_MODER3_0);
    EXPECT_EQ(virtualGPIO->OTYPER & GPIO_OTYPER_OT_3, 0);
    EXPECT_EQ(virtualGPIO->PUPDR & GPIO_PUPDR_PUPDR3_Msk, 0);
    EXPECT_EQ(virtualGPIO->ODR & GPIO_ODR_3, 0);
}


TEST_F(LedTest, On)
{
    mock->DelegateToVirtual();

    LedDrv.On();

    EXPECT_EQ(virtualGPIO->ODR & GPIO_ODR_3, GPIO_ODR_3);
}

TEST_F(LedTest, Off)
{
    mock->DelegateToVirtual();

    LedDrv.Off();

    EXPECT_EQ(virtualGPIO->ODR & GPIO_ODR_3, 0);
}
