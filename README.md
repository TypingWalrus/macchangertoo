# macchangertoo

Yet another tool which changes device's MAC address.

---
### **About**

This MAC changer tool offers more than any other tool you have been using (give it a try) but works only on Linux. With it you can: **Randomize your MAC address, Change it manually, Reset it back to permanent, Set the MAC address to known vendors, Change the hostname and Reset it back**

While you can fully spoof your device on home networks, it won't work on **public** networks because of WIDS and WIPS.

---
### **Requirements**

* Should work on any **Linux** system which uses **NetworkManager**.
* **Packages**: make, g++, git.

---
### **Installation**

* Run:
```bash
git clone https://github.com/TypingWalrus/macchangertoo
cd macchangertoo/main
```

* For it to work properly, before compiling change these *main.cpp* lines (12-14) depending on the system you are using:
```cpp
// defaults
string interface = "wlan0";
string default_hostname = "";
```
* Set the *interface* to your interface (if you use multiple choose the one you are most frequently using) in double quotation marks as you wouldn't need to retype it over and over again with the *-i* flag.
* Set the *default_hostname* to your current hostname (if you don't know what it is just paste the output of "hostname" command) in double quotation marks as you could revert it back.

* Compile it:
```bash
make
```

* For it to be accessible anywhere on the system:
```bash
mv macchangertoo /bin
```

---
Thank you for visiting!
