/*
 * src/keyboard.c
 *
 * keyboard driver
 */

#include "keyboard.h"

struct {
	unsigned mask;
	char key;
} modifiers[KBD_NMOD] = {
	{ KBD_MOD_LCTL, 0x00 },
	{ KBD_MOD_RCTL, 0x00 },
	{ KBD_MOD_LSFT, 0x00 },
	{ KBD_MOD_RSFT, 0x00 },
	{ KBD_MOD_LALT, 0x00 },
	{ KBD_MOD_RALT, 0x00 },
	{ KBD_MOD_LWIN, 0x00 },
	{ KBD_MOD_RWIN, 0x00 }
};

/*
 * check whether a key has been pressed
 */
static int keyboard_keypress(struct keyboard_t *kbd)
{
	int i;

	for (i=0; i<KBD_NKEYCODE; i++)
		if (kbd->old_state.keycodes[i])
			return 1;

	return 0;
}

/*
 * add a keycode to the current state 
 */
static void keyboard_put_key(unsigned keycode)
{
}

/*
 * enable a keyboard
 */
void keyboard_enable(struct keyboard_t *kbd)
{
	kbd->active = 1;
}

/*
 * disable a keyboard
 */
void keyboard_disable(struct keyboard_t *kbd)
{
	kbd->active = 0;
}

int keyboard_check_char(struct keyboard_t *kbd)
{
	int i, j;

	if (kbd->active == 0)
		return 0;

	struct key_state_t old = kbd->old_state;
	struct key_state_t new = kbd->new_state;

	/* ===== Note: adapted from freeBSD:ukbd_do_poll() ===== */

	/* check modifiers */
	if (old.modifiers != new.modifiers) {
		for (i=0; i<KBD_NMOD; i++) {
			if ((old.modifiers & modifiers[i].mask) == 
					(new.modifiers & modifiers[i].mask))
				continue;
			keyboard_put_key(modifiers[i].key |
					(new.modifiers) ? KBD_PRESS : KBD_RELEASE);
		}
	}

	/* check key releases */
	for (i=0; i<KBD_NKEYCODE; i++) {
		if (old.keycodes[i] == 0)
			goto next_release;
		for (j=0; j<KBD_NKEYCODE; j++) {
			if (old.keycodes[i] == new.keycodes[j])
				goto next_release;
		}
		keyboard_put_key(old.keycodes[i] | KBD_RELEASE);
next_release: ;
	}

	/* check key presses */
	for (i=0; i<KBD_NKEYCODE; i++) {
		if (new.keycodes[i] == 0)
			continue;
		for (j=0; j<KBD_NKEYCODE; j++) {
			if (new.keycodes[i] == old.keycodes[j])
				/* FIXME: handle long presses */
				goto next_press;
		}
		keyboard_put_key(new.keycodes[i] | KBD_PRESS);
next_press: ;
	}

	/* update the state information */
	kbd->old_state = kbd->new_state;

	/*  TODO: left off here (last line or so of ukbd_do_poll */
}

/*
 * read a character from the keyboard
keyboard_read_char
 */

/*
 * check whether there is a character waiting for us
keyboard_check_char
 */

/*
 * do we really need an ioctl?
keyboard_ioctl
 */

/*
 * clear the internal state of the keyboard - remove modifiers, clear buffer
keyboard_clear_state
 */
