#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdint.h>

#define TWI_ADDRESS 0x14

static uint8_t lastByte = 0x00;

static void i2cEventReceived(uint8_t data) {
	// Do whatever we want with the received data
	lastByte = data;
}
static void i2cEventBusError() {
	// Ignore
	return;
}
static uint8_t i2cEventTransmit() {
	lastByte = lastByte + 1;
	return lastByte;
}


static void i2cSlaveInit(uint8_t address) {
	cli();

	TWAR = (address << 1) | 0x01; // Respond to general calls and calls towards us
	TWCR = 0xC5; // Set TWIE (TWI Interrupt enable), TWEN (TWI Enable), TWEA (TWI Enable Acknowledgement), TWINT (Clear TWINT flag by writing a 1)

	sei();
	return;
}

static void i2cSlaveShutdown() {
	cli();

	TWCR = 0;
	TWAR = 0;

	sei();
	return;
}

ISR(TWI_vect) {
	switch(TW_STATUS) { /* Note: TW_STATUS is an macro that masks status bits from TWSR) */
		case TW_SR_SLA_ACK:
		case TW_SR_DATA_ACK:
			/*
				We have received data. This is now contained in the TWI
				data register (TWDR)
			*/
			i2cEventReceived(TWDR);
			break;
		case TW_ST_SLA_ACK:
		case TW_ST_DATA_ACK:
			/*
				Either slave selected (SLA_ACK) and data requested or data transmitted, ACK received
				and next data requested
			*/
			TWDR = i2cEventTransmit();
			break;
		case TW_BUS_ERROR:
			i2cEventBusError();
			break;
		default:
			break;
	}
	TWCR = 0xC5; // Set TWIE (TWI Interrupt enable), TWEN (TWI Enable), TWEA (TWI Enable Acknowledgement), TWINT (Clear TWINT flag by writing a 1)
}


void setup() {
	/*
		Disable watchdog, initialize serial
	*/
	wdt_disable();

	/*
		Inputs are - after reset - configured as
		input (tristate) with pullups disabled (as
		they should be)
	*/

	/*
		Initialize TWI slave
	*/
	i2cSlaveInit(TWI_ADDRESS);
}

void loop() {
	// Do nothing here - this is entirely interrupt driven
}
