# How to build Libsai
1. Install QEMU with rocker software switch enabled. We recommend to build the latest version retrieved from the [git repository](http://git.qemu.org/qemu.git).
2. Install any linux distribution you like in QEMU. We use Ubuntu 15.04 server for development.
3. Replace the linux kernel of the guest OS by one with the following config enabled:
  - PCI (normally enabled by default)
  - BRIDGE (normally enabled by default)
  - NET_SWITCHDEV (normally _NOT_ enabled by default, you should edit .config file)
  - NET_VENDOR_ROCKER (normally _NOT_ enabled by default, you should edit .config file)
4. Let's refer the guest OS as "switch VM". Copy switch VM and make three more VMs, that will be refered "VM1", "VM2" and "VM3" hereafter.
5. Allow QEMU to use bridges. Add following lines into `/etc/qemu/bridge.conf` of the host OS.

    ```
    allow br0
    allow br1
    allow br2
    ```

6. Boot the switch VM with user mode network enabled, clone this repository and make it.

    ```
    host-OS $ qemu-system-x86-64 [image of switch VM] -net nic -net user
    switch-VM $ sudo apt-get install gcc make # or yum or ports or whatever
    switch-VM $ git clone https://github.com/osrg/libsai
    switch-VM $ git clone https://github.com/opencomputeproject/OCP-Networking-Project-Community-Contributions OCP
    switch-VM $ export SAIDIR=/path/to/OCP/sai
    switch-VM $ cd libsai && make
    ````
