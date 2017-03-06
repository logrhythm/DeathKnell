DeathKnell
==========

The `Death` functionality allows final callbacks when fatal signal errors  or `g3log` fatal events are received. Examples of fatal signal errors are: `SIGSEGV`, `SIGABRT`, `SIGFPE`, `SIGILL` and `SIGTERM`. Examples of g3log fatal events are `LOG(FATAL)` and `CHECK(<expression == false>)`  

DeathKnell is an extention to the fatal handling provided by`g3log`. The extension makes is significantly easier to also unit test fatal event handling. 

The API is available in the [[Death.h]](https://github.com/LogRhythm/DeathKnell/blob/master/src/Death.h) header file. API usage is best read in the [[unit tests]](https://github.com/LogRhythm/DeathKnell/blob/master/test/DeathTest.cpp). 
