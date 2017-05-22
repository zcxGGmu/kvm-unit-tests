/*
 * s390x io implementation
 *
 * Copyright (c) 2017 Red Hat Inc
 *
 * Authors:
 *  Thomas Huth <thuth@redhat.com>
 *  David Hildenbrand <david@redhat.com>
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License version 2.
 */
#include <libcflat.h>
#include <asm/spinlock.h>

extern void setup_args_progname(const char *args);
extern char ipl_args[];

static struct spinlock lock;

void puts(const char *s)
{
	spin_lock(&lock);
	/* FIXME */
	(void)s;
	spin_unlock(&lock);
}

static void sigp_stop()
{
	register unsigned long status asm ("1") = 0;
	register unsigned long cpu asm ("2") = 0;

	asm volatile(
		"	sigp %0,%1,0(%2)\n"
		: "+d" (status)  : "d" (cpu), "d" (5) : "cc");
}

void setup()
{
	setup_args_progname(ipl_args);
}

void exit(int code)
{
	printf("\nEXIT: STATUS=%d\n", ((code) << 1) | 1);
	sigp_stop();
}
