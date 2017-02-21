cmake_minimum_required(VERSION 3.2)

set(SUBDIRECTORIES 
  common
  evergreen
  memory
  network
  southern-islands
  x86
)

foreach(subdir ${SUBDIRECTORIES} )
    add_subdirectory(${subdir})
endforeach(subdir)
