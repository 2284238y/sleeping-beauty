#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "blucontrol.h"

using namespace std;

BLU::BLU()
{
	// set local and remote addresses to 0
	struct sockaddr_rc loc_addr, rem_addr;
    socklen_t opt = sizeof(rem_addr);

    // activate automatic bluetooth pairing through command line
    // not super safe, but will do for now
    // maybe look at implementing this in a script using bluetoothctl (https://stackoverflow.com/questions/34709583/bluetoothctl-set-passkey)
    system("sudo hciconfig hci0 piscan");

        // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    str2ba(pi_addr, &loc_addr.rc_bdaddr);
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "Accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));
}

int BLU::dataAvailable()
{
	memset(buf, 0, sizeof(buf));
	bytes_read = read(client, buf, sizeof(buf));

	if(bytes_read>0)
	{
		return 1;
	} else 
	{
		return 0;
	}
}

void BLU::getData(char* buf_out)
{
	// copies bytes read to output buffer
	memcpy(buf_out, buf, sizeof(buf_out));
}
