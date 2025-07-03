/* unit-nvm.c
 *
 * unit tests for nvm_flash_writeonce workaround.
 *
 * Copyright (C) 2024 wolfSSL Inc.
 *
 * This file is part of wolfBoot.
 *
 * wolfBoot is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfBoot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#define WOLFBOOT_HASH_SHA256
#define IMAGE_HEADER_SIZE 256
#define MOCK_ADDRESS 0xCC000000
#define MOCK_ADDRESS_BOOT 0xCD000000
#define MOCK_ADDRESS_SWAP 0xCE000000
#define WC_RSA_BLINDING
#define ECC_TIMING_RESISTANT
#include <stdio.h>
#include "libwolfboot.c"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <check.h>

#include "unit-mock-flash.c"


Suite *wolfboot_suite(void);


START_TEST (test_nvm_select_fresh_sector)
{
    int ret, i;
    const char BOOT[] = "BOOT";
    const uint32_t *boot_word = (const uint32_t *)BOOT;
    uint8_t st;
    uint32_t *magic;
    uint8_t *dst, *src;
    uint8_t part = PART_UPDATE;
    uint32_t base_addr = WOLFBOOT_PARTITION_UPDATE_ADDRESS;
    uint32_t home_off = 0;

    ret = mmap_file("/tmp/wolfboot-unit-file.bin", (void *)MOCK_ADDRESS,
            WOLFBOOT_PARTITION_SIZE, NULL);
    ck_assert(ret >= 0);
#ifdef FLAGS_HOME
    ret = mmap_file("/tmp/wolfboot-unit-int-file.bin", (void *)MOCK_ADDRESS_BOOT,
            WOLFBOOT_PARTITION_SIZE, NULL);
    ck_assert(ret >= 0);
    part = PART_BOOT;
    base_addr = WOLFBOOT_PARTITION_BOOT_ADDRESS;
    home_off = PART_BOOT_ENDFLAGS - PART_UPDATE_ENDFLAGS;
#endif
    ret = mmap_file("/tmp/wolfboot-unit-swap.bin", (void *)MOCK_ADDRESS_SWAP,
            WOLFBOOT_SECTOR_SIZE, NULL);
    ck_assert(ret >= 0);

    /* Sanity */
    ck_assert(home_off <= WOLFBOOT_SECTOR_SIZE);
    
    /* unlock the flash to allow operations */
    hal_flash_unlock();

    /* Check swap erase */
    wolfBoot_erase_partition(PART_SWAP);
    ck_assert(erased_swap == 1);
    for (i = 0; i < WOLFBOOT_SECTOR_SIZE; i+=4) {
        uint32_t *word = ((uint32_t *)((uintptr_t)WOLFBOOT_PARTITION_SWAP_ADDRESS + i));
        ck_assert(*word == 0xFFFFFFFF);
    }

    erased_update = 0;
    wolfBoot_erase_partition(part);
#ifndef FLAGS_HOME
    ck_assert(erased_update == 1);
#else
    ck_assert(erased_boot == 1);
#endif
    /* Erased flag sectors: select '0' by default */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select default fresh sector\n");

    /* Force a good 'magic' at the end of sector 1 by setting the magic word */
    wolfBoot_set_partition_state(PART_UPDATE, IMG_STATE_NEW);
    magic = get_partition_magic(PART_UPDATE);
    ck_assert_msg(*magic == *boot_word,
            "Failed to read back 'BOOT' trailer at the end of the partition");

    /* Current selected should now be 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select good fresh sector\n");

    erased_nvm_bank1 = 0;
    erased_nvm_bank0 = 0;

    /* Calling 'set_partition_state' should change the current sector */
    wolfBoot_set_partition_state(PART_UPDATE, IMG_STATE_UPDATING);

    /* Current selected should now be 0 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank1 > 0, "Did not erase the non-selected bank");

    erased_nvm_bank1 = 0;
    erased_nvm_bank0 = 0;

    /* Check state is read back correctly */
    ret = wolfBoot_get_partition_state(PART_UPDATE, &st);
    ck_assert_msg(ret == 0, "Failed to read back state\n");
    ck_assert_msg(st == IMG_STATE_UPDATING, "Bootloader in the wrong state\n");

    /* Check that reading did not change the current sector */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select right sector after reading\n");

    /* Update one sector flag, it should change nvm sector */
    wolfBoot_set_update_sector_flag(0, SECT_FLAG_SWAPPING);

    /* Current selected should now be 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank0 > 0, "Did not erase the non-selected bank");

    /* Check sector state is read back correctly */
    ret = wolfBoot_get_update_sector_flag(0, &st);
    ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
    ck_assert_msg(st == SECT_FLAG_SWAPPING, "Wrong sector flag state\n");

    /* Check that reading did not change the current sector (1) */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select right sector after reading sector state\n");

    /* Update sector flag, again. it should change nvm sector */
    erased_nvm_bank1 = 0;
    erased_nvm_bank0 = 0;
    wolfBoot_set_update_sector_flag(0, SECT_FLAG_UPDATED);

    /* Current selected should now be 0 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank1 > 0, "Did not erase the non-selected bank");

    /* Check sector state is read back correctly */
    ret = wolfBoot_get_update_sector_flag(0, &st);
    ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
    ck_assert_msg(st == SECT_FLAG_UPDATED, "Wrong sector flag state\n");

    /* Check that reading did not change the current sector (0) */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select right sector after reading sector state\n");

    /* Update sector flag, again. it should change nvm sector */
    erased_nvm_bank1 = 0;
    erased_nvm_bank0 = 0;
    wolfBoot_set_update_sector_flag(1, SECT_FLAG_SWAPPING);

    /* Current selected should now be 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank0 > 0, "Did not erase the non-selected bank");

    /* Check sector state is read back correctly */
    ret = wolfBoot_get_update_sector_flag(1, &st);
    ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
    ck_assert_msg(st == SECT_FLAG_SWAPPING, "Wrong sector flag state\n");

    /* Check that reading did not change the current sector (1) */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select right sector after reading sector state\n");

    /* Update sector flag, again. it should change nvm sector */
    erased_nvm_bank1 = 0;
    erased_nvm_bank0 = 0;
    wolfBoot_set_update_sector_flag(1, SECT_FLAG_UPDATED);

    /* Copy flags from 0 to 1 */
    src = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off));
    dst = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off + WOLFBOOT_SECTOR_SIZE));
    for (i = 0; i < 8; i++)
        dst[i] = src[i];

    /* Force-erase 4B of sector flags in 0 */
    dst = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off));
    for (i = 0; i < 4; i++)
        dst[i] = 0xFF;

    /* This should fall back to 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select most recent sector after deleting flags\n");

    /* Start over, update some sector flags */
    wolfBoot_erase_partition(PART_UPDATE);
    wolfBoot_set_update_sector_flag(0, SECT_FLAG_UPDATED);
    wolfBoot_set_update_sector_flag(1, SECT_FLAG_UPDATED);
    wolfBoot_set_update_sector_flag(2, SECT_FLAG_UPDATED);
    wolfBoot_set_update_sector_flag(3, SECT_FLAG_UPDATED);
    wolfBoot_set_update_sector_flag(4, SECT_FLAG_SWAPPING);
    st = IMG_STATE_UPDATING;
    wolfBoot_set_partition_state(PART_UPDATE, (uintptr_t)&st);

    /* Current selected should now be 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank0 > 0, "Did not erase the non-selected bank");

    /* Check sector state is read back correctly */
    for (i = 0; i < 4; i++) {
        ret = wolfBoot_get_update_sector_flag(i, &st);
        ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
        ck_assert_msg(st == SECT_FLAG_UPDATED, "Wrong sector flag state\n");

    }
    ret = wolfBoot_get_update_sector_flag(4, &st);
    ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
    ck_assert_msg(st == SECT_FLAG_SWAPPING, "Wrong sector flag state\n");

    /* Check that reading did not change the current sector (1) */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select right sector after reading sector state\n");

    /* Copy flags from 1 to 0 */
    src = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off + WOLFBOOT_SECTOR_SIZE));
    dst = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off));
    for (i = 0; i < 8; i++)
        dst[i] = src[i];

    /* Force to F0 last sector flag in 0, so that the sector '4' is 'updated' */
    dst = (uint8_t *)((uintptr_t)base_addr + WOLFBOOT_PARTITION_SIZE - (8 + home_off));
    dst[0] = 0xF0;

    /* Check if still there */
    ret = wolfBoot_get_update_sector_flag(4, &st);
    ck_assert_msg(ret == 0, "Failed to read sector flag state\n");
    ck_assert_msg(st == SECT_FLAG_UPDATED, "Wrong sector flag state\n");

    /* This should fall back to 0 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select most recent sector after deleting flags\n");


    /* Erase partition and start over */
    erased_update = 0;
    erased_boot = 0;
    wolfBoot_erase_partition(part);
#ifndef FLAGS_HOME
    ck_assert(erased_update == 1);
#else
    ck_assert(erased_boot == 1);
#endif

    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 0, "Failed to select right sector after reading sector state\n");
    
    /* re-lock the flash: update_trigger implies unlocking/locking */
    hal_flash_lock();

    /* Triggering update to set flags */
    wolfBoot_update_trigger();

    /* Current selected should now be 1 */
    ret = nvm_select_fresh_sector(PART_UPDATE);
    ck_assert_msg(ret == 1, "Failed to select updating fresh sector\n");
    ck_assert_msg(erased_nvm_bank0 > 0, "Did not erase the non-selected bank");

    magic = get_partition_magic(PART_UPDATE);
    ck_assert_msg(*magic == *boot_word,
            "Failed to read back 'BOOT' trailer at the end of the partition");

    /* Sanity check at the end of the operations. */
    ck_assert_msg(locked, "The FLASH was left unlocked.\n");


}
END_TEST


Suite *wolfboot_suite(void)
{
    /* Suite initialization */
    Suite *s = suite_create("wolfboot");

    /* Test cases */
    TCase *nvm_select_fresh_sector = tcase_create("NVM select fresh sector");
    tcase_add_test(nvm_select_fresh_sector, test_nvm_select_fresh_sector);
    suite_add_tcase(s, nvm_select_fresh_sector);

    return s;
}


int main(int argc, char *argv[])
{
    int fails;
    argv0 = strdup(argv[0]);
    Suite *s = wolfboot_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    fails = srunner_ntests_failed(sr);
    srunner_free(sr);
    return fails;
}
