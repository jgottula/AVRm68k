#include "sd.h"
#include <string.h>
#include <util/delay.h>
#include "debug.h"
#include "intr.h"
#include "io.h"
#include "spi.h"

struct {
	bool init;
	bool ver2;
	bool hc;
	uint8_t spiDiv;
	
	struct {
		bool valid;
		uint8_t bytes[4];
	} ocr;
} sdCtx;

/* don't ask me how this works!
 * 
 * seed is initially zero; thereafter, it's the output of the prev round
 * input is the current byte to process
 * bits is the number of bits, usually 8
 * 
 * after the last round, do another with zero as the input and 7 bits */
static uint8_t sdCRC7(uint8_t seed, uint8_t input, uint8_t bits)
{
	uint8_t val = seed, cc = input;
	
	do
	{
		val = (val << 1) + ((cc & 0x80) ? 1 : 0);
		
		if (val & 0x80)
			val ^= 0x9; // polynomial
		
		cc <<= 1;
	}
	while (--bits);
	
	return val & 0x7f;
}

static bool sdCmd4(uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2,
	uint8_t arg3, bool crc, uint16_t timeout, SDRx *resp)
{
	uint8_t cmdByte = 0b01000000 | (cmd & 0b00111111), r1Byte;
	
	spiBegin(SPI_SS_SD, SPIMODE_SD, SPIENDIAN_SD, sdCtx.spiDiv);
	
	/* command byte */
	spiByte(cmdByte);
	
	/* arguments */
	spiByte(arg3);
	spiByte(arg2);
	spiByte(arg1);
	spiByte(arg0);
	
	if (crc)
	{
		uint8_t acc = sdCRC7(0, cmdByte, 8);
		acc = sdCRC7(acc, arg3, 8);
		acc = sdCRC7(acc, arg2, 8);
		acc = sdCRC7(acc, arg1, 8);
		acc = sdCRC7(acc, arg0, 8);
		acc = sdCRC7(acc, 0, 7);
		
		uint8_t crcByte = (acc << 1) | 1;
		
		if (cmd == 0)
			assert(crcByte == 0x95);
		
		spiByte(crcByte);
	}
	else
		spiByte(1);
	
	/* wait for the MS bit to be zero */
	uint16_t timeStart = msec;
	while ((r1Byte = spiByte(0xff)) & _BV(7))
	{
		if (msec - timeStart >= timeout)
		{
			spiEnd();
			return true;
		}
	}
	
	/* load responses based on the command type */
	switch (cmd)
	{
	case SDCMD0: // SDCMD_GO_IDLE_STATE
		resp->r1.resp = r1Byte;
		break;
	case SDCMD8: // SDCMD_SEND_IF_CMD
		resp->r7.resp = r1Byte;
		resp->r7.data[0] = spiByte(0xff);
		resp->r7.data[1] = spiByte(0xff);
		resp->r7.data[2] = spiByte(0xff);
		resp->r7.data[3] = spiByte(0xff);
		break;
	case SDCMD58: // SDCMD_READ_OCR
		resp->r3.resp = r1Byte;
		resp->r3.ocr[0] = spiByte(0xff);
		resp->r3.ocr[1] = spiByte(0xff);
		resp->r3.ocr[2] = spiByte(0xff);
		resp->r3.ocr[3] = spiByte(0xff);
		break;
	}
	
	spiEnd();
	return false;
}

static bool sdCmd3(uint8_t cmd, uint8_t arg0, uint8_t arg1, uint8_t arg2,
	bool crc, uint16_t timeout, SDRx *resp)
{
	return sdCmd4(cmd, arg0, arg1, arg2, 0, crc, timeout, resp);
}

static bool sdCmd2(uint8_t cmd, uint8_t arg0, uint8_t arg1, bool crc,
	uint16_t timeout, SDRx *resp)
{
	return sdCmd4(cmd, arg0, arg1, 0, 0, crc, timeout, resp);
}

static bool sdCmd1(uint8_t cmd, uint8_t arg0, bool crc, uint16_t timeout,
	SDRx *resp)
{
	return sdCmd4(cmd, arg0, 0, 0, 0, crc, timeout, resp);
}

static bool sdCmd0(uint8_t cmd, bool crc, uint16_t timeout, SDRx *resp)
{
	return sdCmd4(cmd, 0, 0, 0, 0, crc, timeout, resp);
}

void sdInit()
{
	SDRx resp;
	
	memset(&sdCtx, 0, sizeof(sdCtx));
	
	/* start at 0 MHz / 64 = 312 kHz */
	sdCtx.spiDiv = SPIDIV_64;
	
	/* wait for the card to power up */
	_delay_ms(35);
	
	/* send 80 dummy clock pulses without selecting the device */
	for (uint8_t i = 10; i > 0; --i)
		spiByte(0xff);
	
	/* send a CMD0 with the select line low to enter SPI mode */
	if (sdCmd0(SDCMD0, true, 1000, &resp))
	{
		uartWritePSTR("No card, or error.\n");
		
		die();
	}
	
	if (resp.r1.resp != SDR1_IDLE)
	{
		uartWritePSTR("CMD0 error: 0x");
		uartWriteHex8(resp.r1.resp, true);
		uartWriteChr('\n');
		
		die();
	}
	
	/* send a CMD8 to check for SD version >= 2.00 (long timeout) */
	if (sdCmd2(SDCMD8, 0b10101010, SDVOLT_33, true, 2000, &resp))
	{
		uartWritePSTR("Card unresponsive.\n");
		
		die();
	}
	
	if (resp.r7.resp & SDR1_ILLEGAL_CMD)
	{
		uartWritePSTR("SDv1/MMC card.\n");
		
		sdCtx.ver2 = false;
	}
	else if (resp.r7.resp == SDR1_IDLE)
	{
		if (resp.r7.data[3] != 0b10101010)
		{
			uartWritePSTR("CMD8 pattern fail.\nExp: 0xAA Act: 0x");
			uartWriteHex8(resp.r7.data[3], true);
			uartWriteChr('\n');
			
			die();
		}
		else if ((resp.r7.data[2] & 0xf) != SDVOLT_33)
		{
			uartWritePSTR("Bad voltage range.\n");
			
			die();
		}
		
		uartWritePSTR("SDv2 card.\n");
		
		sdCtx.ver2 = true;
	}
	else
	{
		uartWritePSTR("CMD8 error: 0x");
		uartWriteHex8(resp.r7.resp, true);
		uartWriteChr('\n');
		
		die();
	}
	
	/* load the OCR register, which has information on supported voltages */
	if (sdCmd0(SDCMD58, false, 100, &resp))
	{
		uartWritePSTR("CMD58 timeout.\n");
		
		die();
	}
	else if (resp.r3.resp == SDR1_IDLE)
	{
		memcpy(sdCtx.ocr.bytes, resp.r3.ocr, 4);
		sdCtx.ocr.valid = true;
	}
	
	sdCtx.init = true;
	uartWritePSTR("Init OK.\n");
	
	/* TODO: set the SPI clock rate to what the card says is its max;
	 * then continue following the init flowchart */
}
