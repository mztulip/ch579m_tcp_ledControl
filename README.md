# ch579m_tcp_ledControl

Simple tcp server on ch579m board with lwip usage.
It allows to enable and disable onboard LED.

Compile:
```
chmod +x make.sh(if necessary)
make.sh
```

Led can be controlled using UDP packet.
On Linux machine packets can be send as below:
```
echo -n "1" > /dev/udp/192.168.2.235/8001
echo -n "0" > /dev/udp/192.168.2.235/8001
```

Compiled using:
arm-none-eabi-gcc (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)[Link](https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2?rev=78196d3461ba4c9089a67b5f33edf82a&hash=5631ACEF1F8F237389F14B41566964EC)
GNU bash, version 5.2.21(1)-release (x86_64-pc-linux-gnu)

![alt text](https://github.com/mztulip/ch579m_ledBlink_gcc/blob/main/eval.png?raw=true)

Board can be programmed over USB cable using https://github.com/ch32-rs/wchisp.
```
wchisp flash output.elf
```

Project is based on another project with lwip port for ch579: https://gitee.com/maji19971221/lwip-routine.
Project uses lwip: https://savannah.nongnu.org/projects/lwip/
