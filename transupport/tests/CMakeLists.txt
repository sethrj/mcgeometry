srj_make_test(
  TESTS   tFastTimer tSuperTimer tVectorComp
  DEPENDS transupport
  SUBPROJECT transupport)
# variables from the macro go out of scope at the end of the file


##copy python runner
#install(FILES runTests.py
#        DESTINATION "bin")
#
##run python check when complete, if target is test
##add_custom_command(
