import qbs

CppApplication {
    name:"proj"

    cpp.cxxLanguageVersion: "c++14"

    cpp.includePaths: [
        "src/shared",
        "src/capture",
        "src/egl",
        "src/mavlink",
        "src/scene",
        "src/json",
        qbs.sysroot+"/usr/include/eigen3",
        qbs.sysroot+"/usr/include",
        qbs.sysroot+"/usr/local/include",
    ]

    cpp.defines:[
        //"GLM_LEFT_HANDED"
    ]

    Group {
        name: "shared"
        prefix: "src/shared/"
        files:[
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "capture"
        prefix: "src/capture/"
        files:[
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "egl"
        prefix: "src/egl/"
        files:[
            "*.cpp",
            "*.h"
        ]
        excludeFiles: qbs.architecture.contains("arm") ? ["glut*"] : ["egl*"]
    }

    Group {
        name: "scene"
        prefix: "src/scene/"
        files:[
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "main"
        prefix: "src/*"
        files: [
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "io"
        prefix: "src/io/**/**"
        files:[
            "*.c",
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "telemetry"
        prefix: "src/mavlink/**/**"
        files:[
            "*.c",
            "*.cpp",
            "*.h"
        ]
    }

    property stringList libs: [
        "pthread",
        "boost_system"
    ]
    cpp.staticLibraries:libs.concat(qbs.architecture.contains("arm") ? [
                                                                   "EGL",
                                                                   "GLESv2",
                                                                   "UMP",
                                                                   "Mali"
                                                               ] : [
                                                                           //"glut",
                                                                           "GLEW",
                                                                           "GL",
                                                                           "glfw"
                                                               ]
                                    )
    Group {
        name: "The App itself"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/kest"
    }
}
