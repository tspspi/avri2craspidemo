# AVR to RaspberryPi I2C communication example

This code demonstrates the basic operations when using
an AVR as I2C slave and an RaspberryPi running FreeBSD
as master.

The AVR code has been packed into an Arduino compatible
source file ```avri2craspidemo.ino```. If one just
wants to use it on bare avr-gcc one can just move the
code from ```setup``` into the main function, remove the
```loop``` function and add an infinite loop at the
end of ```main```.

The test code for the RaspberryPi ```avri2craspidemo.c```
can be compiled with clang using

```
clang -o avri2craspidemo -Wall -ansi -std=c99 -pedantic avri2craspidemo.c
```

It requires to have proper access permissions on the ```/dev/iic1```
device.
