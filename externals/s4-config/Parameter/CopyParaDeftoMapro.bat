set IP_ADDRESS=192.168.113.1

ssh -o stricthostkeychecking=no root@%IP_ADDRESS% "rm -r /usr/share/s4/backend/ParaDef.json"
scp -o stricthostkeychecking=no .\ParaDef.json root@%IP_ADDRESS%:/usr/share/s4/backend/ParaDef.json
pause

