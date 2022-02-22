#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization(); 

 private:
    long prev_active_ticks;
    long prev_idle_ticks;
};

#endif