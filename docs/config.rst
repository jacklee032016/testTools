Configuration of Moxa EDS-405A PTP
##########################################


Basic configuration
===============================
::

    username:admin/user; password:none 
    IP address: 192.168.168.2
    Enable PTP: Basic Settings/Time/PTP Settings
	    Select `E2E v2 BC` mode;
	DIP switch: Turbo Ring: OFF; otherwise can't ping it;	


Connect
-----------------
3 options:
#. Moxa and PC both connect to router: 192.168.168.1;	
#. Moxa and PC both connect to switch: 192.168.168.36;
#. PC connect to one port of Moxa directly, Moxa connect to router or switch;
	
Testing
-----------------
::

    tcpdump -n src 192.168.168.2: find send packet to 224.0.1.129:319/320;
	
	./Linux.bin.X86/bin/ptp4l -i eth0 -q -m -S -s
	    -q -m : debug info on console;
		-S: software timestamp;
		-s: slave mode;
		default:
		    E2E, delay request-response
			UDP IPV4

    sudo ./Linux.bin.X86/bin/phc2sys -a -r
