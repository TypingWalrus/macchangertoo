#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <algorithm>
#include <random>

using namespace std;

// defaults
string interface = "wlan0";

string MACGenerator() {
  random_device rd;
  mt19937 g(rd());

  string ChooseFrom = "AAABBBCCCDDDEEEFFF111222333444555666777888999";
  shuffle(ChooseFrom.begin(), ChooseFrom.end(), g);

  string MAC;
  for(int i = 0; i < 12; i++) {
    if(i % 2 == 0 && i != 0) {
      MAC += ":";
    } MAC += ChooseFrom[i];
  }

  return MAC;
}

void Version() {
  cout << "macchangertoo by\033[36m TypingWalrus\033[0m\n";
  cout << "Version: 0.12";
  exit(0);
}

void Help() {
  cout << "Usage:\n";
  cout << "-v, --version      Show current version\n";
  cout << "-h, --help         Display this page\n";
  cout << "-r                 Set Random MAC\n";
  cout << "-m                 Set MAC manually\n";
  cout << "\033[32m                     macchangertoo -i [INTERFACE] -m [MAC]\033[0m\n";
  cout << "-s, --show         Show current MAC\n";
  cout << "-i, --interface    Set the interface\n";
  exit(0);
}

string ShowCurrentMAC() {
  FILE* com = popen("ip link | awk '/ether/ {print $2}'", "-r");
  string mac;
  char buf[128];

  while(fgets(buf, sizeof(buf), com) != nullptr) {
    mac += buf;
  } pclose(com);

  return mac;

  // FIX THIS LATER
}

void MACChange(string mac = "random") {
  if(mac == "random") {
    mac = MACGenerator();
  }

  // cout << "Current MAC: " << ShowCurrentMAC();
  string command = "sudo ip link set dev " + interface + " down && " + "sudo ip link set dev " + interface + " address " + mac + " && sudo ip link set dev " + interface + " up";
  system(command.c_str());
}

int main(int argc, char* argv[]) {
  // checking the flags
  if(argc > 1) {
    for(int i = 1; i < argc; i++) {
      string arg = argv[i];

      if(arg == "-v" || arg == "--version") {
        Version();
      }

      else if(arg == "-h" || arg == "--help") {
        Help();
      }

      else if(arg == "-r") {
        MACChange();
      }

      else if(arg == "-m") {
        if(argc >= 3) {
          MACChange(argv[i + 1]);
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -m [MAC]";
        }
      }

      else if(arg == "-i" || arg == "--interface") {
        if(argc >= 3) {
          interface = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -r";
        }
      }

      else if(arg == "-s" || arg == "--show") {
        ShowCurrentMAC();
      }
    }
  } else {
    Help();
  }

  return 0;
}
