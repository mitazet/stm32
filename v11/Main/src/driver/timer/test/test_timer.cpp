// テストケース記述ファイル
#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "timer_driver.h"
#include "stm32f303x8.h"

using ::testing::_;

class Mock{
    public:
        MOCK_METHOD1(NVIC_EnableIRQ, void (IRQn_Type));
        MOCK_METHOD1(NVIC_DisableIRQ, void (IRQn_Type));
};

using ::testing::NiceMock;

NiceMock<Mock> *mock;

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

extern TimerBase timer_def[TIMER_NUM];

uint32_t  clk_en[TIMER_NUM] = {RCC_APB1ENR_TIM6EN, RCC_APB1ENR_TIM7EN};
IRQn_Type IRQn[TIMER_NUM]   = {TIM6_DAC1_IRQn, TIM7_DAC2_IRQn};

// fixtureNameはテストケース群をまとめるグループ名と考えればよい、任意の文字列
// それ以外のclass～testing::Testまではおまじないと考える
class TimerTest : public ::testing::Test {
    public:
        TimerDriver& TimerDrv = TimerDriver::GetInstance();
        RCC_TypeDef *virtualRCC;
        TIM_TypeDef *virtualTIM[TIMER_NUM];

    protected:
        // fixtureNameでグループ化されたテストケースはそれぞれのテストケース実行前に
        // この関数を呼ぶ。共通の初期化処理を入れておくとテストコードがすっきりする
        virtual void SetUp()
        {
            mock = new NiceMock<Mock>();
            virtualRCC = new RCC_TypeDef();

            for(int i=0; i<TIMER_NUM; i++){
                virtualTIM[i] = new TIM_TypeDef();
                timer_def[i] = {virtualRCC, virtualTIM[i], clk_en[i], IRQn[i]};
            }
        }
        // SetUpと同様にテストケース実行後に呼ばれる関数。共通後始末を記述する。
        virtual void TearDown()
        {
            delete mock;
            delete virtualRCC;
            for(int i=0; i<TIMER_NUM; i++){
                delete virtualTIM[i];
            }
        }
};

// テストケース
TEST_F(TimerTest, Init)
{
    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_CALL(*mock, NVIC_DisableIRQ(IRQn[id]));
    }

    TimerDrv.Init();

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_EQ(0, virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, Add_sec_NG_time)
{
    TimerDrv.Init();

    //範囲外
    int timeout_sec = 81;
    for(int id = 0; id < TIMER_NUM; id++){

        EXPECT_EQ(TIMER_OUT_OF_RANGE, TimerDrv.Add_sec((TimerId)id, timeout_sec, dummy_function));

        EXPECT_EQ(0, virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, Add_sec_NG_id)
{
    TimerDrv.Init();

    //範囲内
    int timeout_sec = 80;

    //範囲外
    EXPECT_EQ(TIMER_ILLEGAL_ID, TimerDrv.Add_sec(TIMER_NUM, timeout_sec, dummy_function));

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_EQ(0, virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, Add_sec_OK)
{
    TimerDrv.Init();

    //範囲内
    int timeout_sec = 80;

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));

        EXPECT_EQ(TIMER_OK, TimerDrv.Add_sec((TimerId)id, timeout_sec, dummy_function));

        EXPECT_EQ(clk_en[id], virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(9999, virtualTIM[id]->PSC);
        EXPECT_EQ(800 * timeout_sec, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ((void*)dummy_function, (void*)TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, Add_msec_NG_time)
{
    TimerDrv.Init();

    //範囲外
    int timeout_msec = 8001;
    for(int id = 0; id < TIMER_NUM; id++){

        EXPECT_EQ(TIMER_OUT_OF_RANGE, TimerDrv.Add_msec((TimerId)id, timeout_msec, dummy_function));

        EXPECT_EQ(0, virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, Add_msec_NG_id)
{
    TimerDrv.Init();

    //範囲内
    int timeout_sec = 8000;

    //範囲外
    EXPECT_EQ(TIMER_ILLEGAL_ID, TimerDrv.Add_msec(TIMER_NUM, timeout_sec, dummy_function));

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_EQ(0, virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, TimerDriver::TimeupFunction[id]);
    }
}
TEST_F(TimerTest, Add_msec_OK)
{
    TimerDrv.Init();

    //範囲内
    int timeout_msec = 8000;

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));

        EXPECT_EQ(TIMER_OK, TimerDrv.Add_msec((TimerId)id, timeout_msec, dummy_function));

        EXPECT_EQ(clk_en[id], virtualRCC->APB1ENR & clk_en[id]);
        EXPECT_EQ(999, virtualTIM[id]->PSC);
        EXPECT_EQ(8 * timeout_msec, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ((void*)dummy_function, (void*)TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, delete_msec_NG)
{
    TimerDrv.Init();

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        //Set sec
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));

        int timeout_msec = 8000; 
        TimerDrv.Add_msec((TimerId)id, timeout_msec, dummy_function);

        //Cancel
        EXPECT_EQ(TIMER_ILLEGAL_ID, TimerDrv.Delete(TIMER_NUM));

        EXPECT_EQ(999, virtualTIM[id]->PSC);
        EXPECT_EQ(8 * timeout_msec, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ((void*)dummy_function, (void*)TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, delete_msec_OK)
{
    TimerDrv.Init();

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        //Set msec
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));
        TimerDrv.Add_msec((TimerId)id, 8000, dummy_function);

        //Cancel
        EXPECT_CALL(*mock, NVIC_DisableIRQ(IRQn[id]));

        EXPECT_EQ(TIMER_OK, TimerDrv.Delete((TimerId)id));

        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, (void*)TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, delete_sec_NG)
{
    TimerDrv.Init();

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        //Set sec
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));

        int timeout_sec = 80; 
        TimerDrv.Add_sec((TimerId)id, timeout_sec, dummy_function);

        //Cancel
        EXPECT_EQ(TIMER_ILLEGAL_ID, TimerDrv.Delete(TIMER_NUM));

        EXPECT_EQ(9999, virtualTIM[id]->PSC);
        EXPECT_EQ(800 * timeout_sec, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(TIM_CR1_CEN, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(TIM_DIER_UIE, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ((void*)dummy_function, (void*)TimerDriver::TimeupFunction[id]);
    }
}

TEST_F(TimerTest, delete_sec_OK)
{
    TimerDrv.Init();

    for(uint32_t id = 0; id < TIMER_NUM; id++){
        //Set msec
        EXPECT_CALL(*mock, NVIC_EnableIRQ(IRQn[id]));
        TimerDrv.Add_sec((TimerId)id, 80, dummy_function);

        //Cancel
        EXPECT_CALL(*mock, NVIC_DisableIRQ(IRQn[id]));

        EXPECT_EQ(TIMER_OK, TimerDrv.Delete((TimerId)id));

        EXPECT_EQ(0, virtualTIM[id]->PSC);
        EXPECT_EQ(0, virtualTIM[id]->ARR);
        EXPECT_EQ(0, virtualTIM[id]->CNT);
        EXPECT_EQ(0, virtualTIM[id]->CR1 & TIM_CR1_CEN);
        EXPECT_EQ(0, virtualTIM[id]->DIER & TIM_DIER_UIE);
        EXPECT_EQ(NULL, (void*)TimerDriver::TimeupFunction[id]);
    }
}
