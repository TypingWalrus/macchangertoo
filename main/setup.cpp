#include <iostream>
#include <cstdio>
#include <sys/wait.h>
#include <string>
#include <fstream>

using namespace std;

string Info(string command) {
  FILE* com = popen(command.c_str(), "r");
  string out, inter;
  char buf[128];

  while(fgets(buf, sizeof(buf), com) != nullptr) {
    out += buf;
  } pclose(com);

  for(auto i : out) {
    if(i != '\n') {
      inter += i;
    }
  }
  
  return inter;
}

void MakeDefaults(string interface, string default_hostname) {
  ofstream out("defaults.hpp");
  string code = "#pragma once\n#include <string>\nstd::string interface_default = \"" + interface + "\", default_hostname_default = \"" + default_hostname + "\";";

  out << code;
  out.close();
}

int main() {
  string interface, default_hostname = Info("hostname");

  cout << "Your interface (" << Info("ip -br addr show up | awk '/UP/ {print $1}'") << "): ";
  getline(cin, interface);
  if(interface == "") {
    interface = Info("ip -br addr show up | awk '/UP/ {print $1}'");
  }

  MakeDefaults(interface, default_hostname);

  return 0;
}
