set(WTF_LIBRARY_TYPE STATIC)

list(APPEND WTF_INCLUDE_DIRECTORIES
    "${WTF_DIR}/wtf/java"
    "${CMAKE_SOURCE_DIR}/Source"
    "${JAVA_INCLUDE_PATH}"
    "${JAVA_INCLUDE_PATH2}"
)

list(APPEND WTF_PUBLIC_HEADERS
    java/JavaEnv.h
    java/JavaRef.h
    java/DbgUtils.h
    java/JavaMath.h
    unicode/java/UnicodeJava.h
)

list(APPEND WTF_SOURCES
    java/FileSystemJava.cpp
    java/JavaEnv.cpp
    java/MainThreadJava.cpp
    java/StringJava.cpp
    java/TextBreakIteratorInternalICUJava.cpp
    java/CPUTimeJava.cpp
)

list(APPEND WTF_LIBRARIES
    "${JAVA_JVM_LIBRARY}"
)

list(APPEND WTF_SYSTEM_INCLUDE_DIRECTORIES
	  "${JDK_INCLUDE_DIRS}"
)

if (APPLE)
    file(COPY mac/MachExceptions.defs DESTINATION ${WTF_DERIVED_SOURCES_DIR})

    add_custom_command(
        OUTPUT
            ${WTF_DERIVED_SOURCES_DIR}/MachExceptionsServer.h
            ${WTF_DERIVED_SOURCES_DIR}/mach_exc.h
            ${WTF_DERIVED_SOURCES_DIR}/mach_excServer.c
            ${WTF_DERIVED_SOURCES_DIR}/mach_excUser.c
        MAIN_DEPENDENCY mac/MachExceptions.defs
        WORKING_DIRECTORY ${WTF_DERIVED_SOURCES_DIR}
        COMMAND mig -sheader MachExceptionsServer.h MachExceptions.defs
        VERBATIM)

    list(APPEND WTF_SOURCES
        ${WTF_DERIVED_SOURCES_DIR}/mach_excServer.c
        ${WTF_DERIVED_SOURCES_DIR}/mach_excUser.c
    )

    list(APPEND WTF_PUBLIC_HEADERS
        cf/TypeCastsCF.h
    )

    list(APPEND WTF_PRIVATE_INCLUDE_DIRECTORIES
        # Check whether we can use WTF/icu
        # "${WTF_DIR}/icu"
        ${WTF_DERIVED_SOURCES_DIR}
    )

    list(APPEND WTF_SOURCES
        BlockObjCExceptions.mm
        cf/LanguageCF.cpp
        cf/RunLoopCF.cpp
        cocoa/MachSendRight.cpp
        cocoa/MemoryFootprintCocoa.cpp
        cocoa/MemoryPressureHandlerCocoa.mm
        cocoa/WorkQueueCocoa.cpp
        text/cf/StringCF.cpp
        text/cf/StringImplCF.cpp
        text/cocoa/StringImplCocoa.mm
    )

    find_library(COCOA_LIBRARY Cocoa)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    list(APPEND WTF_LIBRARIES
        ${COREFOUNDATION_LIBRARY}
        ${COCOA_LIBRARY}
    )
elseif (UNIX)
    list(APPEND WTF_SOURCES
        generic/RunLoopGeneric.cpp
        generic/WorkQueueGeneric.cpp
        linux/CurrentProcessMemoryStatus.cpp
        linux/MemoryFootprintLinux.cpp
        unix/LanguageUnix.cpp
        unix/MemoryPressureHandlerUnix.cpp
        linux/RealTimeThreads.cpp
    )
    list(APPEND WTF_LIBRARIES rt)
elseif (WIN32)
    list(APPEND WTF_SOURCES
        generic/WorkQueueGeneric.cpp

        win/CPUTimeWin.cpp
        win/DbgHelperWin.cpp
        win/LanguageWin.cpp
        win/MemoryFootprintWin.cpp
        win/MemoryPressureHandlerWin.cpp
        win/OSAllocatorWin.cpp
        win/RunLoopWin.cpp
        win/ThreadingWin.cpp
    )

    list(APPEND WTF_PUBLIC_HEADERS
        text/win/WCharStringExtras.h

        win/DbgHelperWin.h
        win/Win32Handle.h
    )

    list(APPEND WTF_LIBRARIES
        DbgHelp
        winmm
    )
endif ()

if (UNIX)
    list(APPEND WTF_SOURCES
        posix/OSAllocatorPOSIX.cpp
        posix/ThreadingPOSIX.cpp
    )
endif ()

if (DEFINED CMAKE_USE_PTHREADS_INIT)
    list(APPEND WTF_LIBRARIES pthread)
endif()
