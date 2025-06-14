#include "aes-independant.h"
#include "hal.h"
#include "simpleserial.h"
#include <stdint.h>
#include <stdlib.h>

uint8_t get_pt(uint8_t *pt, uint8_t len)
{
	/*
	 * get_pt - does AES block encryption
	 *
	 * receives as input both key and a plaintext
	 * 'p' [16 bytes of plaintext] [16 bytes of key]
	 * 
	 * This function is given as a callback to the simpleserial interface
	 * will be called after receiving a command 'p' and a 32 byte input
	 */
	aes_indep_enc_pretrigger(pt);
	aes_indep_init();
	aes_indep_key(&pt[KEY_LENGTH]);

	trigger_high();
	aes_indep_enc(pt); /* encrypting the data block */
	trigger_low();

	aes_indep_enc_posttrigger(pt);

	simpleserial_put('r', KEY_LENGTH, pt);
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
	simpleserial_addcmd('p', 2 * KEY_LENGTH, get_pt);
	while (1)
		simpleserial_get();
}
