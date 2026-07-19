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

  string ChooseFrom = "AABBCCDDEEFF00112233445566778899";
  shuffle(ChooseFrom.begin(), ChooseFrom.end(), g);

  string MAC;
  for(int i = 0; i < 12; i++) {
    if(i == 0 && isdigit(ChooseFrom[i]) && ChooseFrom[i] % 2 != 0) {
      ChooseFrom[i]--;
    }
    if(i == 1) {
      int value = stoi(string(1, ChooseFrom[1]), nullptr, 16);
      if(value % 2 != 0) {
        ChooseFrom[1] = "0123456789ABCDEF"[value - 1];
      }
    }

    if(i % 2 == 0 && i != 0) {
      MAC += ":";
    } MAC += ChooseFrom[i];
  }

  return MAC;
}

void Version() {
  cout << "macchangertoo by\033[36m TypingWalrus\033[0m\n";
  cout << "Version: 0.21";
  exit(0);
}

void Help() {
  cout << "Usage:\n";
  cout << "-v, --version      Show current version\n";
  cout << "-h, --help         Display this page\n";
  cout << "-r                 Set Random MAC\n";
  cout << "-m                 Set MAC manually\n";
  cout << "\033[32m                     macchangertoo -i [INTERFACE] -m [MAC]\033[0m\n";
  cout << "-p                 Reset MAC\n";
  cout << "-s, --show         Show current MAC\n";
  cout << "-i, --interface    Set the interface\n";
  exit(0);
}

string ShowMAC(string command) {
  FILE* com = popen(command.c_str(), "r");
  string out, mac;
  char buf[128];

  while(fgets(buf, sizeof(buf), com) != nullptr) {
    out += buf;
  } pclose(com);

  for(auto i : out) {
    if(i != '\n') {
      mac += i;
    }
  }

  return mac;
}

void MACChange(string mac = "random") {
  if(mac == "random") {
    mac = MACGenerator();
  }

  cout << "\033[33mPermanent MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $6}'");
  cout << "\n\033[33mPrevious MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $2}'");
  cout << "\n\033[32mCurrent MAC:\033[0m " << mac;
  string command = "sudo ip link set dev " + interface + " down && sudo ip link set dev " + interface + " address " + mac + " && sudo ip link set dev " + interface + " up";
  system(command.c_str());
  exit(0);
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
        if(argc >= 4) {
          for(int j = 2; j < argc; j++) {
            string arg_j = argv[j];
            if(arg_j == "-i" || arg_j == "--interface") {
              interface = argv[j + 1];
            }
          }
        }
        MACChange();
      }

      else if(arg == "-m") {
        if(argc >= 3) {
          if(argc >= 5) {
            for(int j = 3; j < argc; j++) {
              string arg_j = argv[j];
              if(arg_j == "-i" || arg_j == "--interface") {
                interface = argv[j + 1];
              }
            }
          }
          MACChange(argv[i + 1]);
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -m [MAC]";
          exit(0);
        }
      }

      else if(arg == "-i" || arg == "--interface") {
        if(argc >= 3) {
          interface = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -r";
          exit(0);
        }
      }

      else if(arg == "-s" || arg == "--show") {
        cout << "\033[33mCurrent MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $2}'");
      }

      else if(arg == "-p") {
        if(argc >= 4) {
          for(int j = 2; j < argc; j++) {
            string arg_j = argv[j];
            if(arg_j == "-i" || arg_j == "--interface") {
              interface = argv[j + 1];
            }
          }
        }
        string permMAC = ShowMAC("ip link | awk '/ether/ {print $6}'");
        MACChange(permMAC);
      }
    }
  } else {
    Help();
  }

  return 0;
}
