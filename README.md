DeathKnell
==========

The `Death` functionality allows final callbacks when fatal signal errors  or `g3log` fatal events are received. 

*Examples of fatal signal errors are:* 
* `SIGSEGV`
* `SIGABRT`
* `SIGFPE`
* `SIGILL`
* `SIGTERM`
Ref: [[g3log `ksignals`]](https://github.com/KjellKod/g3log/blob/master/src/crashhandler_unix.cpp)


*Examples of g3log fatal events are* 
* `LOG(FATAL) --> FATAL logging event`
* `CHECK(<expression == false>) --> FATAL contract event`  



DeathKnell is an extension to the fatal handling provided by [[g3log]](https://github.com/KjellKod/g3log). The extension makes is significantly easier to also unit test fatal event handling. 

The API is available in the [[Death.h]](https://github.com/LogRhythm/DeathKnell/blob/master/src/Death.h) header file. API usage is best read in the [[unit tests]](https://github.com/LogRhythm/DeathKnell/blob/master/test/DeathTest.cpp). 
