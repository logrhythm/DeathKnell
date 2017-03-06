DeathKnell
==========

The `Death` functionality allows final callbacks when fatal errors (SEGV, SIGKILL, etc) are received. DeathKnell is an extention to the fatal handling provided by g3log. The extension makes is significantly easier to also unit test fatal event handling. 

The API is available in the [[Death.h]](https://github.com/LogRhythm/DeathKnell/blob/master/src/Death.h) header file. API usage is best read in the [[unit tests]](https://github.com/LogRhythm/DeathKnell/blob/master/test/DeathTest.cpp). 
