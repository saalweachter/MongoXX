# -*- Python -*-

import os

VariantDir("build", "src", duplicate=0)
VariantDir("build-test", "test", duplicate=0)
env = Environment(ENV={'PATH' : os.environ['PATH']})

env.Append(CCFLAGS=["-Wall", "-Werror"])
env.Append(CPPPATH=["/opt/local/include"])
env.Append(CPPPATH=["."])
env.Append(LIBPATH=["/opt/local/lib"])
env.Append(CPPPATH=["third-party/UnitTest++/src"])
env.Append(LIBPATH=["third-party/UnitTest++"])


# mongoxx is header-only.
# the only operation here is testing.

# download the UnitTest++ library.
unittestxx = env.Command("unittestxx", [],
                         [ Mkdir("third-party"),
                           "cd third-party && wget 'http://sourceforge.net/projects/unittest-cpp/files/UnitTest%2B%2B/1.4/unittest-cpp-1.4.zip/download'",
                           "cd third-party && unzip unittest-cpp-1.4.zip",
                           "cd third-party/UnitTest++ && make all" ])



test = env.Program("build-test/bin/TestMongoXX", Glob("build-test/*.cc"),
                   LIBS=["UnitTest++",
                         "mongoclient", "boost_system",
                         "boost_thread-mt", "boost_filesystem",
                         "boost_program_options"])
#Requires(test, unittestxx)
test = env.InstallAs("#/TestMongoXX", "build-test/bin/TestMongoXX")
env.NoClean(test)
env.Alias("Test", test)
test = env.Command("run-tests", test, "./TestMongoXX")
env.Alias("test", test)
Default(test)


