_G['Proj'] = nil

function project(name)
    assert(Proj == nill)
    Proj = Project(name)
    return function(attrs)
        -- load languages required by the project
        resolveAttrs(attrs)
        addLanguage(attrs.LANGUAGES)
        proj::setVersion(attrs.VERSION)
        proj::setDescription(attrs.DESC)
    end
end

-- Maker.lua
Project "thinc" {
    LANGUAGES {"C", "CXX"},
    VERSION "0.0.1",
    DESC "A simple project"
}

CFLAGS {CFLAGS(), '-Wall'}

CFlags
CPPFlags
CurrentSourceDir
CurrentBinDir
SourceDir
ProjectDir

AddBinary "thinc" {
    SHARED,
    SOURCES {
        'lib/core/thinc.c'
    }
}

Project:emit(makefileWriter)
