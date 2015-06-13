# Libsai
Libsai is a small SAI implementation for testing SAI applications.

While SAI provides general APIs for switch applications and vendors can have their own SDKs running with SAI and their switches, there is no way to
 - Easily test SAI applications
 - Without using real SAI-capable switches

Our goal is to mitigate these issues by providing open sourced SAI implementation that leverage software switches.

## How to build
Please refer to [Building.md](Building.md)

## How to run
Before staring, make sure you finished everything in the "How to build" section.

1. Run the switch VM

    ```
    host-OS $ sudo qemu-system-x86-64 [image of switch VM] -device rocker,name=sw1,len-ports=4,ports[0]=dev0,ports[1]=dev1,ports[2]=dev2 -netdev bridge,br=br0,id=dev0 -netdev bridge,br=br1,id=dev1 -netdev bridge,br=br2,id=dev2
    ```

2. Run the three other VMs

    ````
    host-OS $ qemu-system-x86-64 [image of VM1] -net nic,macaddr=52:54:00:00:00:11 -netdev bridge,br=br0
    host-OS $ qemu-system-x86-64 [image of VM2] -net nic,macaddr=52:54:00:00:00:12 -netdev bridge,br=br1
    host-OS $ qemu-system-x86-64 [image of VM3] -net nic,macaddr=52:54:00:00:00:13 -netdev bridge,br=br2
    ````

3. Execute libsai in the switch VM

    ````
    switch-VM $ cd libsai
    switch-VM $ sudo ./a.out
    ````

At this point, the three VMs are connected with a topology described in the below figure.

## Copyright
Copyright (C) 2015 [Nippon Telegraph and Telephone Corporation](http://www.ntt.co.jp/index_e.html). Released under [Apache License 2.0](LICENSE).
