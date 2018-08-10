// テストケース記述ファイル
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "flash_driver.h"
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

FLASH_TypeDef *virtualFlash;
uint8_t* virtualAddress;
uint32_t virtualStart;
uint32_t virtualEnd;
FlashDriver& FlashDrv = FlashDriver::GetInstance();

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class FlashTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new NiceMock<MockIo>();
            virtualFlash = new FLASH_TypeDef();
            virtualAddress = new uint8_t[100];
            virtualStart = (uint32_t)&virtualAddress[0];
            virtualEnd = (uint32_t)&virtualAddress[100]; // 範囲外のアドレスを取得
            FlashDrv.SetBase(virtualFlash, virtualStart, virtualEnd);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualFlash;
            delete virtualAddress;
        }
};

// テストケース
TEST_F(FlashTest, Init)
{
    mock->DelegateToVirtual();

    EXPECT_CALL(*mock, WriteReg(&virtualFlash->KEYR, FLASH_KEY1));
    EXPECT_CALL(*mock, WriteReg(&virtualFlash->KEYR, FLASH_KEY2));

    FlashDrv.Init();

    EXPECT_EQ(FLASH_KEY2, virtualFlash->KEYR);
}


TEST_F(FlashTest, ReadOK)
{
    mock->DelegateToVirtual();

    virtualAddress[30] = 0xDE;

    EXPECT_EQ(0xDE, FlashDrv.Read(&virtualAddress[30]));
}

TEST_F(FlashTest, ReadNG_AddrUnder)
{
    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.Read(&virtualAddress[-1]));
}

TEST_F(FlashTest, ReadNG_AddrOver)
{
    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.Read(&virtualAddress[100]));
}

using ::testing::Return;

TEST_F(FlashTest, WriteOK)
{
    mock->DelegateToVirtual();

    virtualAddress[0] = 0;
    virtualAddress[1] = 0;
    uint16_t* dummy = (uint16_t*)&virtualAddress[0];
    uint16_t data = 0xBEEF;

    EXPECT_CALL(*mock, ReadBit(&virtualFlash->CR, FLASH_CR_LOCK)).WillOnce(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_PG));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_EOP)).WillOnce(Return(FLASH_SR_EOP));
    EXPECT_CALL(*mock, ClearBit(&virtualFlash->SR, FLASH_SR_EOP));
    
    EXPECT_EQ(FLASH_RESULT_OK, FlashDrv.Write(dummy, data));

    EXPECT_EQ(*dummy, data);
}

TEST_F(FlashTest, WriteNG_EOP)
{
    mock->DelegateToVirtual();

    uint16_t* dummy = (uint16_t*)&virtualAddress[0];
    uint16_t data = 0xBEEF;

    EXPECT_CALL(*mock, ReadBit(&virtualFlash->CR, FLASH_CR_LOCK)).WillOnce(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_PG));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_EOP)).WillOnce(Return(0));
    
    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.Write(dummy, data));
}

TEST_F(FlashTest, WriteNG_AddrUnder)
{
    mock->DelegateToVirtual();

    uint16_t* dummy = (uint16_t*)&virtualAddress[-1];
    uint16_t data = 0xBEEF;

    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.Write(dummy, data));
}

TEST_F(FlashTest, WriteNG_AddrOver)
{
    mock->DelegateToVirtual();

    uint16_t* dummy = (uint16_t*)&virtualAddress[99];
    uint16_t data = 0xBEEF;

    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.Write(dummy, data));
}

TEST_F(FlashTest, PageEraseOK)
{
    mock->DelegateToVirtual();

    uint8_t *dummy = &virtualAddress[99];

    EXPECT_CALL(*mock, ReadBit(&virtualFlash->CR, FLASH_CR_LOCK)).WillOnce(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_PER));
    EXPECT_CALL(*mock, WriteReg(&virtualFlash->AR, (uint32_t)dummy));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_STRT));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_EOP)).WillOnce(Return(FLASH_SR_EOP));
    EXPECT_CALL(*mock, ClearBit(&virtualFlash->SR, FLASH_SR_EOP));

    EXPECT_EQ(FLASH_RESULT_OK, FlashDrv.PageErase(dummy));
}

TEST_F(FlashTest, PageEraseNG_EOP)
{
    mock->DelegateToVirtual();

    uint8_t *dummy = &virtualAddress[99];

    EXPECT_CALL(*mock, ReadBit(&virtualFlash->CR, FLASH_CR_LOCK)).WillOnce(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_PER));
    EXPECT_CALL(*mock, WriteReg(&virtualFlash->AR, (uint32_t)dummy));
    EXPECT_CALL(*mock, SetBit(&virtualFlash->CR, FLASH_CR_STRT));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(FLASH_SR_BSY));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_BSY)).WillRepeatedly(Return(0));
    EXPECT_CALL(*mock, ReadBit(&virtualFlash->SR, FLASH_SR_EOP)).WillOnce(Return(0));

    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.PageErase(dummy));
}

TEST_F(FlashTest, PageEraseNG_AddrUnder)
{
    mock->DelegateToVirtual();

    uint8_t *dummy = &virtualAddress[-1];

    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.PageErase(dummy));
}

TEST_F(FlashTest, PageEraseNG_AddrOver)
{
    mock->DelegateToVirtual();

    uint8_t *dummy = &virtualAddress[100];

    EXPECT_EQ(FLASH_RESULT_NG, FlashDrv.PageErase(dummy));
}
