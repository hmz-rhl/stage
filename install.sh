cat logo;
sudo apt-get update;
sudo apt-get upgrade;
cd lib/;
make;
make install;
cd ../expander/;
make;
make install;
cd ../hubload/;
sudo gcc hubload.c -o hubload_daemon_C -lmosquitto -lpthread -lwiringPi -lHubload;
sudo service hubload_daemon_C stop;
sleep 7;
sudo cp hubload_daemon_C /opt/hubload/scripts/;
sudo cp hubload_daemon_C.service /etc/systemd/system/;
sudo systemctl enable hubload_daemon_C.service;
sudo service hubload_daemon_C start;
sleep 7;
sudo reboot;