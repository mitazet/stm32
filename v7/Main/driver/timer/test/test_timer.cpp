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

class Mock{
    public:
        MOCK_METHOD1(NVIC_EnableIRQ, void (IRQn_Type));
        MOCK_METHOD1(NVIC_DisableIRQ, void (IRQn_Type));
        MOCK_METHOD2(IntrHandlerSet, void (intr_type_t, intr_handler_t));
};

Mock *mock;
uint32_t virtualISER[8];

extern "C" {

    void SetBit(__IO void* address, uint32_t bit){
        *((uint32_t*)address) |= bit;
    }

    void ClearBit(__IO void* address, uint32_t bit){
        *((uint32_t*)address) &= ~bit;
    }

    void ClearReg(__IO void* address){
        *((uint32_t*)address) = 0;
    }

    void WriteReg(__IO void* address, uint32_t data){
        *((uint32_t*)address) = data;
    }

    void NVIC_EnableIRQ(IRQn_Type IRQn){
        mock->NVIC_EnableIRQ(IRQn);
    }

    void NVIC_DisableIRQ(IRQn_Type IRQn){
        mock->NVIC_DisableIRQ(IRQn);
    }

    void IntrHandlerSet(intr_type_t type, intr_handler_t handler){
        mock->IntrHandlerSet(type, handler);
    }

    void dummy_function(void){
    }
}

RCC_TypeDef *virtualRCC;
TIM_TypeDef *virtualTIM;
IRQn_Type virtualIRQn;

extern void (*TimerTimeupFunction)(void);

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class TimerTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new Mock();
            virtualRCC = new RCC_TypeDef();
            virtualTIM = new TIM_TypeDef();
            virtualIRQn = TIM6_DAC1_IRQn;

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
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));
    EXPECT_CALL(*mock, IntrHandlerSet(INTR_TYPE_TIMER, _));

    TimerInit();

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, TimerTimeupFunction);
}

TEST_F(TimerTest, Add_sec)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));
    EXPECT_CALL(*mock, IntrHandlerSet(INTR_TYPE_TIMER, _));

    TimerInit();

    int timeout_sec;

    //範囲外
    timeout_sec = 81;
    EXPECT_EQ(-1, TimerAdd_sec(timeout_sec, dummy_function));

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)TimerTimeupFunction);

    //範囲内
    timeout_sec = 80;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));

    EXPECT_EQ(0, TimerAdd_sec(timeout_sec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(9999, virtualTIM->PSC);
    EXPECT_EQ(800 * timeout_sec, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)TimerTimeupFunction);
}

TEST_F(TimerTest, Add_msec)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));
    EXPECT_CALL(*mock, IntrHandlerSet(INTR_TYPE_TIMER, _));

    TimerInit();

    int timeout_msec;

    //範囲外
    timeout_msec = 8001;
    EXPECT_EQ(-1, TimerAdd_msec(timeout_msec, dummy_function));

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)TimerTimeupFunction);

    //範囲内
    timeout_msec = 8000;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));

    EXPECT_EQ(0, TimerAdd_msec(timeout_msec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(999, virtualTIM->PSC);
    EXPECT_EQ(8 * timeout_msec, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)TimerTimeupFunction);
}

TEST_F(TimerTest, Cancel)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));
    EXPECT_CALL(*mock, IntrHandlerSet(INTR_TYPE_TIMER, _));
    TimerInit();

    //Set msec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));
    TimerAdd_msec(8000, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    TimerCancel();

    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)TimerTimeupFunction);

    //Set sec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));
    TimerAdd_msec(80, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    TimerCancel();

    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)TimerTimeupFunction);
}
