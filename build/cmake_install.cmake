# Install script for directory: /home/xiaoliang/APP/QR_locolization

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/xiaoliang/APP/QR_locolization/build/lib/libapriltags.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/AprilTags" TYPE FILE FILES
    "/home/xiaoliang/APP/QR_locolization/AprilTags/TagDetection.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/XYWeight.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Quad.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Edge.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/TagDetector.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag36h11_other.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag36h9.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Segment.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Gaussian.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/UnionFindSimple.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag36h11.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/TagFamily.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/GLineSegment2D.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag16h5_other.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag16h5.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/GrayModel.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/GLine2D.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/FloatImage.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/MathUtil.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Gridder.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Homography33.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag25h7.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/Tag25h9.h"
    "/home/xiaoliang/APP/QR_locolization/AprilTags/pch.h"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/xiaoliang/APP/QR_locolization/build/lib/pkgconfig/apriltags.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/xiaoliang/APP/QR_locolization/build/example/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/xiaoliang/APP/QR_locolization/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
