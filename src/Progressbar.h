//
// Created by Happy on 05/07/2020.
//

#ifndef SUDOKU_PROGRESSBAR_H
#define SUDOKU_PROGRESSBAR_H

#include <iosfwd>

class Progressbar {
public:
  Progressbar(int total_count, std::ostream &s, int width = 70);
  void Step(int step);
  ~Progressbar();
private:
  int count_;
  int width_;
  int progress_;
  std::ostream& s_;
};

#endif // SUDOKU_PROGRESSBAR_H
