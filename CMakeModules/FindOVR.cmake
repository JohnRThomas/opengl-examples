include(FindPackageHandleStandardArgs)

# Rekhi Linux lab
#set(CMAKE_LIBRARY_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.4.4/LibOVR/Lib/Linux/Release/x86_64" ${CMAKE_LIBRARY_PATH})
#set(CMAKE_INCLUDE_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.4.4/LibOVR/Include" ${CMAKE_INCLUDE_PATH})
#set(CMAKE_INCLUDE_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.4.4/LibOVR/Src" ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Debug" ${CMAKE_LIBRARY_PATH})
set(CMAKE_LIBRARY_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Release" ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Include" ${CMAKE_INCLUDE_PATH})
set(CMAKE_INCLUDE_PATH "/home/campus11/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Src" ${CMAKE_INCLUDE_PATH})

# Kuhl's machines
set(CMAKE_LIBRARY_PATH "/home/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Debug" ${CMAKE_LIBRARY_PATH})
set(CMAKE_LIBRARY_PATH "/home/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Release" ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH "/home/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Include" ${CMAKE_INCLUDE_PATH})
set(CMAKE_INCLUDE_PATH "/home/kuhl/public-ogl/ovr_sdk_linux_0.5.0.1/LibOVR/Src" ${CMAKE_INCLUDE_PATH})


# Allow folder to be stored alongside the opengl-examples folder.
#set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.4.4/LibOVR/Lib/Linux/Release/x86_64" ${CMAKE_LIBRARY_PATH})
#set(CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.4.4/LibOVR/Include" ${CMAKE_INCLUDE_PATH})
#set(CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.4.4/LibOVR/Src" ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Debug" ${CMAKE_LIBRARY_PATH})
set(CMAKE_LIBRARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.5.0.1/LibOVR/Lib/Linux/x86_64/Release" ${CMAKE_LIBRARY_PATH})
set(CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.5.0.1/LibOVR/Include" ${CMAKE_INCLUDE_PATH})
set(CMAKE_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../ovr_sdk_linux_0.5.0.1/LibOVR/Src" ${CMAKE_INCLUDE_PATH})



# library was named libovr.a in 0.4.4 and libOVR.a in 0.5.0.1
find_library(OVR_LIBRARIES libOVR.a libovr.a)
find_library(OVR_LIBRARY_SO libOVRRT64_0.so.5)
find_path(OVR_INCLUDE_DIR_LIB OVR.h)
find_path(OVR_INCLUDE_DIR_SRC OVR_CAPI_GL.h)

# find_library(OVR_udev udev)
find_library(OVR_pthread pthread)
find_library(OVR_gl GL)
find_library(OVR_x11 X11)
find_library(OVR_xrandr Xrandr)
find_library(OVR_rt rt)


find_package_handle_standard_args(OVR DEFAULT_MSG
	OVR_LIBRARIES
	OVR_INCLUDE_DIR_LIB
	OVR_INCLUDE_DIR_SRC
	OVR_LIBRARY_SO
	OVR_pthread
	OVR_gl
	OVR_x11
	OVR_xrandr
	OVR_rt)

# TODO: Change PATH to DIRECTORY below when Rekhi lab uses a version of cmake newer than 2.8.11
get_filename_component(OVR_LIBRARY_DIR ${OVR_LIBRARY_SO} PATH)

# append the two include directories together.
set(OVR_INCLUDE_DIRS ${OVR_INCLUDE_DIR_LIB} ${OVR_INCLUDE_DIR_SRC})
set(OVR_LIBRARIES ${OVR_LIBRARIES} -Wl,-rpath=${OVR_LIBRARY_DIR} ${OVR_LIBRARY_SO} ${OVR_pthread} ${OVR_gl} ${OVR_x11} ${OVR_xrandr} ${OVR_rt})
