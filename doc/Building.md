# How to build Libsai
1. Download QEMU source code from the [git repository](http://git.qemu.org/qemu.git) and build it.
 - We strongly recommend this because packaged versions from distributions most probably do not yet support the software switch we use (rocker).
 - No special build option is required.
2. Install [Ubuntu 15.04 server](http://releases.ubuntu.com/vivid/) as a guest OS.
 - We recommend the simplest installation, with no LVM and all directories in the same partition.
 - If it takes unexpectedly long time, make sure that you have KVM support.
    - Check your BIOS/UEFI setting to see if hardware virtualization support is enabled (some vendors disable it by default for security reasons).
    - Check if kvm kernel module is installed by `lsmod | grep kvm`. If this yields no output, try `sudo apt-get install kvm`.
    - Add `-enable-kvm` option on your QEMU execution command line.
 - Any other linux distribution _might_ work, if you have any strong reason to use one.
3. Boot the guest OS and replace the kernel by the one with rocker driver enabled.

    ```
    host-OS$ qemu-system-x86-64 [image name] -net nic -net user -enable-kvm
    guest-OS$ git clone https://github.com/osrg/libsai
    guest-OS$ cd libsai/linux
    guest-OS$ sudo dpkg -i linux-image.deb
    guest-OS$ sudo shutdown -hP now
    ```
 The differences of the configs in the kernel installed above and original Ubuntu 15.04 are:
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
    host-OS$ qemu-system-x86-64 [image of switch VM] -net nic -net user -enable-kvm
    switch-VM$ sudo apt-get install gcc make
    switch-VM$ git clone https://github.com/opencomputeproject/OCP-Networking-Project-Community-Contributions OCP
    switch-VM$ export SAIDIR=/path/to/OCP/sai
    switch-VM$ cd libsai && make
    ````
