#### for importing the mcgeometry library into 
def useLibrary(env, libName):
	theDir = Dir(buildDir + libName)
	env.Append(LIBPATH = [theDir])
	env.Append(LIBS    = [libName])

def installHeaders(env, files):
	if env['prefix'] == '.':
		if 'install' in map(str, BUILD_TARGETS):
			print "ERROR: set prefix option to use install target"
		return
	installDir = env['prefix'] + '/include/' + projectName
	env.Alias('install',env.Install(installDir, files))

def installLibrary(env, files):
	if env['prefix'] == '.':
		if 'install' in map(str, BUILD_TARGETS):
			print "ERROR: set prefix option to use install target"
		return
	installDir = env['prefix'] + '/lib/'
	env.Alias('install',env.Install(installDir, files))

def installBinary(env, files):
	if env['prefix'] == '.':
		if 'install' in map(str, BUILD_TARGETS):
			print "ERROR: set prefix option to use install target"
		return
	installDir = env['prefix'] + '/bin/'
	env.Alias('install', env.Install(installDir, files))
###############################################################################
#                  MAIN FILE BEGINS HERE
###############################################################################

# "Variables" stores the previously used compile options
vars = Variables(['variables.cache'])
vars.Add(ListVariable('DBC',  'Design by contract', 7, map(str, range(8)) ))
vars.Add(BoolVariable('DEBUG','Turn optimization off', True))
vars.Add(BoolVariable('VERBOSE','Show all compiling info', False))
vars.Add(BoolVariable('USEENV','Import the local environment variables', False))
vars.Add(PathVariable('prefix','Optional install location for libraries and header files','.'))

# create a new "Environment" from which everything is built
env = Environment(variables = vars)
# if we're supposed to use the shell environment variables, we have to create a
# new Environment object and pass in those variables, since otherwise we'll use
# the original path for all of the tools
if env['USEENV'] == True:
    import os
    env = Environment(ENV = os.environ, variables = vars) 
    print "NOTICE: Setting environment variables to those of parent shell."

# save the variables as new defaults in a cache
vars.Save('variables.cache', env)

### generate help text
env.Help(vars.GenerateHelpText(env) + """
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
if env['VERBOSE'] == False:
	env['CCCOMSTR']   = "\tCompiling $TARGET with $CCC"
	env['CXXCOMSTR']  = "\tCompiling $TARGET with $CXX"
	env['LINKCOMSTR'] = "\tLinking $TARGET"
	env['RANLIBCOMSTR'] = "\tIndexing $TARGET"
	env['ARCOMSTR'] = "\tArchiving $TARGET"
	env['INSTALLSTR'] = "\tInstalling <$SOURCE> as $TARGET"

print env.subst("Using C++ compiler $CXX version $CXXVERSION")
print env.subst("Using design by contract options DBC=${DBC}")

env.Append(CPPDEFINES = {'DBC' : '${DBC}'} )

buildDir = "#build/"
outputDirPath = "#bin/"

#### check to see whether to build optimized or not ####
# and change the build or output directories
if env['DEBUG'] == True:
	print("Using debug mode")
	#buildDir = '#build-debug/'
	#outputDirPath = '#bin/'
else:
	print("Using optimized mode")
	if (str(env['DBC']) != '0'):
		print("NOTICE: optimized build with non-zero dbcVal (%s)" % 
					env.subst("${DBC}"))

	env.MergeFlags('-O3')
	#buildDir = '#build-optimized/'
	#outputDirPath = '#bin-optimized/'

### have compiler warn about everything
env.MergeFlags('-Wall')

### add the base directory so we can #include "Surfaces/blah.hpp" etc.
env.Append(CPPPATH = [buildDir])

# "export" variables to import in the other sconscript files
Export('env')
Export('buildDir')
Export('outputDirPath')
Export('useLibrary')
Export('installHeaders')
Export('installLibrary')
Export('installBinary')

### TARGET OPTIONS ###
# default to not building all the tests etc.; manually add libraries
Default(None)
# let the "all" target build everything locally
Alias('all','.')

# Tell all the inside directories to compile
projectNames = ['transupport', 'mcgeometry']

for projectName in projectNames:
	Export('projectName')
	SConscript(projectName + '/SConscript', variant_dir=(buildDir + projectName))

# optionally build examples
SConscript('examples/SConscript',variant_dir=(buildDir + 'examples'))

#print "BUILD_TARGETS is", map(str, BUILD_TARGETS)

