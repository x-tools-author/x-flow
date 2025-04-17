# argTargetDir

file(GLOB_RECURSE exe_files "${argTargetDir}/*.exe")
foreach(exe_file ${exe_files})
  file(REMOVE ${exe_file})
endforeach()

file(GLOB_RECURSE exe_files "${argTargetDir}/*.ilk")
foreach(exe_file ${exe_files})
  file(REMOVE ${exe_file})
endforeach()

file(GLOB_RECURSE exe_files "${argTargetDir}/*.pdb")
foreach(exe_file ${exe_files})
  file(REMOVE ${exe_file})
endforeach()
