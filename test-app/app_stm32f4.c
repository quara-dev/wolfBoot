/* stm32f4.c
 *
 * Test bare-metal blinking led application
 *
 * Copyright (C) 2021 wolfSSL Inc.
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "system.h"
#include "timer.h"
#include "led.h"
#include "hal.h"
#include "wolfboot/wolfboot.h"
#include "spi_flash.h"
#include "target.h"

#ifdef TARGET_stm32f4

#ifndef CLOCK_SPEED
#define CLOCK_SPEED (168000000)
#endif

/* Common UART Config */
#if !defined(USE_UART1) && !defined(USE_UART3)
#define USE_UART3
#endif
#define UART_PIN_AF 7
#define UART_CR1_UART_ENABLE    (1 << 13)
#define UART_CR1_SYMBOL_LEN     (1 << 12)
#define UART_CR1_PARITY_ENABLED (1 << 10)
#define UART_CR1_PARITY_ODD     (1 << 9)
#define UART_CR1_TX_ENABLE      (1 << 3)
#define UART_CR1_RX_ENABLE      (1 << 2)
#define UART_CR2_STOPBITS       (3 << 12)
#define UART_SR_TX_EMPTY        (1 << 7)
#define UART_SR_RX_NOTEMPTY     (1 << 5)


/* Common GPIO Config */
#define GPIO_MODE_AF (2)

/* UART1 Config */
#ifdef USE_UART1
#define UART_RX_PIN 7
#define UART_TX_PIN 6

#define UART1 (0x40011000)
#define UART_SR       (*(volatile uint32_t *)(UART1))
#define UART_DR       (*(volatile uint32_t *)(UART1 + 0x04))
#define UART_BRR      (*(volatile uint32_t *)(UART1 + 0x08))
#define UART_CR1      (*(volatile uint32_t *)(UART1 + 0x0c))
#define UART_CR2      (*(volatile uint32_t *)(UART1 + 0x10))

#define UART_CLOCK_ER           (*(volatile uint32_t *)(0x40023844))
#define UART_CLOCK_ER_VAL 	(1 << 4)

#define GPIO_CLOCK_ER (*(volatile uint32_t *)(0x40023830))
#define GPIO_CLOCK_ER_VAL (1 << 1)
#define GPIOB_BASE 0x40020400
#define GPIO_MODE  (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIO_AF    (*(volatile uint32_t *)(GPIOB_BASE + 0x20))
#endif

/* UART3 Config */
#ifdef USE_UART3
#define UART_RX_PIN 9
#define UART_TX_PIN 8

#define UART3 (0x40004800)
#define UART_SR       (*(volatile uint32_t *)(UART3))
#define UART_DR       (*(volatile uint32_t *)(UART3 + 0x04))
#define UART_BRR      (*(volatile uint32_t *)(UART3 + 0x08))
#define UART_CR1      (*(volatile uint32_t *)(UART3 + 0x0c))
#define UART_CR2      (*(volatile uint32_t *)(UART3 + 0x10))

#define UART_CLOCK_ER           (*(volatile uint32_t *)(0x40023840))
#define UART_CLOCK_ER_VAL 	    (1 << 18)

#define GPIO_CLOCK_ER (*(volatile uint32_t *)(0x40023830))
#define GPIO_CLOCK_ER_VAL (1 << 3)
#define GPIOD_BASE 0x40020c00
#define GPIO_MODE  (*(volatile uint32_t *)(GPIOD_BASE + 0x00))
#define GPIO_AF    (*(volatile uint32_t *)(GPIOD_BASE + 0x20))
#endif

#define MSGSIZE 16
#define PAGESIZE (256)
static uint8_t page[PAGESIZE];
static const char ERR='!';
static const char START='*';
static const char UPDATE='U';
static const char ACK='#';
static uint8_t msg[MSGSIZE];

extern void flash_set_waitstates(void);


#ifdef WOLFBOOT_NO_SIGN

#define AIRCR *(volatile uint32_t *)(0xE000ED0C)
#define AIRCR_VKEY (0x05FA << 16)
#   define AIRCR_SYSRESETREQ (1 << 2)

void arch_reboot(void)
{
    AIRCR = AIRCR_SYSRESETREQ | AIRCR_VKEY;
    while(1)
        ;

}
#endif

void uart_write(const char c)
{
    uint32_t reg;
    do {
        reg = UART_SR;
    } while ((reg & UART_SR_TX_EMPTY) == 0);
    UART_DR = c;
}

static void uart_pins_setup(void)
{
    uint32_t reg;
    GPIO_CLOCK_ER |= GPIO_CLOCK_ER_VAL;
    /* Set mode = AF */
    reg = GPIO_MODE & ~ (0x03 << (UART_RX_PIN * 2));
    GPIO_MODE = reg | (2 << (UART_RX_PIN * 2));
    reg = GPIO_MODE & ~ (0x03 << (UART_TX_PIN * 2));
    GPIO_MODE = reg | (2 << (UART_TX_PIN * 2));

    /* Alternate function: use low pins (6 and 7) */
    reg = GPIO_AF & ~(0xf << ((UART_TX_PIN) * 4));
    GPIO_AF = reg | (UART_PIN_AF << ((UART_TX_PIN) * 4));
    reg = GPIO_AF & ~(0xf << ((UART_RX_PIN) * 4));
    GPIO_AF = reg | (UART_PIN_AF << ((UART_RX_PIN) * 4));
}

int uart_setup(uint32_t bitrate, uint8_t data, char parity, uint8_t stop)
{
    uint32_t reg;
    /* Enable pins and configure for AF7 */
    uart_pins_setup();
    /* Turn on the device */
    UART_CLOCK_ER |= UART_CLOCK_ER_VAL;

    /* Configure for TX + RX */
    UART_CR1 |= (UART_CR1_TX_ENABLE | UART_CR1_RX_ENABLE);

    /* Configure clock */
    UART_BRR =  CLOCK_SPEED / bitrate;

    /* Configure data bits */
    if (data == 8)
        UART_CR1 &= ~UART_CR1_SYMBOL_LEN;
    else
        UART_CR1 |= UART_CR1_SYMBOL_LEN;

    /* Configure parity */
    switch (parity) {
        case 'O':
            UART_CR1 |= UART_CR1_PARITY_ODD;
            /* fall through to enable parity */
        case 'E':
            UART_CR1 |= UART_CR1_PARITY_ENABLED;
            break;
        default:
            UART_CR1 &= ~(UART_CR1_PARITY_ENABLED | UART_CR1_PARITY_ODD);
    }
    /* Set stop bits */
    reg = UART_CR2 & ~UART_CR2_STOPBITS;
    if (stop > 1)
        UART_CR2 = reg & (2 << 12);
    else
        UART_CR2 = reg;

    /* Turn on uart */
    UART_CR1 |= UART_CR1_UART_ENABLE;

    return 0;
}

char uart_read(void)
{
    char c;
    volatile uint32_t reg;
    do {
        reg = UART_SR;
    } while ((reg & UART_SR_RX_NOTEMPTY) == 0);
    c = (char)(UART_DR & 0xff);
    return c;
}

static void ack(uint32_t _off)
{
    uint8_t *off = (uint8_t *)(&_off);
    int i;
    uart_write(ACK);
    for (i = 0; i < 4; i++) {
        uart_write(off[i]);
    }
}

static int check(uint8_t *pkt, int size)
{
    int i;
    uint16_t c = 0;
    uint16_t c_rx = *((uint16_t *)(pkt + 2));
    uint16_t *p = (uint16_t *)(pkt + 4);
    for (i = 0; i < ((size - 4) >> 1); i++)
        c += p[i];
    if (c == c_rx)
        return 0;
    return -1;
}

volatile uint32_t time_elapsed = 0;
void main(void) {
    uint32_t tlen = 0;
    volatile uint32_t recv_seq;
    uint32_t r_total = 0;
    uint32_t tot_len = 0;
    uint32_t next_seq = 0;
    uint32_t version = 0;
    uint8_t *v_array = (uint8_t *)&version;
    int i;
    memset(page, 0xFF, PAGESIZE);
    boot_led_on();
    flash_set_waitstates();
    clock_config();
    led_pwm_setup();
    pwm_init(CLOCK_SPEED, 0);

    /* Dim the led by altering the PWM duty-cicle
     * in isr_tim2 (timer.c)
     *
     * Every 50ms, the duty cycle of the PWM connected
     * to the blue led increases/decreases making a pulse
     * effect.
     */
    timer_init(CLOCK_SPEED, 1, 50);
    uart_setup(115200, 8, 'N', 1);
    memset(page, 0xFF, PAGESIZE);
    asm volatile ("cpsie i");

    while(time_elapsed == 0)
        WFI();



    hal_flash_unlock();
    version = wolfBoot_current_firmware_version();
    if ((version & 0x01) == 0)
        wolfBoot_success();
#ifdef EXT_ENCRYPTED
    wolfBoot_set_encrypt_key("0123456789abcdef0123456789abcdef", 32);
#endif
    uart_write(START);
    for (i = 3; i >= 0; i--) {
        uart_write(v_array[i]);
    }
#ifdef WOLFBOOT_NO_SIGN
    while(time_elapsed < 140)
	WFI();
    arch_reboot();
#endif
    while (1) {
        r_total = 0;
        do {
            while(r_total < 2) {
                msg[r_total++] = uart_read();
                if ((r_total == 2) && ((msg[0] != 0xA5) || msg[1] != 0x5A)) {
                    r_total = 0;
                    continue;
                }
            }
            msg[r_total++] = uart_read();
            if ((tot_len == 0) && r_total == 2 + sizeof(uint32_t))
                break;
            if ((r_total > 8)  && (tot_len <= ((r_total - 8) + next_seq)))
                break;
        } while (r_total < MSGSIZE);
        if (tot_len == 0)  {
            tlen = msg[2] + (msg[3] << 8) + (msg[4] << 16) + (msg[5] << 24);
            if (tlen > WOLFBOOT_PARTITION_SIZE - 8) {
                uart_write(ERR);
                uart_write(ERR);
                uart_write(ERR);
                uart_write(ERR);
                uart_write(START);
                recv_seq = 0;
                tot_len = 0;
                continue;
            }
            tot_len = tlen;
            ack(0);
            continue;
        }
        if (check(msg, r_total) < 0) {
            ack(next_seq);
            continue;
        }
        recv_seq = msg[4] + (msg[5] << 8) + (msg[6] << 16) + (msg[7] << 24);
        if (recv_seq == next_seq)
        {
            int psize = r_total - 8;
            int page_idx = recv_seq % PAGESIZE;
            memcpy(&page[recv_seq % PAGESIZE], msg + 8, psize);
            page_idx += psize;
            if ((page_idx == PAGESIZE) || (next_seq + psize >= tot_len)) {
                uint32_t dst = (WOLFBOOT_PARTITION_UPDATE_ADDRESS + recv_seq + psize) - page_idx;
                if ((dst % WOLFBOOT_SECTOR_SIZE) == 0) {
                    hal_flash_erase(dst, WOLFBOOT_SECTOR_SIZE);
                }
                hal_flash_write(dst, page, PAGESIZE);
                memset(page, 0xFF, PAGESIZE);
            }
            next_seq += psize;
        }
        ack(next_seq);
        if (next_seq >= tot_len) {
            /* Update complete */
            spi_flash_probe();
            wolfBoot_update_trigger();
            spi_flash_release();
            hal_flash_lock();
            break;
        }
    }
    /* Wait for reboot */
    while(1)
        ;
}
#endif /** TARGET_stm32f4 **/

