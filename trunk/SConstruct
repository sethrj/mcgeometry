#### for importing the mcgeometry library into 
def useLibrary(env, libName):
	Import('buildDir')
	theDir = Dir(buildDir + libName)
	env.Append(LIBPATH = [theDir])
	env.Append(LIBS    = [libName])

###############################################################################
#                  MAIN FILE BEGINS HERE
###############################################################################

# "Variables" stores the previously used compile options
vars = Variables(['variables.cache'])
vars.Add(ListVariable('DBC',  'Design by contract', 7, map(str, range(8)) ))
vars.Add(BoolVariable('DEBUG','Turn optimization off', True))
vars.Add(BoolVariable('VERBOSE','Show all compiling info', False))

# create a new "Environment" from which everything is built
env = Environment(variables = vars)

vars.Save('variables.cache', env)
Help(vars.GenerateHelpText(env))

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

# "export" variables to import in the subdirectories
Export('env')
Export('buildDir')
Export('outputDirPath')
Export('useLibrary')

# Tell all the inside directories to compile
projectNames = ['transupport', 'mcgeometry']

for name in projectNames:
	SConscript(name + '/SConscript', variant_dir=(buildDir + name))
