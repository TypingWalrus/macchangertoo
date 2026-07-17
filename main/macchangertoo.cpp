#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;

// defaults
string interface = "wlan0";

void MACGenerator() {
  // generate random MAC
}

void Version() {
  cout << "macchangertoo by\033[36m TypingWalrus\033[0m\n";
  cout << "Version: 0.1";
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

void MACChange(string mac = "random") {
  // change the MAC
}

void ShowCurrentMAC() {
  // show current MAC
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
  }

  return 0;
}
