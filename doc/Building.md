# How to build Libsai
1. Install QEMU with rocker software switch enabled. We recommend to build the latest version retrieved from the [git repository](http://git.qemu.org/qemu.git). No special build option is required to enable rocker device.
2. Install [Ubuntu 15.04 server](http://releases.ubuntu.com/vivid/) as a guest OS (any other linux distribution might work, if you have any special reason to use one).
3. Boot the guest OS and replace the kernel by the one with rocker driver enabled.

    ```
    host-OS$ qemu-system-x86-64 [image name] -net nic -net user
    guest-OS$ git clone https://github.com/osrg/libsai
    guest-OS$ cd libsai/linux
    guest-OS$ sudo dpkg -i linux-image.deb
    guest-OS$ sudo shutdown -hP now
    ```

 - The differences of the configs in the kernel installed above and original Ubuntu 15.04 are:
  - CONFIG_NET_SWITCHDEV=y
  - CONFIG_NET_VENDOR_ROCKER=y
  - CONFIG_ROCKER=m
4. Let's refer the guest OS as "switch VM". Copy switch VM and make three more VMs, that will be refered "VM1", "VM2" and "VM3" hereafter.
5. Allow QEMU to use bridges. Add following lines into `/etc/qemu/bridge.conf` of the host OS.

    ```
    allow br0
    allow br1
    allow br2
    ```

6. Create bridges on the host OS. Note that they must be re-created after the host OS is rebooted.

    ```
    host-OS$ sudo apt-get install bridge-utils
    host-OS$ sudo brctl addbr br0
    host-OS$ sudo brctl addbr br1
    host-OS$ sudo brctl addbr br2
    ```

7. Boot the switch VM and build libsai.

    ```
    host-OS$ qemu-system-x86-64 [image of switch VM] -net nic -net user
    switch-VM$ sudo apt-get install gcc make
    switch-VM$ git clone https://github.com/opencomputeproject/OCP-Networking-Project-Community-Contributions OCP
    switch-VM$ export SAIDIR=/path/to/OCP/sai
    switch-VM$ cd libsai && make
    ````
