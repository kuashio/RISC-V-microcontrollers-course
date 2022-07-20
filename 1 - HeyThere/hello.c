/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <stdint.h>

int main() {
	volatile uint32_t a, b, c;
	a = 4;
	b = 37;
	c = a + b;
	printf("Hey there! Greetings from a RISC-V CPU!\n");
}
