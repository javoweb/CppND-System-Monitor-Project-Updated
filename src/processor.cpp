#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float utilization{0.0};
    float total = float(LinuxParser::Jiffies());
    float active = float(LinuxParser::ActiveJiffies());
    // Calculate utilization rate
    utilization = (active - prevActive) / (total - prevTotal);
    // Set current state as previous state for next calculation
    prevTotal = total;
    prevActive = active;
    return utilization;
 }