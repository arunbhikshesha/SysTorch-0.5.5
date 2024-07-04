## Cross development environment (Windows 10/11):
- Visual Studio Code [Download](https://code.visualstudio.com/)
- [CMake](https://cmake.org/download/): Install latest cmake-x.xx.x-win64-x64.msi with default settings, choose option *"Add CMake to the system PATH for all users"*!
- Copy file "**ninja.exe**" from latest ninja-win.zip on [GitHub](https://github.com/ninja-build/ninja/releases) to a directory in PATH, for instance to ``C:\Program Files\CMake\bin`` (a.k.a. C:\Programme\ ..).
- Install the latest [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), confirm all default settings.
- Install [Git for Windows](https://gitforwindows.org/) (git bash required by some shell scripts), confirm all default settings.

- In VSCode install at least required extensions ([also see file ``.vscode/extensions.json``](.vscode/extensions.json)):
  - CMake Tools (Microsoft)
  - CMake (twxs)
  - C/C++ (Microsoft)
  - Cortex-Debug (marus25)
  - Remote - Development incl. Remote - WSL (Microsoft).

- in VSCode write ***your*** installation path and MaPro board IP settings in ***User***(!)- settings.json:
  -> (``F1``), then type in ``Open User Settings (JSON)``, then paste there:
    ```
    "terminal.integrated.env.windows": {
        "ARMGCC_DIR": "C:/ST/STM32CubeIDE_1.12.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.200.202301161003/tools",
        "STM32CubeProgrammerCLI": "C:/ST/STM32CubeIDE_1.12.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_2.0.600.202301161003/tools/bin/STM32_Programmer_CLI.exe"
    },
    "cmake.environment": {
        "MAPRO_BOARD_IP": "192.168.1.13",
        "ARMGCC_DIR_WINDOWS": "C:/ST/STM32CubeIDE_1.12.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.200.202301161003/tools",
    },
    "cortex-debug.stlinkPath.windows": "C:/ST/STM32CubeIDE_1.12.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.0.500.202301161003/tools/bin/ST-LINK_gdbserver.exe",
    "cortex-debug.armToolchainPath.windows": "C:/ST/STM32CubeIDE_1.12.1/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.10.3-2021.10.win32_1.0.200.202301161003/tools/bin",
    ```
- JTAG debug tested with ST-Link/V2 adapter

## Alternatively: "Dockerized" Dev. Environment in VS Code (Linux/WSL2):
**Requirements:**
- [Docker installed](https://docs.docker.com/engine/install/) (Tested with Docker Desktop for Windows and WSL 2 backend)
- VSCode mit Extension "Remote - Development incl. Remote - WSL (Microsoft)" (ms-vscode-remote.vscode-remote-extensionpack) installed
- Git-clone the project on a Linux/WSL2 drive. When first open project folder with VSCode, on question *"Folder contains a Dev Container configuration file. Reopen folder to develop in a container"* click "Reopen in Container".

**Limitations:**
- Debugging with USB-JTAG adapter on WSL2/ docker container not mature yet. See [DebugInWSL2](DebugInWSL2.md).

## Build and Run (Windows 10/11 or Linux container):
- Git-clone the project into working directory on a Windows (or Linux/WSL2 for dockerized environment!) drive and open the project folder with Visual Studio Code
- CMake Configure: Press F1 -> choose command "*CMake: Delete Cache and Reconfigure*"
- in VSCode Status Bar choose "active kit" = "Lorch GCC ARM-Cortex-M" (if not listed choose "Search for kits" first)
- in VSCode Status Bar choose "current build variant" = **Debug** (or Release)
- in VSCode Status Bar set "active target to build" to:
  - **build**: for only building the firmware files (and e.g. flash and run/debug the .elf file later)
  - **build_and_encrypt**: to build, encrypt and assemble the Firmware package including the .info file
  - **build_and_run**: for build + install package to MaPro + flash the firmware via CAN bus onto the target
- Click "*Build*" in the VSCode Status Bar to actually run the selected Build option.
- To compile only: Click Icon "Compile Active File" top right (only possible after first successful build)
- To manually build/ encrypt/ bundle: call the corresponding scripts in ``./externals/s4-config/peripherie/tools/`` from the project root dir (use a **Git Bash** terminal on Windows!).
- To install or upgrade ***externals*** run ``yarn install`` or ``yarn upgrade`` resp.
  - Yarn installation: [Linux](https://classic.yarnpkg.com/en/docs/install#debian-stable) / [Windows](https://classic.yarnpkg.com/en/docs/install#windows-stable) (Yarn already installed in the docker Dev Container!)

## JTAG Flash/ Debug (Windows 10/11):
- to flash the (*not encrypted*) firmware via JTAG adapter directly onto the target and run it press Ctrl+Shift+B (= ``Terminal -> Run Build Task...``)
- to debug the (*not encrypted*) firmware via JTAG adapter press F5 (= Run->Start Debugging) while keeping the **Debug** option active. - [VSCode Debugging tips](https://code.visualstudio.com/docs/editor/debugging)
  Press Shift+F5 or "Stop/ Disconnect" in the Debug Context Menu to end debugging.
- to flash the bootloader (*with encryption, dev. key*) via JTAG choose ``Terminal -> Run Task... -> CubeProg: Flash Bootloader (SWD)``. The corresponding device firmware (*with encryption, dev. key*) can now be deployed and tested on the system via the **build_and_run** CAN flash step described above.

## General:
- after certain changes (*build* directory deleted, new or removed source files etc.) or to refresh the Build-time/'K_VERSION' variable the **CMake configure** step has to be re-run:
  Press F1 -> choose command "*CMake: Delete Cache and Reconfigure*" !
- Clickable compile warnings/ errors are listed in tab "PROBLEMS" at the bottom..
- The C/C++ extension supports source code formatting using clang-format:
  - *Format Document*: (``Shift+Alt+F``) to format the entire file,
  - *Format Selection*: (``Ctrl+K Ctrl+F``) or select from right-click context-menu,
  - Lorch Coding Style rules set in workspace in file *.clang-format*,
  - *format-on-save* (possibly for *modifications only*) and *format-on-type* activated!
