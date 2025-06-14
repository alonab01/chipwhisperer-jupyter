#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t set_key(uint8_t *pt, uint8_t len)
{
	/*
	 * set_key - function used to change the key
	 * given as a callback to the simpleserial interface
	 * Needs a command 'k' and a 16 byte (256 bit) input
	 */
	aes_indep_init();
	aes_indep_key(pt); // sets the key. size = 16 bytes
	return 0x0;
}

uint8_t get_pt(uint8_t *pt, uint8_t len)
{
	/*
	 * get_pt - does AES block encryption with the key 
	 * stored inside the microcontroller
	 * This key can be configured using the `set_key` function
	 *
	 * This function is given as a callback to the simpleserial interface
	 * will be called after receiving a command 'p' and a 16 byte input
	 */
	aes_indep_enc_pretrigger(pt);

	trigger_high();
	aes_indep_enc(pt); /* encrypting the data block */
	trigger_low();

	aes_indep_enc_posttrigger(pt);

	simpleserial_put('r', 16, pt);
	return 0x00;
}

int main(void)
{
	uint8_t secret_key[KEY_LENGTH] = {DEFAULT_KEY};

	platform_init();
	init_uart();
	trigger_setup();

	aes_indep_init();
	aes_indep_key(secret_key); // sets the key. size = 16 bytes

	simpleserial_init();
	simpleserial_addcmd('p', 16, get_pt);
	simpleserial_addcmd('k', 16, set_key);
	while (1)
		simpleserial_get();
}
