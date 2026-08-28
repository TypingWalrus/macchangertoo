#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <algorithm>
#include <random>

// including headers
#include "defaults.hpp"

using namespace std;

// defaults
string interface = interface_default;
string default_hostname = default_hostname_default;

bool flag_random, flag_manual, flag_permanent, flag_hostname, flag_keep, flag_ttl, flag_debug;
string flag_manual_address, flag_hostname_name, flag_ttl_value;

// When debug mode is on, every command is shown
void CheckDebug(string com) {
  if(flag_debug) {
    cout << "\033[31m" << com << "\033[0m";
  }
}

// Generates random MAC addresses
string MACGenerator(bool known_vendor = false) {
  random_device rd;
  mt19937 g(rd());
  uniform_int_distribution<> dist(0, 15);

  string ChooseFrom = "abcdef0123456789";
  string MAC;

  if(!known_vendor) {
    for(int i = 0; i < 12; i++) {
      int rand = dist(g);
      if(i == 0 && isdigit(ChooseFrom[rand]) && ChooseFrom[rand] % 2 != 0) {
        ChooseFrom[rand]--;
      }
      if(i == 1) {
        int value = stoi(string(1, ChooseFrom[rand]), nullptr, 16);
        if(value % 2 != 0) {
          ChooseFrom[rand] = "0123456789abcdef"[value - 1];
        }
      }

      if(i % 2 == 0 && i != 0) {
        MAC += ":";
      } MAC += ChooseFrom[rand];
    }
  } else {
    for(int i = 0; i < 6; i++) {
      int rand = dist(g);
      if(i % 2 == 0 && i != 0) {
        MAC += ":";
      } MAC += ChooseFrom[rand];
    }
  }

  return MAC;
}

// Shows version
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
  cout << "Version: 1.13 - no OUIs\n";
  exit(0);
}

// Help page
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
  cout << "\033[33m--hostname\033[0m         Change hostname (will show up as that device)\n";
  cout << "\033[32m                     macchangertoo --hostname [HOSTNAME]\033[0m\n";
  cout << "\033[33m--reset-hostname\033[0m   Reset hostname\n";
  cout << "\033[33m-k\033[0m,\033[33m --keep\033[0m         Keep the same vendor bytes\n";
  cout << "\033[33m-t\033[0m,\033[33m --ttl\033[0m          Modify TTL (Android,MacOS,Linux=64, Windows=128)\n";
  cout << "\033[32m                     macchangertoo --ttl [TTL]\033[0m\n";
  cout << "\033[33m--debug\033[0m            Debug\n";
  cout << "\nIf you encounter any bugs: \033[34mhttps://github.com/TypingWalrus/macchangertoo\033[0m\n";
  exit(0);
}

// Shows specified MAC address
string ShowMAC(bool perm, string command, bool only_vendor = false) {
  CheckDebug(command);

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

  // Fix
  if(perm) {
    string check = "ip link | awk '/ether/ {print $5}'";

    CheckDebug(check);

    FILE* com_perm = popen(check.c_str(), "r");
    char buf_perm[128];
    string output;

    while(fgets(buf_perm, sizeof(buf_perm), com_perm) != nullptr) {
      output += buf_perm;
    } pclose(com_perm);
    if(output != "permaddr\n") {
      mac = ShowMAC(false, "ip link | awk '/ether/ {print $2}'");
    }
  }

  return mac;
}

// Changes MAC address
void MACChange(string mac = "random", bool perm = false) {
  if(mac == "random") {
    mac = MACGenerator();
  }

  string command = "sudo ip link set dev " + interface + " down && sudo ip link set dev " + interface + " address " + mac + " && sudo ip link set dev " + interface + " up";

  CheckDebug(command);

  cout << "\033[33mPermanent MAC:\033[0m " << ShowMAC(true, "ip link | awk '/ether/ {print $6}'");
  cout << "\n\033[33mPrevious MAC:\033[0m " << ShowMAC(false, "ip link | awk '/ether/ {print $2}'");

  // checking if the MAC address is valid
  int status = system(command.c_str());
  if(status != 0) {
    if(perm) {
      cout << "\n\033[31mError:\033[0m Already set\n";
      string set_up = "sudo ip link set dev " + interface + " up";

      CheckDebug(set_up);

      system(set_up.c_str());

      exit(0);
    }
    cout << "\n\033[31mError:\033[0m Invalid MAC address\n";
    string set_up = "sudo ip link set dev " + interface + " up";

    CheckDebug(set_up);

    system(set_up.c_str());

    exit(0);
  }

  cout << "\n\033[32mCurrent MAC:\033[0m " << mac << "\n";
}

// Extras
// Changes hostname
void ChangeHostname(string name) {
  string command = "sudo nmcli general hostname " + name + " && sudo service NetworkManager restart";

  CheckDebug(command);

  system(command.c_str());

  cout << "\033[32mHostname:\033[0m " << name << '\n';
}

// Changes TTL
void ChangeTTL(string value) {
  string command = "sudo sysctl -w net.ipv4.ip_default_ttl=" + value;

  CheckDebug(command);

  FILE* com = popen(command.c_str(), "r");
  pclose(com);
  cout << "\033[32mCurrent TTL:\033[0m " << value << '\n';
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
        if(argc > i + 1) {
          i++;
          flag_manual_address = argv[i];
        } else {
          cout << "\033[31mError:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -m [MAC]\n";
          exit(0);
        }
      }

      else if(arg == "-i" || arg == "--interface") {
        if(argc > i + 1) {
          i++;
          interface = argv[i];
        } else {
          cout << "\033[31mError:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo -i [INTERFACE] -r\n";
          exit(0);
        }
      }

      else if(arg == "-s" || arg == "--show") {
        cout << "\033[33mCurrent MAC:\033[0m " << ShowMAC(false, "ip link | awk '/ether/ {print $2}'") << "\n";
      }

      else if(arg == "-p") {
        flag_permanent = true;
      }

      else if(arg == "--hostname") {
        flag_hostname = true;
        if(argc > i + 1) {
          i++;
          flag_hostname_name = argv[i];
        } else {
          cout << "\033[31mError:\033[0m Too few arguments.\n";
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
          i++;
          flag_ttl_value = argv[i];
        } else {
          cout << "\033[31mError:\033[0m Too few arguments.\n";
          cout << "Try: \033[33m macchangertoo --ttl [VALUE]\n";
          exit(0);
        }
      }

      else if(arg == "--debug") {
        flag_debug = true;
      }

      else {
        cout << "\033[31mError: \033[0mUnknown option:\033[33m " << argv[i] << "\n\n";
        Help();
      }
    }
  } else {
    Help();
  }

  if(!flag_debug) {
    freopen("/dev/null", "w", stderr);
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
    string permMAC = ShowMAC(false, "ip link | awk '/ether/ {print $6}'");
    MACChange(permMAC, true);
  }

  if(flag_hostname) {
    ChangeHostname(flag_hostname_name);
  }

  if(flag_keep) {
    string keepMAC = ShowMAC(false, "ip link | awk '/ether/ {print $2}'", true) + MACGenerator(true);
    MACChange(keepMAC);
  }

  if(flag_ttl) {
    ChangeTTL(flag_ttl_value);
  }

  return 0;
}
