#include "test.h"
#include "CUnit/Basic.h"
#include "CUnit/CUnit.h"


int init_suite(void){
    return 0;
}

int clean_suite(void){
    return 0;
}

int main(void) {

    //Initialize Cunit test registry
    CU_pSuite pSuite1 = NULL;
    if(CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    pSuite1 = CU_add_suite("Dice testing", init_suite, clean_suite);
    
    if(pSuite1 == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }


    if(NULL == CU_add_test(pSuite1, "Testing dice rolls", test_dice)){
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests(); //OUTPUT to the screen

	CU_cleanup_registry(); //Cleaning the Registry
    return CU_get_error();
}