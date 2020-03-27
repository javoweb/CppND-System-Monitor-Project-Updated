#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float mem_total{1.0};
  float mem_free{0.0};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        mem_total = std::stof(value);
      } else if (key == "MemFree:") {
        mem_free = std::stof(value);
      }
    }
  }
  return mem_free / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string value;
  long uptime{1};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    uptime = std::stoi(value);
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long total_time{0};
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  for (string data : cpu_stat) {
    total_time += stol(data);
  }
  return total_time;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long uptime{0};
  string line;
  string data;
  int it{1};
  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      if (it >= 14 || it <= 17) {
        std::istringstream linestream(line);
        linestream >> data;
        uptime += std::stol(data);
        it++;
      }
    }
  }
  return uptime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long active_time{0};
  vector<string> cpu_stat = LinuxParser::CpuUtilization();
  for (uint it = 0; it < cpu_stat.size(); it++) {
    if (it != CPUStates::kIdle_ && it != CPUStates::kIOwait_) {
      active_time += stol(cpu_stat[it]);
    }
  }
  return active_time;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::ActiveJiffies();
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_stats{};
  string line;
  string data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> data;
    while (linestream >> data) {
      cpu_stats.push_back(data);
    }
  }
  return cpu_stats;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes{0};
  string line;
  string key;
  string data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> data;
      if (key == "processes") {
        processes = std::stoi(data);
      }
    }
  }
  return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int processes{0};
  string line;
  string key;
  string data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> data;
      if (key == "procs_running") {
        processes = std::stoi(data);
      }
    }
  }
  return processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command;
  string line;
  string path = kProcDirectory + std::to_string(pid) + kCmdlineFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> command;
    }
  }
  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  unsigned long long ram{0};
  string line;
  string data;
  int it{1};
  int aux{0};
  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    // Avoid errors if there are spaces in executable's filename
    std::getline(stream, line);
    for (auto& char_ : line) {
      if (char_ == '(') {
        aux = 1;
      } else if (char_ == ')') {
        aux = 0;
      }
      if (aux == 1) {
        char_ = '0';
      }
    }
    std::istringstream linestream(line);
    while (linestream >> data) {
      if (it == 23) {  // Data in field 23 of stat file
        ram = std::stoull(data);
      }
      it++;
    }
  }
  // convert bytes to Mb
  ram /= 1048576;
  return std::to_string(ram);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string uid;
  string line;
  string key;
  string data;
  string path = kProcDirectory + std::to_string(pid) + kStatusFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> data;
      if (key == "Uid:") {
        uid = data;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string user;
  string line;
  string key;
  string data;
  string x;
  string uid = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> data >> x >> key;
      if (key == uid) {
        user = data;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long uptime{0};
  string line;
  string data;
  int it{1};
  int aux{0};
  string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path);
  if (stream.is_open()) {
    // Avoid errors if there are spaces in executable's filename
    while (std::getline(stream, line)) {
      for (auto& char_ : line) {
        if (char_ == '(') {
          aux = 1;
        } else if (char_ == ')') {
          aux = 0;
        }
        if (aux == 1) {
          char_ = '0';
        }
      }
      std::istringstream linestream(line);
      while (linestream >> data) {
        if (it == 22) {  // Data in field 22 of stat file
          uptime += std::stol(data);
        }
        it++;
      }
    }
  }
  // Calculate up time
  uptime = UpTime() - uptime / sysconf(_SC_CLK_TCK);
  return uptime;
}