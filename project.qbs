import qbs

CppApplication {
    name:"proj"

    cpp.cxxLanguageVersion: "c++14"

    cpp.includePaths: [
        "src/shared",
        "src/capture",
        "src/egl",
        "src/mavlink",
        "src/scene"
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
    }

    Group {
        name: "mavlink"
        prefix: "src/mavlink/"
        files:[
            "*.cpp",
            "*.h"
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
        name: "main"
        prefix: "src/*"
        files: [
            "*.cpp",
            "*.h"
        ]
    }

    cpp.libraryPaths:[
        "../../../usr/lib/arm-linux-gnueabihf"
    ]
    cpp.staticLibraries:[
        "pthread",
        "boost_system",
        "EGL",
        "GLESv2",
        "UMP",
        "Mali"
    ]
}
