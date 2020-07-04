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
* Version 12: https://www.youtube.com/watch?v=D1WXQlXBE4c
* Version 13: https://www.youtube.com/watch?v=naaq102wGX4
* Version 14: https://www.youtube.com/watch?v=k1C8PTeFuDM
* Version 15: https://www.youtube.com/watch?v=KUlmMBJ86o0

## Current Benchmark results

The following data set is used for the benchmark: https://www.kaggle.com/rohanrao/sudoku

```
Benchmark results:      Solved 8909310 out of 9000001 requested.
Out of the solved 0 were determined to be incorrect
Solver stats {
        Groups: (1 : 33866835)(2 : 89631)(3 : 2612)(4 : 14)
        Intersections: 43829
        Fish rules:
                X-Wing : 8552
                Swordfish : 223
                Jellyfish : 6
        Finned Fish rules:
                Finned X-Wing : 1392
                Finned Swordfish : 283
                Finned Jellyfish : 73
                Finned Squirmbag : 17
        XChains rules:
                XChain of size 4 : 63118
                XChain of size 6 : 2043
                XChain of size 8 : 57
};
```
