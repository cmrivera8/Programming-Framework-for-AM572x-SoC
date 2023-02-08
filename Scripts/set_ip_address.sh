#!/bin/bash

# Show partition table
sudo lsblk 

# Get the desired partition and new ip from the user
echo ""
read -p "Enter the desired partition (example sdc2): " partition

echo ""
read -p "Enter the desired ip for the target eth0 (example 192.168.137.8): " ip

# Create the directory /mnt/sdcard if it does not exist
if [ ! -d "/mnt/sdcard" ]; then
  sudo mkdir /mnt/sdcard
fi

# Mount the partition on /mnt/sdcard
sudo mount /dev/$partition /mnt/sdcard

# Modify the file /mnt/sdcard/etc/network/interfaces
content=$(cat << EOL
# /etc/network/interfaces -- configuration file for ifup(8), ifdown(8)
 
# The loopback interface
auto lo
iface lo inet loopback

# Wireless interfaces
iface wlan0 inet dhcp
    wireless_mode managed
    wireless_essid any
    wpa-driver wext
    wpa-conf /etc/wpa_supplicant.conf

iface tiwlan0 inet dhcp
    wireless_mode managed
    wireless_essid any

iface atml0 inet dhcp

# Wired or wireless interfaces
auto eth0
iface eth0 inet static
        address ${ip}
        netmask 255.255.255.0
#        pre-up /bin/grep -v -e "ip=[0-9]\+\.[0-9]\+\.[0-9]\+\.[0-9]\+" /proc/cmdline > /dev/null
#        udhcpc_opts -R -b

iface eth1 inet dhcp
iface eth2 inet dhcp
iface eth3 inet dhcp
iface eth4 inet dhcp

# Ethernet/RNDIS gadget (g_ether)
# ... or on host side, usbnet and random hwaddr
iface usb0 inet dhcp

# Bluetooth networking
iface bnep0 inet dhcp
EOL
)

#"sudo" in front of a redirection operator like >, it only applies to the command before the redirection operator, not to the redirection itself. In other words, the shell still runs as a regular user and does not have permission to write to the file.
#This is why the owneship of the file is changed temporarily: 

sudo chown $(whoami) /mnt/sdcard/etc/network/interfaces
echo "$content" > /mnt/sdcard/etc/network/interfaces
sudo chown root /mnt/sdcard/etc/network/interfaces

# Create the file /mnt/sdcard/etc/rc.local
sudo bash -c 'cat > /mnt/sdcard/etc/rc.local << EOL
#!/bin/sh -e
#
# rc.local
#
# This script is executed at the end of each multiuser runlevel.
# Make sure that the script will "exit 0" on success or any other
# value on error.
#
# In order to enable or disable this script just change the execution
# bits.
#
# By default this script does nothing.

/etc/init.d/networking restart

exit 0
EOL'

# Give the file /mnt/sdcard/etc/rc.local execute permissions
sudo chmod +x /mnt/sdcard/etc/rc.local

# Unmount the directory /mnt/sdcard
sudo umount /mnt/sdcard
