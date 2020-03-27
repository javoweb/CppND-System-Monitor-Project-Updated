#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float prevTotal{0.0};   // Previous total cpu time
  float prevActive{0.0};  // Previous idle cpu time
};

#endif