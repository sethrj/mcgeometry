# Introduction #

Before doing `svn commit`, make sure to run both
```
scons DBC=0 test
```
and
```
scons DBC=7 test
```

Too often have I accidentally left broken DBC code in a `Require` statement, and sometimes mistakes can happen that only happen when DBC is turned off.