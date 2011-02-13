# -*- Python -*-

import os

VariantDir("build", "src", duplicate=0)
VariantDir("build-test", "test", duplicate=0)
env = Environment(ENV={'PATH' : os.environ['PATH']})

env.Append(CPPPATH=["third-party/UnitTest++/src"])
env.Append(LIBPATH=["third-party/UnitTest++"])


# The core operation is testing.
test = env.Program("build-test/bin/TestProgram", Glob("build-test/*.cc"), LIBS=["UnitTest++"])
env.NoClean(test)
test = env.InstallAs("#/bin/Test", "build-test/bin/TestProgram")
env.Alias("Test", test)
test = env.Command("run-tests", test, "bin/Test")
env.Alias("test", test)
Default(test)


