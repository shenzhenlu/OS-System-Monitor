#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>

//#include <experimental/filesystem>
#include "linux_parser.h"

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

// Utility functions
template <typename T>
T findValueByKey(string const &keyFilter, string const &filename){
  string line, key;
  T value;

  std::ifstream filestream(LinuxParser::kProcDirectory + filename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
          }
      }
    }
  }
  return value; 
};

template <typename T> 
T getValueOfFile(string const &filename) {
  string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};


// Read and return operating system
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
        if (key == "PRETTY_NAME" && !value.empty()) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return kernel
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Read and return process IDs
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
/*
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  for (auto& dir_entry: std::experimental::filesystem::directory_iterator(kProcDirectory)) {

      string filename = dir_entry.path().stem();
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
  }
  return pids;
}
*/

// Read and return the system memory utilization
double LinuxParser::MemoryUtilization() { 
  string memTotal = filterMemTotalString;
  string memFree = filterMemFreeString;
  double Total = findValueByKey<double>(memTotal, kMeminfoFilename);
  double Free = findValueByKey<double>(memFree, kMeminfoFilename);
  return (Total - Free) / Total; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  return getValueOfFile<long>(kUptimeFilename);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string key;
  vector<string> values{};
  string value;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key = filterProcesses;
  return findValueByKey<int>(key, kStatFilename); 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {  
  string key = filterRunningProcesses;
  return findValueByKey<int>(key, kStatFilename); 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  return getValueOfFile<string>(std::to_string(pid) + kCmdlineFilename); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string key = filterProcMem;
  string value = findValueByKey<string>(key, std::to_string(pid) + kStatusFilename); 
    return to_string(stoi(value) / 1024);
  }

// Read and return the user ID associated with a process
int LinuxParser::Uid(int pid) {
  string key = filterUID;
  return findValueByKey<int>(key, std::to_string(pid) + kStatusFilename);  
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  int key;
  string _;
  string value;
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':' , ' ');
      std::istringstream linestream(line);
      linestream >> value >> _ >> key;
        if (key == Uid(pid)) {
          return value; 
        }
    }
  }
  return string();  
}

// Read and return CPU utilization of processes
vector<float> LinuxParser::CpuUtilization(int pid) { 
  vector <float> values{};
  string key;
  string value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
      switch(i) {
        case 13:
          values.emplace_back(std::stof(value) / sysconf(_SC_CLK_TCK));
          break;
        case 14:
          values.emplace_back(std::stof(value) / sysconf(_SC_CLK_TCK));
          break;
        case 15:
          values.emplace_back(std::stof(value) / sysconf(_SC_CLK_TCK));
          break;
        case 16:
          values.emplace_back(std::stof(value) / sysconf(_SC_CLK_TCK));
          break;
        case 21:
          values.emplace_back(std::stof(value) / sysconf(_SC_CLK_TCK));
          break;
      }
    }
  }
  return values;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    return std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0; 
}