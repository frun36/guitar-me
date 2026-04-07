#include "ring_buffer.h"
#include "unity.h"

#define CAPACITY 8

static int s_buff[CAPACITY];

static RingBuffer_t s_rb;

void setUp(void) {
    RingBuffer_Init(&s_rb, s_buff, CAPACITY, sizeof(int));
}

void tearDown(void) {}

static void Helper_FillBuffer(size_t count) {
    for (size_t i = 0; i < count; i++) {
        int val = (int)i;
        RingBuffer_Push(&s_rb, &val);
    }
}

static void Helper_ClearBuffer(size_t count) {
    int dummy = -1;
    for (size_t i = 0; i < count; i++) {
        RingBuffer_Pop(&s_rb, &dummy);
    }
}

static void Helper_FillClearBuffer(size_t count) {
    Helper_FillBuffer(count);
    Helper_ClearBuffer(count);
}

void test_RingBuffer_AtInitialization_IsEmpty(void) {
    TEST_ASSERT_TRUE(RingBuffer_IsEmpty(&s_rb));
}

void test_RingBuffer_FirstPush_Succeeds(void) {
    int val = 0;

    bool push_res = RingBuffer_Push(&s_rb, &val);

    TEST_ASSERT_TRUE(push_res);
}

void test_RingBuffer_PushWithinCapacityNoWrap_Succeeds(void) {
    int val = 0;
    RingBuffer_Push(&s_rb, &val);
    val++;

    bool push_res = RingBuffer_Push(&s_rb, &val);

    TEST_ASSERT_TRUE(push_res);
}

void test_RingBuffer_PartiallyFilled_NotEmpty(void) {
    Helper_FillBuffer(3);

    bool is_empty = RingBuffer_IsEmpty(&s_rb);

    TEST_ASSERT_FALSE(is_empty);
}

void test_RingBuffer_PartiallyFilled_NotFull(void) {
    Helper_FillBuffer(3);

    bool is_full = RingBuffer_IsFull(&s_rb);

    TEST_ASSERT_FALSE(is_full);
}

void test_RingBuffer_Full_NotEmpty(void) {
    Helper_FillBuffer(CAPACITY - 1);

    bool is_empty = RingBuffer_IsEmpty(&s_rb);

    TEST_ASSERT_FALSE(is_empty);
}

void test_RingBuffer_Full_IsFull(void) {
    Helper_FillBuffer(CAPACITY - 1);

    bool is_full = RingBuffer_IsFull(&s_rb);

    TEST_ASSERT_TRUE(is_full);
}

void test_RingBuffer_Full_PushFails(void) {
    Helper_FillBuffer(CAPACITY - 1);
    int val = 99;

    bool push_res = RingBuffer_Push(&s_rb, &val);

    TEST_ASSERT_FALSE(push_res);
}

void test_RingBuffer_EmptyBuffer_PopFails(void) {
    int val = -1;

    bool pop_res = RingBuffer_Pop(&s_rb, &val);

    TEST_ASSERT_FALSE(pop_res);
    TEST_ASSERT_EQUAL_INT(-1, val);
}

void test_RingBuffer_PartiallyFilled_PopReturnsCorrectValues(void) {
    int pushed_values[3] = {11, 12, 13};
    RingBuffer_Push(&s_rb, pushed_values);
    RingBuffer_Push(&s_rb, pushed_values + 1);
    RingBuffer_Push(&s_rb, pushed_values + 2);

    int popped_values[3] = {-1, -1, -1};
    bool pop_res_1 = RingBuffer_Pop(&s_rb, popped_values);
    bool pop_res_2 = RingBuffer_Pop(&s_rb, popped_values + 1);
    bool pop_res_3 = RingBuffer_Pop(&s_rb, popped_values + 2);

    TEST_ASSERT_TRUE(pop_res_1 && pop_res_2 && pop_res_3);
    TEST_ASSERT_EQUAL_INT_ARRAY(pushed_values, popped_values, 3);
}

void test_RingBuffer_PartiallyFilled_PopMakesEmpty(void) {
    Helper_FillBuffer(3);

    int dummy = -1;
    bool pop_res_1 = RingBuffer_Pop(&s_rb, &dummy);
    bool pop_res_2 = RingBuffer_Pop(&s_rb, &dummy);
    bool pop_res_3 = RingBuffer_Pop(&s_rb, &dummy);
    bool is_empty = RingBuffer_IsEmpty(&s_rb);

    TEST_ASSERT_TRUE_MESSAGE(
        pop_res_1 && pop_res_2 && pop_res_3,
        "Pop failed unexpectedly"
    );
    TEST_ASSERT_TRUE(is_empty);
}

void test_RingBuffer_PushAfterWraparound_Succeeds(void) {
    Helper_FillClearBuffer(CAPACITY - 1);
    int val = 36;

    bool push_res = RingBuffer_Push(&s_rb, &val);

    TEST_ASSERT_TRUE(push_res);
}

void test_RingBuffer_PushAfterWraparound_NotEmpty(void) {
    Helper_FillClearBuffer(CAPACITY - 1);
    int val = 36;

    bool push_res = RingBuffer_Push(&s_rb, &val);
    bool is_empty = RingBuffer_IsEmpty(&s_rb);

    TEST_ASSERT_TRUE_MESSAGE(push_res, "Push failed unexpectedly");
    TEST_ASSERT_FALSE(is_empty);
}

void test_RingBuffer_Wraparound_PopReturnsCorrectValues(void) {
    Helper_FillClearBuffer(CAPACITY - 2);
    int pushed_values[4] = {11, 12, 13, 14};
    RingBuffer_Push(&s_rb, pushed_values);
    RingBuffer_Push(&s_rb, pushed_values + 1);
    RingBuffer_Push(&s_rb, pushed_values + 2);
    RingBuffer_Push(&s_rb, pushed_values + 3);

    int popped_values[4] = {-1, -1, -1, -1};
    bool pop_res_1 = RingBuffer_Pop(&s_rb, popped_values);
    bool pop_res_2 = RingBuffer_Pop(&s_rb, popped_values + 1);
    bool pop_res_3 = RingBuffer_Pop(&s_rb, popped_values + 2);
    bool pop_res_4 = RingBuffer_Pop(&s_rb, popped_values + 3);

    TEST_ASSERT_TRUE(pop_res_1 && pop_res_2 && pop_res_3 && pop_res_4);
    TEST_ASSERT_EQUAL_INT_ARRAY(pushed_values, popped_values, 4);
}

void test_RingBuffer_Wraparound_PopMakesEmpty(void) {
    Helper_FillClearBuffer(CAPACITY - 2);
    Helper_FillBuffer(3);

    int dummy = -1;
    bool pop_res_1 = RingBuffer_Pop(&s_rb, &dummy);
    bool pop_res_2 = RingBuffer_Pop(&s_rb, &dummy);
    bool pop_res_3 = RingBuffer_Pop(&s_rb, &dummy);
    bool is_empty = RingBuffer_IsEmpty(&s_rb);

    TEST_ASSERT_TRUE_MESSAGE(
        pop_res_1 && pop_res_2 && pop_res_3,
        "Pop failed unexpectedly"
    );
    TEST_ASSERT_TRUE(is_empty);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_RingBuffer_AtInitialization_IsEmpty);
    RUN_TEST(test_RingBuffer_FirstPush_Succeeds);
    RUN_TEST(test_RingBuffer_PushWithinCapacityNoWrap_Succeeds);
    RUN_TEST(test_RingBuffer_PartiallyFilled_NotEmpty);
    RUN_TEST(test_RingBuffer_PartiallyFilled_NotFull);
    RUN_TEST(test_RingBuffer_Full_NotEmpty);
    RUN_TEST(test_RingBuffer_Full_IsFull);
    RUN_TEST(test_RingBuffer_Full_PushFails);
    RUN_TEST(test_RingBuffer_EmptyBuffer_PopFails);
    RUN_TEST(test_RingBuffer_PartiallyFilled_PopReturnsCorrectValues);
    RUN_TEST(test_RingBuffer_PartiallyFilled_PopMakesEmpty);
    RUN_TEST(test_RingBuffer_PushAfterWraparound_Succeeds);
    RUN_TEST(test_RingBuffer_PushAfterWraparound_NotEmpty);
    RUN_TEST(test_RingBuffer_Wraparound_PopReturnsCorrectValues);
    RUN_TEST(test_RingBuffer_Wraparound_PopMakesEmpty);
    return UNITY_END();
}
