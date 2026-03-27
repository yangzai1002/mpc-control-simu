#ifndef CUART_H
#define CUART_H
#include <stdio.h>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <linux/spi/spidev.h>
#include <termios.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/mman.h> //mmap head file
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#define FALSE -1
#define TRUE 0

class CUart
{
public:
    CUart()
    {
    }
    virtual ~CUart()
    {

        closeUart(mFd);
    }

    virtual int openUart(int fd, const char *port);
    virtual void closeUart(int fd);
    virtual int setUart(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
    virtual int initUart(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);
    virtual int recvUart(int fd, char *rcv_buf, int data_len);
    virtual int sendUart(int fd, const char *send_buf, int data_len);
    virtual int recvUartNonBlk(int fd, char *rcv_buf, int data_len);

public:
    int mFd;
};

#endif
