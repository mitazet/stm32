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
#if 0
RCC_TypeDef *virtualRCC[TIMER_NUM];
TIM_TypeDef *virtualTIM[TIMER_NUM];
uint32 enable_bit[TIMER_NUM];
IRQn_Type virtualIRQn[TIMER_NUM];
TimerBase base[TIMER_NUM];
Timer0Driver *Timer0;
Timer1Driver *Timer1;
#else
RCC_TypeDef *virtualRCC;
TIM_TypeDef *virtualTIM;
uint32_t enable_bit;
IRQn_Type virtualIRQn;
Timer0Driver& Timer0 = Timer0Driver::GetInstance();
#endif

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class TimerTest : public ::testing::Test {
    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new Mock();
#if 0
            for(int i=0; i<TIMER_NUM; i++){
                virtualRCC[i] = new RCC_TypeDef();
                virtualTIM[i] = new TIM_TypeDef();
                if(i==0){
                    enable_bit[i] = RCC_APB1ENR_TIM6EN;
                    virtualIRQn[i] = TIM6_DAC1_IRQn;
                }else{
                    enable_bit[i] = RCC_APB1ENR_TIM7EN;
                    virtualIRQn[i] = TIM7_DAC2_IRQn;
                }
                base[i] = {virtualRCC[i], virtualTIM[i], enable_bit[i], virtualIRQn[i]};
            }

            Timer0 = new Timer0Driver();
            Timer0->SetBase(base[0]);

            Timer1 = new Timer1Driver();
            Timer1->SetBase(base[1]);
#else
            virtualRCC = new RCC_TypeDef();
            virtualTIM = new TIM_TypeDef();
            enable_bit = RCC_APB1ENR_TIM6EN;
            virtualIRQn = TIM6_DAC1_IRQn;
            TimerBase base = {virtualRCC, virtualTIM, enable_bit, virtualIRQn};
            Timer0.SetBase(base);
#endif
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualRCC;
            delete virtualTIM;
        }
};

// テストケース
TEST_F(TimerTest, Init)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    Timer0.Init();

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, Timer0Driver::TimeupFunction[TIMER_0]);
}

TEST_F(TimerTest, Start_sec)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    Timer0.Init();

    int timeout_sec;

    //範囲外
    timeout_sec = 81;
    EXPECT_EQ(-1, Timer0.Start_sec(timeout_sec, dummy_function));

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, Timer0Driver::TimeupFunction[TIMER_0]);

    //範囲内
    timeout_sec = 80;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));

    EXPECT_EQ(0, Timer0.Start_sec(timeout_sec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(9999, virtualTIM->PSC);
    EXPECT_EQ(800 * timeout_sec, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)Timer0Driver::TimeupFunction[TIMER_0]);
}

TEST_F(TimerTest, Start_msec)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    Timer0.Init();

    int timeout_msec;

    //範囲外
    timeout_msec = 8001;
    EXPECT_EQ(-1, Timer0.Start_msec(timeout_msec, dummy_function));

    EXPECT_EQ(0, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[TIMER_0]);

    //範囲内
    timeout_msec = 8000;
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));

    EXPECT_EQ(0, Timer0.Start_msec(timeout_msec, dummy_function));

    EXPECT_EQ(RCC_APB1ENR_TIM6EN, virtualRCC->APB1ENR & RCC_APB1ENR_TIM6EN);
    EXPECT_EQ(999, virtualTIM->PSC);
    EXPECT_EQ(8 * timeout_msec, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(TIM_CR1_CEN, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(TIM_DIER_UIE, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ((void*)dummy_function, (void*)Timer0Driver::TimeupFunction[TIMER_0]);
}

TEST_F(TimerTest, Cancel)
{
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));
    Timer0.Init();

    //Set msec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));
    Timer0.Start_msec(8000, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    Timer0.Cancel();

    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[TIMER_0]);

    //Set sec
    EXPECT_CALL(*mock, NVIC_EnableIRQ(virtualIRQn));
    Timer0.Start_msec(80, dummy_function);

    //Cancel
    EXPECT_CALL(*mock, NVIC_DisableIRQ(virtualIRQn));

    Timer0.Cancel();

    EXPECT_EQ(0, virtualTIM->PSC);
    EXPECT_EQ(0, virtualTIM->ARR);
    EXPECT_EQ(0, virtualTIM->CNT);
    EXPECT_EQ(0, virtualTIM->CR1 & TIM_CR1_CEN);
    EXPECT_EQ(0, virtualTIM->DIER & TIM_DIER_UIE);
    EXPECT_EQ(NULL, (void*)Timer0Driver::TimeupFunction[TIMER_0]);
}
