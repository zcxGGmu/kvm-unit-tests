/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * SIGP related definitions.
 *
 * Copied from the Linux kernel file arch/s390/include/asm/sigp.h
 */

#ifndef _ASMS390X_SIGP_H_
#define _ASMS390X_SIGP_H_

/* SIGP order codes */
#define SIGP_SENSE			1
#define SIGP_EXTERNAL_CALL		2
#define SIGP_EMERGENCY_SIGNAL		3
#define SIGP_START			4
#define SIGP_STOP			5
#define SIGP_RESTART			6
#define SIGP_STOP_AND_STORE_STATUS	9
#define SIGP_INITIAL_CPU_RESET		11
#define SIGP_CPU_RESET			12
#define SIGP_SET_PREFIX			13
#define SIGP_STORE_STATUS_AT_ADDRESS	14
#define SIGP_SET_ARCHITECTURE		18
#define SIGP_COND_EMERGENCY_SIGNAL	19
#define SIGP_SENSE_RUNNING		21
#define SIGP_SET_MULTI_THREADING	22
#define SIGP_STORE_ADDITIONAL_STATUS	23

/* SIGP condition codes */
#define SIGP_CC_ORDER_CODE_ACCEPTED 0
#define SIGP_CC_STATUS_STORED	    1
#define SIGP_CC_BUSY		    2
#define SIGP_CC_NOT_OPERATIONAL	    3

/* SIGP cpu status bits */

#define SIGP_STATUS_INVALID_ORDER	0x00000002UL
#define SIGP_STATUS_CHECK_STOP		0x00000010UL
#define SIGP_STATUS_STOPPED		0x00000040UL
#define SIGP_STATUS_EXT_CALL_PENDING	0x00000080UL
#define SIGP_STATUS_INVALID_PARAMETER	0x00000100UL
#define SIGP_STATUS_INCORRECT_STATE	0x00000200UL
#define SIGP_STATUS_NOT_RUNNING		0x00000400UL

#ifndef __ASSEMBLER__


static inline int sigp(uint16_t addr, uint8_t order, unsigned long parm,
		       uint32_t *status)
{
	register unsigned long reg1 asm ("1") = parm;
	uint64_t bogus_cc = SIGP_CC_NOT_OPERATIONAL;
	int cc;

	asm volatile(
		"	tmll	%[bogus_cc],3\n"
		"	sigp	%[reg1],%[addr],0(%[order])\n"
		"	ipm	%[cc]\n"
		"	srl	%[cc],28\n"
		: [cc] "=d" (cc), [reg1] "+d" (reg1)
		: [addr] "d" (addr), [order] "a" (order), [bogus_cc] "d" (bogus_cc)
		: "cc");
	if (status)
		*status = reg1;
	return cc;
}

static inline int sigp_retry(uint16_t addr, uint8_t order, unsigned long parm,
			     uint32_t *status)
{
	int cc;

	do {
		cc = sigp(addr, order, parm, status);
	} while (cc == 2);
	return cc;
}

#endif /* __ASSEMBLER__ */
#endif /* _ASMS390X_SIGP_H_ */
