import os.path

################################################################################
class EnvHelper:
    BaseEnv  = None
    BuildDir = "#build/"
    OutputDirPath = "#bin/"

    def __init__(self, projectName = ""):
        self.env           = EnvHelper.BaseEnv.Clone()
        self.__projectName = projectName

        self.__isInstallTarget = ('install' in map(str, BUILD_TARGETS))

    def checkForPrefix(self):
        if self.__isInstallTarget:
            if EnvHelper.BaseEnv['prefix'] == '.':
                print "ERROR: set prefix option to use install target"
                return False
            else:
                return True
        else:
            return False

    def installHeaders(self, files):
        if (self.checkForPrefix() == False): return
        installDir = os.path.join(EnvHelper.BaseEnv['prefix'],
                        'include/', self.__projectName)
        self.env.Alias('install', self.env.Install(installDir, files))

    def installLibrary(self, files):
        if (self.checkForPrefix() == False): return
        installDir = os.path.join(EnvHelper.BaseEnv['prefix'],
                        'lib/')
        self.env.Alias('install', self.env.Install(installDir, files))

    def installBinary(self, files):
        if (self.checkForPrefix() == False): return
        installDir = os.path.join(EnvHelper.BaseEnv['prefix'], 'bin/')
        self.env.Alias('install', self.env.Install(installDir, files))

    def useLibrary(self, libName):
        theDir = Dir(self.BuildDir + libName)
        self.env.Append(LIBPATH = [theDir])
        self.env.Append(LIBS    = [libName])

    def includePath(self, basepath, lib=None):
        if (basepath == "."):
            return

        self.env.AppendUnique(CPPPATH = [os.path.join(basepath, 'include/')])
        self.env.AppendUnique(LIBPATH = [os.path.join(basepath, 'lib/')])
        if lib:
            self.env.AppendUnique(LIBS    = [lib])
    
###############################################################################
#                  MAIN FILE BEGINS HERE
###############################################################################

# "Variables" stores the previously used compile options
vars = Variables(['variables.cache'])
vars.Add(ListVariable('DBC',  'Design by contract', 7, map(str, range(8)) ))
vars.Add(BoolVariable('DEBUG','Turn optimization off', True))
vars.Add(BoolVariable('VERBOSE','Show all compiling info', False))
vars.Add(BoolVariable('USEENV','Import the local environment variables', False))
vars.Add(PathVariable('blitz','Location of Blitz library','.'))
vars.Add(PathVariable('prefix','Optional install location for libraries and header files','.'))

# create a new "Environment" from which everything is built
baseEnv = Environment(variables = vars)
# if we're supposed to use the shell environment variables, we have to create a
# new Environment object and pass in those variables, since otherwise we'll use
# the original path for all of the tools
if baseEnv['USEENV'] == True:
    import os
    baseEnv = Environment(ENV = os.environ, variables = vars) 
    print "NOTICE: Setting environment variables to those of parent shell."

# save the variables as new defaults in a cache
vars.Save('variables.cache', baseEnv)

### generate help text
baseEnv.Help(vars.GenerateHelpText(baseEnv) + """
----- TARGETS -----
default:  build libraries
install:  copy libraries and headers to prefix directory
test:     build unit tests
examples: build example binaries
all:      make libraries, unit tests, and examples

Setting up an install directory:
            scons prefix=/Users/Shared/_local/
            scons install
""")

#### print detailed build log?
if baseEnv['VERBOSE'] == False:
    baseEnv['CCCOMSTR']   = "\tCompiling $TARGET with $CCC"
    baseEnv['CXXCOMSTR']  = "\tCompiling $TARGET with $CXX"
    baseEnv['LINKCOMSTR'] = "\tLinking $TARGET"
    baseEnv['RANLIBCOMSTR'] = "\tIndexing $TARGET"
    baseEnv['ARCOMSTR'] = "\tArchiving $TARGET"
    baseEnv['INSTALLSTR'] = "\tInstalling <$SOURCE> as $TARGET"

print baseEnv.subst("Using C++ compiler $CXX version $CXXVERSION")
print baseEnv.subst("Using design by contract options DBC=${DBC}")

baseEnv.Append(CPPDEFINES = {'DBC' : '${DBC}'} )

#### check to see whether to build optimized or not ####
# and change the build or output directories
if baseEnv['DEBUG'] == True:
    print("Using debug mode")
else:
    print("Using optimized mode")
    if (str(baseEnv['DBC']) != '0'):
        print("NOTICE: optimized build with non-zero dbcVal (%s)" % 
                    baseEnv.subst("${DBC}"))

    baseEnv.MergeFlags('-O3')

### have compiler warn about everything
baseEnv.MergeFlags('-Wall')

### add the base directory so we can #include "Surfaces/blah.hpp" etc.
baseEnv.Append(CPPPATH = [EnvHelper.BuildDir])

### set up the environment helper
EnvHelper.BaseEnv = baseEnv
Export('EnvHelper')

### TARGET OPTIONS ###
# default to not building all the tests etc.; manually add libraries
Default(None)
# let the "all" target build everything locally
Alias('all','.')

# Tell all the inside directories to compile
projectNames = ['transupport', 'mcgeometry']

for projectName in projectNames:
    Export('projectName')
    SConscript(projectName + '/SConscript',
            variant_dir=(EnvHelper.BuildDir + projectName))

# optionally build examples
SConscript('examples/SConscript',
        variant_dir=(EnvHelper.BuildDir + 'examples'))

#print "BUILD_TARGETS is", map(str, BUILD_TARGETS)

