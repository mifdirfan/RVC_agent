if(EXISTS "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent.exe")
  if(NOT EXISTS "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_tests.cmake" OR
     NOT "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_tests.cmake" IS_NEWER_THAN "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent.exe" OR
     NOT "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_tests.cmake" IS_NEWER_THAN "${CMAKE_CURRENT_LIST_FILE}")
    include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_discovery.cmake")
  endif()
endif()
if(EXISTS "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_tests.cmake")
  include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_tests.cmake")
else()
  add_test(test_afferent_NOT_BUILT test_afferent_NOT_BUILT)
endif()
