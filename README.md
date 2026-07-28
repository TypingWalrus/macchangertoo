# macchangertoo

Yet another tool which changes device's MAC address.

---
### **About**

This MAC changer tool offers more than any other tool you have been using (give it a try) but works only on Linux. With it you can: **Randomize your MAC address, Change it manually, Reset it back to permanent, Set the MAC address to known vendors, Change the hostname and Reset it back**

While you can fully spoof your device on home networks, it won't work on **public** networks because of WIDS and WIPS.

---
### **Requirements**

* Should work on any **Linux** (even on **Android** (with sudo configured)) system which uses **NetworkManager**.
* **Packages**: make, g++, git, awk.

---
### **Installation**

* Run:
```bash
git clone https://github.com/TypingWalrus/macchangertoo
cd macchangertoo/main

make
```

* For it to be accessible anywhere on the system:
```bash
sudo mv macchangertoo /bin
```

---
Thank you for visiting!
