# Sudoku fun project


[![Build status](https://ci.appveyor.com/api/projects/status/elojn7ea90xhfy1i?svg=true)](https://ci.appveyor.com/project/HappyCerberus/sudoku/branch/master)
[![Build Status](https://travis-ci.org/HappyCerberus/sudoku.svg?branch=master)](https://travis-ci.org/HappyCerberus/sudoku)
[![codecov](https://codecov.io/gh/HappyCerberus/sudoku/branch/master/graph/badge.svg)](https://codecov.io/gh/HappyCerberus/sudoku)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/HappyCerberus/sudoku/blob/master/LICENSE)

This repository contains the code for a "human-like" Sudoku solver. This is a fun project of mine that I have been live streaming every weekend.

The main idea is to create a solver that implements human-style Sudoku solving techniques and is able to judge puzzle difficulty from that.

Ultimately I want to use this solver to generate some fun Sudoku variant puzzles.

If you have any questions about the code, join me for one of the streams, every Sat&Sun 14:30 CE(S)T at https://www.youtube.com/user/HappyCerberus

You can also hit me up on my Discord server (https://discord.gg/qTAZkd), or here on Github.

Previous live streams:

* Version 0 (keyboard noise) :  https://www.youtube.com/watch?v=5sWMJq4A7w8
* Version 1 : https://www.youtube.com/watch?v=V4AI9G5i_SE
* Version 2 : https://www.youtube.com/watch?v=DRZIKmg4SlI
* Version 3 : https://www.youtube.com/watch?v=TaL9mO_nx_k
* Version 4 : https://www.youtube.com/watch?v=bBplxuWt6Uw
* Version 5 : https://www.youtube.com/watch?v=TJvSay58Kts
* Version 6 : https://www.youtube.com/watch?v=2NGetdNAMMY
* Version 7 : https://www.youtube.com/watch?v=Z9soPicHYlQ
* Version 8 : https://www.youtube.com/watch?v=1BDzCkx2H3M
* Version 9 : https://www.youtube.com/watch?v=tTQywLiZ9sk
* Version 10: https://www.youtube.com/watch?v=JLhMUNtEJoY
* Version 11: https://www.youtube.com/watch?v=muoFClZMEsQ

## Current Benchmark results

The following data set is used for the benchmark: https://www.kaggle.com/rohanrao/sudoku

```
Benchmark results: 	Solved 8920361 out of 9000001 requested.
Out of the solved 135 were determined to be incorrect
Solver stats {
	Groups: (1 : 34111982)(2 : 96463)(3 : 3015)(4 : 50)
	Intersections: 53463
	Fish rules: 
		X-Wing : 11457
		Swordfish : 1712
		Jellyfish : 15
	Finned Fish rules: 
		Finned X-Wing : 70165
		Finned Swordfish : 14118
		Finned Jellyfish : 3985
		Finned SquirmbagWhale : 1358
		Finned Leviathan : 453
};
```
