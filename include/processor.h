#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <string>
#include "linux_parser.h"

using std::string;
using std::vector;

class Processor {
 public:
  double Utilization();

  // Declare any necessary private members
 private:
    double prevTotal_{0.0};
    double prevIdle_{0.0};
};

#endif