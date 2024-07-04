# Ports used

| Port  | Service                         | Comment                                                 |
| ----- | ------------------------------- | ------------------------------------------------------- |
| 80    | Nginx web server                | Browser e.g.: MaPro-IP/s4-diag                          |
| 443   | Nginx web server HTTPS          |                                                         |
| 1534  | tcf-agent                       |                                                         |
| 5355  | systemd-resolve                 |                                                         |
| 9080  | gRPC Webproxy                   | Interface S4_DiagApp <-> gRPC server                    |
| 9095  | S4_Main debug gdb server        |                                                         |
| 19999 | Netdata                         | Browser: Netdata System Overview                        |
| 50000 | s4-rest-service                 | Interface to HMI                                        |
| 50001 | s4-backend                      | gRPC Port                                               |
| 50053 | s4-main                         | gRPC Port                                               |
| 2244  | fluent-bit                      | TCP Input Port for basic logging                        |
| 2245  | fluent-bit                      | TCP Input Port for logs from errormanager and eventlog  |
| 9514  | fluent-bit                      | syslog Output Port                                      |
