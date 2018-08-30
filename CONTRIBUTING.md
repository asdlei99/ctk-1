Contributing to CTK
===================
First of all, thanks for dropping by! This is an ambitious project and it requires more than just a single person
to get it done, so any help is appreciated.


Found a Bug?
------------
If you've found a bug you can create a bug report [here on GitHub](https://github.com/dr-soft/ctk/issues). The more
information you can provide, the quicker I'll be able to get it fixed. Sample programs and sample files help a lot,
as does a detailed list of steps we can follow to reproduce the problem.

You can also submit a pull request which, provided your fix is correct and well written, is the quickest way to get
the bug fixed.


Pull Requests
-------------
If you want to do actual development on CTK, pull requests are the best place to start. Just don't do any
significant amount of work without talking to me first - If I don't like it, it won't be merged.


Branch Management
-----------------
The master branch is stable and always in a state where I can do an emergency release. The major work for the
next release is in the "dev" branch.

Bug fixes and major features are always implemented in their own branch, and then merged when ready. 


Programming Standard
--------------------
Everything is C89, including tests. I'm supporting VC6 and GCC with `-std=c89 -pedantic -Wall -Wpedantic`. Look at
the existing code for syntax standards.
