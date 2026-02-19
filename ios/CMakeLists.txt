cmake_minimum_required(VERSION 3.20)
project(zomdroid_ios LANGUAGES C CXX OBJC OBJCXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_GENERATOR STREQUAL "Xcode")
    # CI builds unsigned artifacts (.app/.ipa/.tipa), so disable signing for all generated targets.
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
endif()

option(ZOMDROID_IOS_ENABLE_EMULATION "Build in-process box64 linker backend (jailbreak/JIT path)" ON)
option(ZOMDROID_IOS_BUILD_APP "Build iOS app bundle shell" ON)

set(ZOMDROID_CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/main/cpp)

add_library(zomdroid_ios_core STATIC
    Sources/zomdroid_core_api_stub.c
    Sources/ZomdroidIOSBridge.mm
    ${ZOMDROID_CPP_DIR}/zomdroid.c
)

target_include_directories(zomdroid_ios_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ZOMDROID_CPP_DIR}
)

target_link_libraries(zomdroid_ios_core
    "-framework Foundation"
    "-framework UIKit"
    "-framework CoreGraphics"
    "-framework QuartzCore"
)

if (ZOMDROID_IOS_ENABLE_EMULATION)
    set(NOGIT ON)
    set(ARM_DYNAREC ON)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-D_XOPEN_SOURCE=700>)
    if (APPLE)
        # box64 expects GNU extensions that map to Apple private/libSystem equivalents.
        add_compile_definitions(
            sincos=__sincos
            sincosf=__sincosf
            isnanf=isnan
            isinff=isinf
            alloca=__builtin_alloca
        )
    endif()
    add_subdirectory(${ZOMDROID_CPP_DIR}/box64 ${CMAKE_CURRENT_BINARY_DIR}/box64)
    target_include_directories(box64 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_definitions(box64 PRIVATE BUILD_DYNAMIC)

    add_library(zomdroid_ios_linker STATIC
        ${ZOMDROID_CPP_DIR}/linker.c
        ${ZOMDROID_CPP_DIR}/emulation.c
        ${ZOMDROID_CPP_DIR}/wrapped_jni.c
    )

    target_include_directories(zomdroid_ios_linker
        PUBLIC
        ${ZOMDROID_CPP_DIR}
    )

    target_link_libraries(zomdroid_ios_linker
        box64
        zomdroid_ios_core
    )
endif()

if (ZOMDROID_IOS_BUILD_APP)
    set(APP_SOURCES
        App/main.m
        App/ZomdroidAppDelegate.m
        App/ZomdroidGameViewController.mm
    )

    if (ZOMDROID_IOS_ENABLE_EMULATION)
        list(APPEND APP_SOURCES Sources/ZomdroidDyldInterpose.c)
    endif()

    set_source_files_properties(
        Sources/ZomdroidIOSBridge.mm
        App/main.m
        App/ZomdroidAppDelegate.m
        App/ZomdroidGameViewController.mm
        PROPERTIES COMPILE_FLAGS "-fobjc-arc"
    )

    add_executable(ZomdroidIOSApp MACOSX_BUNDLE ${APP_SOURCES})

    target_include_directories(ZomdroidIOSApp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ZOMDROID_CPP_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/App
    )

    target_link_libraries(ZomdroidIOSApp
        zomdroid_ios_core
        "-framework Foundation"
        "-framework UIKit"
        "-framework CoreGraphics"
        "-framework QuartzCore"
        "-framework GameController"
    )

    if (ZOMDROID_IOS_ENABLE_EMULATION)
        target_link_libraries(ZomdroidIOSApp zomdroid_ios_linker)
    endif()

    set_target_properties(ZomdroidIOSApp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/App/Info.plist
        XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO"
    )
endif()
