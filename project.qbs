import qbs

Project {
    name:"fpb-aerobatics"

    CppApplication {
        name:"proj"
        //condition: qbs.architecture.contains("arm")

        cpp.cxxLanguageVersion: "c++14"

        cpp.includePaths: [
            "src",
            "src/shared",
            "src/capture",
            "src/egl",
            "src/mavlink",
            "src/scene",
            "src/json",
            "src/gis/proj.4-master/src",
            qbs.sysroot+"/usr/include/eigen3",
            qbs.sysroot+"/usr/include",
            qbs.sysroot+"/usr/local/include",
            "/home/kiselev/mali/Mali_OpenGL_ES_SDK_v2.4.4/simple_framework/inc"
        ]

        cpp.defines:[
            "GLES_VERSION=2",
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
            excludeFiles: qbs.architecture.contains("arm") ? [
                                                                 "maliEmulatorRenderer*",
                                                                 "glut*"
                                                             ] : [
                                                                 "eglRenderer*",
                                                                 "maliEmulatorRenderer*"
                                                             ]
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
            name: "track"
            prefix:"src/track/"
            files:[
                "*.cpp",
                "*.h"
            ]
        }
        Group {
            name: "gis"
            prefix:"src/gis/**/**"
            files:[
                "*.cpp",
                "*.c",
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

        Group {
            name: "h264encode"
            prefix: "src/h264Compress/**/"
            files:[
                "*.c",
                "*.cpp",
                "*.h"
            ]
        }

        property stringList libs: [
            "pthread",
            "boost_system",
            "avcodec",
            "avformat",
            "avutil",
            "swresample",
            "x264",
            "z",
            "mp3lame",

        ]

        property stringList eglLibs: qbs.architecture.contains("arm") ? [
                                                                            "EGL",
                                                                            "GLESv2",
                                                                            "UMP",
                                                                            "Mali"
                                                                        ] : [
                                                                            "glut",
                                                                            "GLEW",
                                                                            "GL",
                                                                            "glfw"
                                                                        ]
        cpp.staticLibraries: libs.concat(eglLibs)

        Group {
            name: "The App itself"
            fileTagsFilter: "application"
            qbs.install: true
            qbs.installDir: "/home/kest"
        }

        Properties{
            condition: qbs.architecture.contains("arm")
            cpp.cFlags: "-mfpu=neon-vfpv4"
            cpp.cxxFlags : "-mfpu=neon-vfpv4"
        }
    }

    Project {
        name: "tests"
        condition: false

        AutotestRunner {
            Depends { name: "Qt.core" }
        }
        references: [
            "tests/TrackReaderTest.qbs"
        ]
    }

}
