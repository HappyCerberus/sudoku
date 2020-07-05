//
// Created by Happy on 05/07/2020.
//

#include "Progressbar.h"
#include <iomanip>
#include <ostream>

Progressbar::Progressbar(int total_count, std::ostream &s, int width)
    : count_(total_count), width_(width), progress_(0), s_(s) {
  s_ << "[" << std::setw(width - 1) << "]" << std::flush;
}

void Progressbar::Step(int step) {
  progress_ = std::min(progress_+step,count_);
  int bars = progress_ * (width_ - 2) / count_;
  s_ << "\r[" << std::setfill('*') << std::setw(bars) << "*"
     << std::setw(width_ - 1 - bars) << std::setfill(' ') << "]" << std::flush;
}
Progressbar::~Progressbar() {
  s_ << std::endl;
}
