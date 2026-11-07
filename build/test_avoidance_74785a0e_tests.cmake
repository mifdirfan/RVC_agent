add_test([=[Avoidance.AV01_BothFreeTurnsLeft]=]  C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_avoidance.exe [==[--gtest_filter=Avoidance.AV01_BothFreeTurnsLeft]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Avoidance.AV01_BothFreeTurnsLeft]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\mifdz\Downloads\261106_RVC_SASD_Claude\tests\test_avoidance.cpp:9]==]
    WORKING_DIRECTORY [==[C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[Avoidance.AV02_LeftFreeRightBlockedTurnsLeft]=]  C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_avoidance.exe [==[--gtest_filter=Avoidance.AV02_LeftFreeRightBlockedTurnsLeft]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Avoidance.AV02_LeftFreeRightBlockedTurnsLeft]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\mifdz\Downloads\261106_RVC_SASD_Claude\tests\test_avoidance.cpp:10]==]
    WORKING_DIRECTORY [==[C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[Avoidance.AV03_LeftBlockedRightFreeTurnsRight]=]  C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_avoidance.exe [==[--gtest_filter=Avoidance.AV03_LeftBlockedRightFreeTurnsRight]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Avoidance.AV03_LeftBlockedRightFreeTurnsRight]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\mifdz\Downloads\261106_RVC_SASD_Claude\tests\test_avoidance.cpp:11]==]
    WORKING_DIRECTORY [==[C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[Avoidance.AV04_BothBlockedBackward]=]  C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_avoidance.exe [==[--gtest_filter=Avoidance.AV04_BothBlockedBackward]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[Avoidance.AV04_BothBlockedBackward]=]
  PROPERTIES
    
    DEF_SOURCE_LINE [==[C:\Users\mifdz\Downloads\261106_RVC_SASD_Claude\tests\test_avoidance.cpp:12]==]
    WORKING_DIRECTORY [==[C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
set(test_avoidance_TESTS [==[Avoidance.AV01_BothFreeTurnsLeft]==] [==[Avoidance.AV02_LeftFreeRightBlockedTurnsLeft]==] [==[Avoidance.AV03_LeftBlockedRightFreeTurnsRight]==] [==[Avoidance.AV04_BothBlockedBackward]==])
