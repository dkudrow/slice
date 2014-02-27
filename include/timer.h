/*
 * include/timer.h
 *
 * system timer
 */ 

#ifndef TIMER_H
#define TIMER_H

/* addresses of timer */
#define TIMER_BASE		0x20003000

/* offsets for timer registers */
#define TIMER_CTRL		0x0
#define TIMER_CNTLO		0x4
#define TIMER_CNTHI		0x8
#define TIMER_CMP0		0xC
#define TIMER_CMP1		0x10
#define TIMER_CMP2		0x14
#define TIMER_CMP3		0x18

/* function prototypes */
unsigned timer_read();
void timer_wait(unsigned ticks);

#endif /* TIMER_H */
