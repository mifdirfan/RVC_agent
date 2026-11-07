# CMake generated Testfile for 
# Source directory: C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude
# Build directory: C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_afferent_74785a0e_include.cmake")
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_interfaces_74785a0e_include.cmake")
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_actions_74785a0e_include.cmake")
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_avoidance_74785a0e_include.cmake")
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_controller_74785a0e_include.cmake")
include("C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/build/test_system_74785a0e_include.cmake")
add_test("CallHierarchy.CH01_MatchesStructureChart" "C:/Users/mifdz/AppData/Local/Programs/Python/Python314/python.exe" "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/tests/check_call_hierarchy.py" "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/src")
set_tests_properties("CallHierarchy.CH01_MatchesStructureChart" PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/CMakeLists.txt;114;add_test;C:/Users/mifdz/Downloads/261106_RVC_SASD_Claude/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
