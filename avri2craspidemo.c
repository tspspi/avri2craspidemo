#include <sys/cdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <dev/iicbus/iic.h>

#define IIC_DEVICE "/dev/iic1"

/*
        This method provides a simple way
        of "scanning" an I2C bus by checking
        on which adresses devices exist. Note that
        since there is no real auto discovery on I2C
        this MAY trigger actions on some devices that
        are not desired - it's realized just as a write
        followed by a read.
*/
static void scanI2CBus(int fd) {
        unsigned int i;
        uint8_t buf[2] = { 0, 0 };
        struct iic_msg msg[2];
        struct iic_rdwr_data rdwr;

        msg[0].flags = !IIC_M_RD;
        msg[0].len   = sizeof(buf);
        msg[0].buf   = buf;

        msg[1].flags = IIC_M_RD;
        msg[1].len  = sizeof(buf);
        msg[1].buf  = buf;

        rdwr.nmsgs = 2;

        for(i = 1; i < 128; i++) {
                // Set address
                msg[0].slave = i;
                msg[1].slave = i;

                rdwr.msgs = msg;
                if(ioctl(fd, I2CRDWR, &rdwr) >= 0) {
                        // Success - we have found a device
                        printf("Device found: %02x\n", i);
                }
        }
}

static char i2cRead(int fd) {
        uint8_t buf[1];

        struct iic_msg msg[1];
        struct iic_rdwr_data rdwr;

        msg[0].slave = 0x14 << 1; // Assuming the code above for the AVR has been used with address 0x14
        msg[0].flags = IIC_M_RD;
        msg[0].len   = sizeof(buf);
        msg[0].buf   = buf;

        rdwr.msgs = msg;
        rdwr.nmsgs = 1;

        if(ioctl(fd, I2CRDWR, &rdwr) < 0)  {
                perror("I2CRDWR");
                return(0xFF);
        }

        return buf[0];
}

static void i2cSend(int fd, char value) {
        uint8_t buf[1];
        struct iic_msg msg;
        struct iic_rdwr_data rdwr;

        buf[0] = value;
        msg.slave = 0x14 << 1; // Assuming the code above for the AVR has been used with address 0x14
        msg.flags = 0;
        msg.len   = sizeof( buf );
        msg.buf   = buf;

        rdwr.msgs = &msg;
        rdwr.nmsgs = 1;

        if (ioctl(fd, I2CRDWR, &rdwr) < 0)  {
                perror("I2CRDWR");
        }
}


int main ( int argc, char **argv )  {
        int fd;

        if ((fd = open(IIC_DEVICE, O_RDWR)) < 0 )  {
                perror("open");
                return -1;

        }

        scanI2CBus(fd);

        printf("\n\nDoing some read and write tests\n\n");

        i2cSend(fd, 0x08);
        printf("First read: %u\n", (unsigned int)i2cRead(fd));
        printf("Second read: %u\n", (unsigned int)i2cRead(fd));
        printf("Third read: %u\n", (unsigned int)i2cRead(fd));


        close(fd);

        return 0;
}
