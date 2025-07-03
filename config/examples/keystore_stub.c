/* Keystore stub file for wolfBoot - automatically generated */
/* This file will be replaced by the actual keystore during key generation */

#include "keystore.h"

#ifdef WOLFBOOT_NO_SIGN
/* No signing - empty keystore */
const struct keystore_slot PubKeys[1] = { {0} };

int keystore_num_pubkeys(void)
{
    return 0;
}

uint8_t *keystore_get_buffer(int id)
{
    (void)id;
    return NULL;
}

int keystore_get_size(int id)
{
    (void)id;
    return 0;
}
#else
/* Placeholder keystore - will be replaced by actual keys */
#warning "Using placeholder keystore - generate actual keys for production"

const struct keystore_slot PubKeys[1] = {
    {
        .slot_id = 0,
        .key_type = 0,
        .part_id_mask = 0xFFFFFFFF,
        .pubkey_size = 0,
        .pubkey = { 0 }
    }
};

int keystore_num_pubkeys(void)
{
    return 0;  /* Return 0 until actual keys are generated */
}

uint8_t *keystore_get_buffer(int id)
{
    (void)id;
    return NULL;
}

int keystore_get_size(int id)
{
    (void)id;
    return 0;
}
#endif
