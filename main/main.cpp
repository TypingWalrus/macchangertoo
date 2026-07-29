#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <algorithm>
#include <random>

// including headers
#include "mac_vendors.hpp"
#include "defaults.hpp"

using namespace std;

// defaults
string interface = interface_default;
string default_hostname = default_hostname_default;

bool flag_random, flag_manual, flag_permanent, flag_vendor, flag_hostname, flag_keep, flag_ttl;
string flag_manual_address, flag_vendor_name, flag_hostname_name, flag_ttl_value;

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
  cout << "Version: 1.06\n";
  exit(0);
}

void Help() {
  cout << "Usage:\n";
  cout << "\033[33m-v\033[0m,\033[33m --version\033[0m      Show current version\n";
  cout << "\033[33m-h\033[0m,\033[33m --help\033[0m         Display this page\n";
  cout << "\033[33m-r\033[0m                 Set Random MAC\n";
  cout << "\033[33m-m\033[0m                 Set MAC manually\n";
  cout << "\033[32m                     macchangertoo -i [INTERFACE] -m [MAC]\033[0m\n";
  cout << "\033[33m-p\033[0m                 Reset MAC\n";
  cout << "\033[33m-s\033[0m,\033[33m --show\033[0m         Show current MAC\n";
  cout << "\033[33m-i\033[0m,\033[33m --interface\033[0m    Set the interface\n";
  cout << "\033[33m--vendor\033[0m           Set MAC to known vendors (Nokia, Dell, Apple, etc.)\n";
  cout << "\033[32m                     macchangertoo -i [INTERFACE] --vendor [VENDOR]\033[0m\n";
  cout << "\033[33m--hostname\033[0m         Change hostname (will show up as that device)\n";
  cout << "\033[32m                     macchangertoo --hostname [HOSTNAME]\033[0m\n";
  cout << "\033[33m--reset-hostname\033[0m   Reset hostname\n";
  cout << "\033[32m                     macchangertoo --reset-hostname\033[0m\n";
  cout << "\033[33m-k\033[0m,\033[33m --keep\033[0m         Keep the same vendor bytes\n";
  cout << "\033[33m-t\033[0m,\033[33m --ttl\033[0m          Modify TTL (Android,MacOS,Linux=64, Windows=128)\n";
  cout << "\033[32m                     macchangertoo --ttl 128\033[0m\n";
  exit(0);
}

string ShowMAC(string command, bool only_vendor = false) {
  FILE* com = popen(command.c_str(), "r");
  string out, mac;
  char buf[128];

  while(fgets(buf, sizeof(buf), com) != nullptr) {
    out += buf;
  } pclose(com);

  if(only_vendor) {
    for(int i = 0; i < 9; i++) {
      mac += out[i];
    }
  } else {
    for(auto i : out) {
      if(i != '\n') {
        mac += i;
      }
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
  cout << "\n\033[32mCurrent MAC:\033[0m " << mac << "\n";
  system(command.c_str());
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

// Extras
void ChangeHostname(string name) {
  string command = "sudo nmcli general hostname " + name + " && sudo service NetworkManager restart";

  system(command.c_str());
}

void ChangeTTL(string value) {
  string command = "sudo sysctl -w net.ipv4.ip_default_ttl=" + value;
  system(command.c_str());
}

int main(int argc, char* argv[]) {
  freopen("/dev/null", "w", stderr);

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
        if(argc > i + 1) {
          flag_manual_address = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -m [MAC]\n";
          exit(0);
        }
      }

      else if(arg == "-i" || arg == "--interface") {
        if(argc > i + 1) {
          interface = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -r\n";
          exit(0);
        }
      }

      else if(arg == "-s" || arg == "--show") {
        cout << "\033[33mCurrent MAC:\033[0m " << ShowMAC("ip link | awk '/ether/ {print $2}'") << "\n";
      }

      else if(arg == "-p") {
        flag_permanent = true;
      }

      else if(arg == "--vendor") {
        flag_vendor = true;
        if(argc > i + 1) {
          flag_vendor_name = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] --vendor [VENDOR]\n";
          exit(0);
        }
      }

      else if(arg == "--hostname") {
        flag_hostname = true;
        if(argc > i + 1) {
          flag_hostname_name = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo --hostname [HOSTNAME]\n";
          exit(0);
        }
      }

      else if(arg == "--reset-hostname") {
        ChangeHostname(default_hostname);
      }

      else if(arg == "-k" || arg == "--keep") {
        flag_keep = true;
      }

      else if(arg == "-t" || arg == "--ttl") {
        flag_ttl = true;
        if(argc > i + 1) {
          flag_ttl_value = argv[i + 1];
        } else {
          cout << "\033[31m Error:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo --ttl [VALUE]\n";
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

  if(flag_manual) {
    MACChange(flag_manual_address);
  }

  if(flag_permanent) {
    string permMAC = ShowMAC("ip link | awk '/ether/ {print $6}'");
    MACChange(permMAC);
  }

  if(flag_vendor) {
    WithVendor(flag_vendor_name);
  }

  if(flag_hostname) {
    ChangeHostname(flag_hostname_name);
  }

  if(flag_keep) {
    string keepMAC = ShowMAC("ip link | awk '/ether/ {print $2}'", true) + MACGenerator(true);
    MACChange(keepMAC);
  }

  if(flag_ttl) {
    ChangeTTL(flag_ttl_value);
  }

  return 0;
}
