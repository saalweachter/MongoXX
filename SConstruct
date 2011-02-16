# -*- Python -*-

import os

VariantDir("build", "src", duplicate=0)
VariantDir("build-test", "test", duplicate=0)
env = Environment(ENV={'PATH' : os.environ['PATH']})

env.Append(CCFLAGS=["-Wall", "-Werror"])
env.Append(CPPPATH=["/opt/local/include"])
env.Append(CPPPATH=["third-party/UnitTest++/src"])
env.Append(LIBPATH=["third-party/UnitTest++"])


library_library = env.StaticLibrary("build/mongoxx", Glob("build/*.cc"),
                                    LIBS=["mongoclient", "boost_system",
                                          "boost_thread-mt", "boost_filesystem",
                                          "boost_program_options"])
library = env.InstallAs("#/install/lib/libmongoxx.a", library_library)
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
testenv.Append(CPPPATH=["install/include"])
testenv.Append(LIBPATH=["install/lib/"])
testenv.Append(LIBPATH=["/opt/local/lib"])

test = testenv.Program("build-test/bin/TestMongoXX", Glob("build-test/*.cc"),
                       LIBS=["UnitTest++", library,
                             "mongoclient", "boost_system",
                             "boost_thread-mt", "boost_filesystem",
                             "boost_program_options"])
Requires(test, library)
Requires(test, headers)
test = testenv.InstallAs("#/TestMongoXX", "build-test/bin/TestMongoXX")
testenv.NoClean(test)
testenv.Alias("Test", test)
test = testenv.Command("run-tests", test, "./TestMongoXX")
testenv.Alias("test", test)
Default(test)


