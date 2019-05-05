#include "test.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"

void test_dice(void) {

    int * dice1;
    int * dice2; 
    int * dice3; 
    int * dice4; 

    int d1[2] = {1,3};
    int d2[2] = {4,4};
    int d3[2] = {6,5};
    int d4[2] = {3,5};

    dice1 = d1;
    dice2 = d2;
    dice3 = d3;
    dice4 = d4;

    bool b1 = check_even(dice1);
    bool b2 = check_even(dice2);
    bool b3 = check_even(dice3);
    bool b4 = check_even(dice4);

    CU_ASSERT_TRUE(b1);
    CU_ASSERT_TRUE(b2);
    CU_ASSERT_TRUE(b3);
    CU_ASSERT_TRUE(b4);

}