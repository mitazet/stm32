// テストケース記述ファイル
#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "timer0_driver.h"
#include "timer1_driver.h"
#include "stm32f303x8.h"

using ::testing::_;

class Mock{
    public:
        MOCK_METHOD1(NVIC_EnableIRQ, void (IRQn_Type));
        MOCK_METHOD1(NVIC_DisableIRQ, void (IRQn_Type));
};

Mock *mock;

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

    void dummy_function(void){
    }
}

RCC_TypeDef *virtualRCC[TIMER_NUM];
TIM_TypeDef *virtualTIM[TIMER_NUM];
uint32_t enable_bit[TIMER_NUM] = {RCC_APB1ENR_TIM6EN, RCC_APB1ENR_TIM7EN};
IRQn_Type virtualIRQn[TIMER_NUM] = {TIM6_DAC1_IRQn, TIM7_DAC2_IRQn};
TimerBase base[TIMER_NUM];
Timer0Driver& Timer0 = Timer0Driver::GetInstance();
Timer1Driver& Timer1 = Timer1Driver::GetInstance();

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class Timer0Test : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new Mock();

            for(int i=0; i<TIMER_NUM; i++){
                virtualRCC[i] = new RCC_TypeDef();
                virtualTIM[i] = new TIM_TypeDef();
                base[i] = {virtualRCC[i], virtualTIM[i], enable_bit[i], virtualIRQn[i]};
            }

            Timer0.SetBase(base[TIMER_0]);
            Timer1.SetBase(base[TIMER_1]);
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualRCC[TIMER_0];
            delete virtualRCC[TIMER_1];
            delete virtualTIM[TIMER_0];
            delete virtualTIM[TIMER_1];
        }
};

// テストケース
TEST_F(Timer0Test, Init)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Init();

    EXPECT_EQ(0, virtualRCC[id]->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM[id]->PSC);
    EXPECT_EQ(0, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Start_sec_NG)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Init();

    int timeout_sec;

    //範囲外
    timeout_sec = 81;
    EXPECT_EQ(-1, Timer0.Start_sec(timeout_sec, dummy_function));

    EXPECT_EQ(0, virtualRCC[id]->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM[id]->PSC);
    EXPECT_EQ(0, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Start_sec_OK)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Init();

    int timeout_sec;

    //範囲内
    timeout_sec = 80;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn[id]));

    EXPECT_EQ(0, Timer0.Start_sec(timeout_sec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC[id]->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(9999, virtualTIM[id]->PSC);
    EXPECT_EQ(800 * timeout_sec, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Start_msec_NG)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Init();

    int timeout_msec;

    //範囲外
    timeout_msec = 8001;
    EXPECT_EQ(-1, Timer0.Start_msec(timeout_msec, dummy_function));

    EXPECT_EQ(0, virtualRCC[id]->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM[id]->PSC);
    EXPECT_EQ(0, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Start_msec_OK)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Init();

    int timeout_msec;

    //範囲内
    timeout_msec = 8000;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn[id]));

    EXPECT_EQ(0, Timer0.Start_msec(timeout_msec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC[id]->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(999, virtualTIM[id]->PSC);
    EXPECT_EQ(8 * timeout_msec, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Cancel_msec)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));
    Timer0.Init();

    //Set msec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn[id]));
    Timer0.Start_msec(8000, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Cancel();

    EXPECT_EQ(0, virtualTIM[id]->PSC);
    EXPECT_EQ(0, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[id]);
}

TEST_F(Timer0Test, Cancel_sec)
{
    TimerId id = TIMER_0;

    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));
    Timer0.Init();

    //Set sec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn[id]));
    Timer0.Start_msec(80, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn[id]));

    Timer0.Cancel();

    EXPECT_EQ(0, virtualTIM[id]->PSC);
    EXPECT_EQ(0, virtualTIM[id]->ARR);
    EXPECT_EQ(0, virtualTIM[id]->CNT);
    EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[id]);
}
