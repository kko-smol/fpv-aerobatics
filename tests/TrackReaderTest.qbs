import qbs

CppApplication {
    name: "TrackReaderTest"
    type: ["application", "autotest"]

    cpp.includePaths:[
        "../src/json",
        "../src/track",
        "../src/scene",
        "../src",
        "../src/gis/proj.4-master/src"
    ]

    cpp.cxxLanguageVersion:"c++14"
    files: [
        "src/trackReaderMain.cpp"
    ]
    Group {
        name: "track"
        prefix:"../src/track/"
        files:[
            "*.cpp",
            "*.h"
        ]
    }
    Group {
        name: "gis"
        prefix:"../src/gis/**/**"
        files:[
            "*.cpp",
            "*.c",
            "*.h"
        ]
    }

    Group {
        name: "Test resources"
        files: "data/testTrack.trk"
        qbs.install: true
        qbs.installDir: "bin"
    }
    Group {
        name: "Test App"
        fileTagsFilter: ["application", "autotest"]
        qbs.install: true
        qbs.installDir: "bin"
    }
}
