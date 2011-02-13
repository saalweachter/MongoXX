# -*- Python -*-

import os

VariantDir("build", "src", duplicate=0)
VariantDir("build-test", "test", duplicate=0)
env = Environment(ENV={'PATH' : os.environ['PATH']})

env.Append(CCFLAGS=["-Wall", "-Werror"])
env.Append(CPPPATH=["third-party/UnitTest++/src"])
env.Append(LIBPATH=["third-party/UnitTest++"])


library = env.StaticLibrary("build/mongoxx", Glob("build/*.cc"))
library = env.InstallAs("#/install/lib/libmongoxx.a", library)
env.NoClean(library)

headers = env.Command("headers", [],
                      [ Mkdir("install"),
                        Mkdir("install/include"),
                        Mkdir("install/include/mongoxx"),
                        "cp src/*.hh install/include/mongoxx/" ])
env.Alias("library", library)
env.Alias("headers", headers)
all = env.Alias("all", [ library, headers ])


# The core operation is testing.
testenv = env.Clone()
testenv.Append(LIBPATH=["install/lib/"])
testenv.Append(CPPPATH=["install/include"])

test = testenv.Program("build-test/bin/TestProgram", Glob("build-test/*.cc"), LIBS=["UnitTest++", library])
test = testenv.InstallAs("#/bin/Test", "build-test/bin/TestProgram")
testenv.NoClean(test)
testenv.Alias("Test", test)
test = testenv.Command("run-tests", test, "bin/Test")
testenv.Alias("test", test)
Default(test)


