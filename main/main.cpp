#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <algorithm>
#include <random>

// including header
#include "mac_vendors.hpp"

using namespace std;

// defaults
string interface = "wlan0";

bool flag_random, flag_manual, flag_permanent, flag_vendor;
string flag_manual_address, flag_vendor_name;

string MACGenerator(bool known_vendor = false) {
  random_device rd;
  mt19937 g(rd());

  string ChooseFrom = "aabbccddeeff00112233445566778899";
  shuffle(ChooseFrom.begin(), ChooseFrom.end(), g);

  string MAC;
  if(!known_vendor) {
    for(int i = 0; i < 12; i++) {
      if(i == 0 && isdigit(ChooseFrom[i]) && ChooseFrom[i] % 2 != 0) {
        ChooseFrom[i]--;
      }
      if(i == 1) {
        int value = stoi(string(1, ChooseFrom[1]), nullptr, 16);
        if(value % 2 != 0) {
          ChooseFrom[1] = "0123456789abcdef"[value - 1];
        }
      }

      if(i % 2 == 0 && i != 0) {
        MAC += ":";
      } MAC += ChooseFrom[i];
    }
  } else {
    for(int i = 0; i < 6; i++) {
      if(i % 2 == 0 && i != 0) {
        MAC += ":";
      } MAC += ChooseFrom[i];
    }
  }

  return MAC;
}

void Version() {
  // Logo
  string ascii_art = R"(
    __  ___           ________                               ______          
   /  |/  /___ ______/ ____/ /_  ____ _____  ____ ____  ____/_  __/___  ____ 
  / /|_/ / __ `/ ___/ /   / __ \/ __ `/ __ \/ __ `/ _ \/ ___// / / __ \/ __ \
 / /  / / /_/ / /__/ /___/ / / / /_/ / / / / /_/ /  __/ /   / / / /_/ / /_/ /
/_/  /_/\__,_/\___/\____/_/ /_/\__,_/_/ /_/\__, /\___/_/   /_/  \____/\____/ 
                                          /____/                             )";
  
  cout << "\033[36m" << ascii_art << "\033[0m\n\n";
  cout << "macchangertoo by\033[36m TypingWalrus\033[0m\n";
  cout << "Version: 0.61";
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
  cout << "--vendor           Set MAC to known vendors (Nokia, Dell, Apple, etc.)\n";
  cout << "\033[32m                     macchangertoo -i [INTERFACE] --vendor [VENDOR]\033[0m\n";
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

  string command = "sudo ip link set dev " + interface + " down && sudo ip link set dev " + interface + " address " + mac + " && sudo ip link set dev " + interface + " up";
  cout << "\033[33mPermanent MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $6}'");
  cout << "\n\033[33mPrevious MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $2}'");
  cout << "\n\033[32mCurrent MAC:\033[0m " << mac;
  system(command.c_str());
  exit(0);
}

void WithVendor(string vendor) {
  map<string, vector<string>> ChooseFromVendors = vendors;

  // checking if it exists
  if(ChooseFromVendors[vendor].empty()) {
    cout << "\033[31m Error:\033[0m Provided vendor doesn't exist.\n";
    exit(0);
  }

  random_device rd;
  mt19937 g(rd());
  shuffle(ChooseFromVendors[vendor].begin(), ChooseFromVendors[vendor].end(), g);

  string MAC = ChooseFromVendors[vendor][0] + ":" + MACGenerator(true);
  MACChange(MAC);
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
        flag_random = true;
      }

      else if(arg == "-m") {
        flag_manual = true;
        if(argc > i) {
          flag_manual_address = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -m [MAC]";
          exit(0);
        }
      }

      else if(arg == "-i" || arg == "--interface") {
        if(argc > i) {
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
        flag_permanent = true;
      }

      else if(arg == "--vendor") {
        flag_vendor = true;
        if(argc > i) {
          flag_vendor_name = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] --vendor [VENDOR]";
          exit(0);
        }
      }
    }
  } else {
    Help();
  }

  // main
  if((flag_random && flag_random == flag_manual) || (flag_random && flag_random == flag_permanent) || (flag_manual && flag_manual == flag_permanent)) {
    Help();
  }

  if(flag_random) {
    MACChange();
  }

  else if(flag_manual) {
    MACChange(flag_manual_address);
  }

  else if(flag_permanent) {
    string permMAC = ShowMAC("ip link | awk '/ether/ {print $6}'");
    MACChange(permMAC);
  }

  else if(flag_vendor) {
    WithVendor(flag_vendor_name);
  }

  return 0;
}
