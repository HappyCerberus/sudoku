# Logic-based Sudoku Solver


[![Build status](https://ci.appveyor.com/api/projects/status/elojn7ea90xhfy1i?svg=true)](https://ci.appveyor.com/project/HappyCerberus/sudoku/branch/master)
[![Build Status](https://travis-ci.org/HappyCerberus/sudoku.svg?branch=master)](https://travis-ci.org/HappyCerberus/sudoku)
[![codecov](https://codecov.io/gh/HappyCerberus/sudoku/branch/master/graph/badge.svg)](https://codecov.io/gh/HappyCerberus/sudoku)
[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/HappyCerberus/sudoku/blob/master/LICENSE)

This repository contains the code for a "human-like" Sudoku solver. This solver does not do any guessing (no backtracking) and only employs solving techniques used by human solvers.

The ultimate goal is to build a high-performance solver that can be used as a tool to help generate interesting Sudoku puzzles.

If you have any questions about the code, join me for one of the streams, every Sat&Sun 14:30 CE(S)T at [Youtube](https://www.youtube.com/user/HappyCerberus) or [Twitch](https://twitch.tv/happycerberus).

The UI for this solver lives in the [sudoku_ui repository](https://happycerberus.github.io/sudoku_ui/) and you can test the current build in the [sudoku_playground repository](https://happycerberus.github.io/sudoku_playground/).

## Stream archive

The video & audio quality improves over time and I'm continually making improvements.

* [2020/05/09](https://www.youtube.com/watch?v=5sWMJq4A7w8)
* [2020/05/10](https://www.youtube.com/watch?v=V4AI9G5i_SE)
* [2020/05/16](https://www.youtube.com/watch?v=DRZIKmg4SlI)
* [2020/05/17](https://www.youtube.com/watch?v=TaL9mO_nx_k)
* [2020/05/23](https://www.youtube.com/watch?v=bBplxuWt6Uw)
* [2020/05/24](https://www.youtube.com/watch?v=TJvSay58Kts)
* [2020/05/30](https://www.youtube.com/watch?v=2NGetdNAMMY)
* [2020/05/31](https://www.youtube.com/watch?v=Z9soPicHYlQ)
* [2020/06/06](https://www.youtube.com/watch?v=1BDzCkx2H3M)
* [2020/06/07](https://www.youtube.com/watch?v=tTQywLiZ9sk)
* [2020/06/13](https://www.youtube.com/watch?v=JLhMUNtEJoY)
* [2020/06/14](https://www.youtube.com/watch?v=muoFClZMEsQ)
* [2020/06/20](https://www.youtube.com/watch?v=D1WXQlXBE4c)
* [2020/06/21](https://www.youtube.com/watch?v=naaq102wGX4)
* [2020/06/27](https://www.youtube.com/watch?v=k1C8PTeFuDM)
* [2020/06/28](https://www.youtube.com/watch?v=KUlmMBJ86o0)
* [2020/07/04](https://www.youtube.com/watch?v=geyO9bl_UJ4)
* [2020/07/05](https://www.youtube.com/watch?v=IP1qALIQAow)
* [2020/07/11](https://www.youtube.com/watch?v=0jpjYkazQ9k)
* [2020/07/12](https://www.youtube.com/watch?v=8KMKMgw0GAE)
* [2020/07/18](https://www.youtube.com/watch?v=HRfkPYK9Ldg)
* [2020/07/19](https://www.youtube.com/watch?v=WKybY8xVm6g)

## Current Benchmark results

The following data set is used for the benchmark: https://www.kaggle.com/rohanrao/sudoku

```
Benchmark results:      Solved 8985618 out of 9000001 requested.
Out of the solved 0 were determined to be incorrect
Solver stats {
        Groups: (1 : 34489883)(2 : 146932)(3 : 6688)(4 : 116)
        Intersections: 95846
        XYChains: 85937
        Fish rules:
                X-Wing : 22540
                Swordfish : 2483
                Jellyfish : 43
        Finned Fish rules:
                Finned X-Wing : 5660
                Finned Swordfish : 1416
                Finned Jellyfish : 314
                Finned Squirmbag : 18
        XChains rules:
                XChain of size 4 : 91131
                XChain of size 6 : 6059
                XChain of size 8 : 303
                XChain of size 10 : 3
};
```
