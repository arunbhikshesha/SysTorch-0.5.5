*** Fragment, not fully tested! ***
### Requirements
- in Windows 11:
  * install latest usbipd-win from https://github.com/dorssel/usbipd-win/releases

- in WSL2 install usbip tools:
  * ``apt install -y linux-tools-virtual hwdata``
  * ``update-alternatives --install /usr/local/bin/usbip usbip `ls /usr/lib/linux-tools/*/usbip | tail -n1` 20``

### Share a USB port with WSL2 (to repeat after each USB dis-/re-connect):
- in Windows 11 PowerShell:
  * ``usbipd wsl list``, output e.g.: ``5-1  0483:3748  STM32 STLink    Not attached``
  * ``usbipd wsl attach --busid 5-1``, output from wsl list then: ``5-1  0483:3748  STM32 STLink  Attached - Ubuntu-22.04``
- then in WSL2:
  * test: ``lsusb``, output e.g.: ``Bus 001 Device 002: ID 0483:3748 STMicroelectronics ST-LINK/V2``
  * start VSCode with Microcontroller Project in Dev Container, available STLink tools here: ``st-flash, st-info, st-trace st-util``
  * test in VSCode bash console: ``st-info --probe --connect-under-reset``, (ST-LINK LED = green), output e.g.: ``Found 1 stlink programmers version: V2J28S7 ... serial: ... flash:  524288 (pagesize: 2048) chipid:  0x469  dev-type:  STM32G47x_G48x``

### Linux Docker container additionally:
````
# install debug tools
RUN apt install --no-install-recommends -y software-properties-common && add-apt-repository -y ppa:deadsnakes/ppa \
    && apt install --no-install-recommends -y build-essential libusb-1.0-0 libusb-1.0-0-dev libncursesw5 python3.8
# build and install stlink toolset (develop branch 2023-02-14)
RUN git clone --single-branch -b develop https://github.com/stlink-org/stlink.git && cd stlink && git checkout c72175133caef1bdfd21d33e93763f1aa5dac3f5 \
    && make clean && make release && make install
````

- for VSCode in devcontainer.json: `` "runArgs": [  "--privileged"  ], ``
