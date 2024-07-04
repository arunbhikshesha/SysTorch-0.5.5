## S4_Config: Collection of configuration files/ interface definitions for S4 firmware  
- FPGA/FPGA_Register_Definition.xlsm (for SPI communication S4_Main <-> FPGA): excel file with VBA macros for generation of files IPC_A7_M4/ipc_fpga_io.h, deploy/config/FPGA_Out-Init.json and S4_DiagApp/FPGA_InOut.json  
- api/: Protocol Buffers and header definitions for gRPC and Shared-memory interfaces
- IPC_A7_M4/: shared memory interface definition A7 <-> M4 core  
- deploy/config/: init/config files for S4_Main and other apps, get copied to ``/usr/share/s4-config/config`` on MaPro  
- deploy/wld/: basic welding program files for S4_Main, get copied to ``/usr/share/s4-config/wld`` on MaPro  
- S4_DiagApp/: JSON config files for diagnostic web app  
- Parameter/: HMI related definitions (Job parameters, 'Doku' parameters)
- Processdata/: Definitions for in S4_Main collected process data (M4, S4_Main, Primary, Wire-feeder, Remote-control..)
- systorch/: init/config files for S4_SystemTorch app

