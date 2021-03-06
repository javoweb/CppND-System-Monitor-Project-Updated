#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
  ComputeCpuUtilization();
  ram_ = LinuxParser::Ram(pid_);
  rami_ = stol(ram_);
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization

void Process::ComputeCpuUtilization() {
  float total = float(LinuxParser::UpTime(pid_));
  float active = float(LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK));
  // Calculate utilization rate
  utilization_ = active / total;
}
float Process::CpuUtilization() { return utilization_; }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }
// long int Process::UpTime() { return 0.0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return rami_ > a.rami_; }