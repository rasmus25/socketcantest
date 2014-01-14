//#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <stdint.h>
#include <iostream>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define MOTOR_ID 5

using namespace std;

namespace {
int s;
int nbytes;
struct sockaddr_can addr;
struct can_frame frame;
struct ifreq ifr;

char *ifname = "can0";

}

bool setupCan() {
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		//		perror("Error while opening socket");
		return false;
	}
	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	//	printf("%s at index %d\n", ifname, ifr.ifr_ifindex);

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		//		perror("Error in socket bind");
		return false;
	}
	return true;
}

void configureMotor(int id, int32_t acceleration, int32_t deceleration) {
	frame.can_id = 0x300 + id;
	frame.can_dlc = sizeof(acceleration) + sizeof(deceleration);
	for (int byte = 0; byte < sizeof(acceleration); byte++)
		frame.data[byte] = uint8_t(acceleration >> (byte * 8));
	for (int byte = 0; byte < sizeof(deceleration); byte++)
		frame.data[byte + sizeof(acceleration)] = uint8_t(deceleration >> (byte
				* 8));

	nbytes = write(s, &frame, sizeof(struct can_frame));

	//	printf("Wrote %d bytes\n", nbytes);
}
void setSpeed(int id, int32_t speed) {
	frame.can_id = 0x400 + id;
	frame.can_dlc = sizeof(speed) * 2;
	for (int byte = 0; byte < sizeof(speed); byte++)
		frame.data[byte] = uint8_t(speed >> (byte * 8));
	for (int byte = 0; byte < sizeof(speed); byte++)
		frame.data[byte + sizeof(speed)] = uint8_t(speed >> (byte * 8));

	nbytes = write(s, &frame, sizeof(struct can_frame));

}

int main(void) {

	if (!setupCan())
		return -1;
	configureMotor(MOTOR_ID, 100, 100);
	while (true) {
		int newspeed;
		cin >> newspeed;
		if (newspeed == 0)
			break;
		setSpeed(MOTOR_ID, newspeed);
	}
	return 0;
}

