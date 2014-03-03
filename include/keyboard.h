/*
 * include/keyboard.h
 *
 * keyboard device
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_NKEYCODE	6			/* maxmimum simultaneous key presses */

#define KBD_RELEASE		0x400		/* OR with keycode to indicate release */
#define KBD_PRESS		0x00		/* OR with keycode to indicate press */

#define KBD_MOD_LCTL	(1 << 0)	/* left control */
#define KBD_MOD_RCTL	(1 << 1)	/* right control */
#define KBD_MOD_LSFT	(1 << 2)	/* left shift */
#define KBD_MOD_RSFT	(1 << 3)	/* right shift */
#define KBD_MOD_LALT	(1 << 4)	/* left alt */
#define KBD_MOD_RALT	(1 << 5)	/* right alt */
#define KBD_MOD_LWIN	(1 << 6)	/* left windows key */
#define KBD_MOD_RWIN	(1 << 7)	/* right windows key */
#define KBD_NMOD		8			/* number of modifiers */

#define KBD_IN_BUF_SIZE	10

#define KBD_LED_CAP		(1 << 0)
#define KBD_LED_NUM		(1 << 1)

struct key_state_t {
	unsigned modifiers;
	unsigned char keycodes[KBD_NKEYCODE];
};

/* 
 * defines the current keyboard state
 */
struct keyboard_t {
	unsigned flags;
	unsigned active;

	/* store the current and old key states to determine presses */
	struct key_state_t old_state;
	struct key_state_t new_state;

	/* buffer incoming keycodes */
	unsigned input_buf[KBD_IN_BUF_SIZE];
	unsigned input_head;
	unsigned input_tail;
};

#endif /* KEYBOARD_H */
