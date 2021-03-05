#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "defdbg.h"
#include "kbd_types.h"
#define KEYBOARD "/dev/Mega8-kbd"
#define KEY_BUF_LEN 255
typedef struct{
 MWKEY mwkey;
 int scancode;
}KeyMap;
class KeyBoard
{
public:
    static KeyBoard kbd;
    KeyBoard();
    ~KeyBoard();
    //public for get_key.c
    char get_key(void);
    int get_line(char *cmd);
private:
    //private for get_key.c
    int kbd_init();
    int kbd_close();
    //private for keyboard.c
    int  KBD_Open(void);
    void KBD_Close(void);
    void KBD_GetModifierInfo(MWKEYMOD *modifiers, MWKEYMOD *curmodifiers);
    int  KBD_Read(char *kbuf, MWKEYMOD *modifiers, MWSCANCODE *scancode);


private:
    //private for get_key.c
    char keybuf[KEY_BUF_LEN];
    int pWrite; //write key buffer point
    int pRead;
    int pHead;
    int KEY_BUF_FULL;
private:
    //private for keyboard.c
    int fd;
    MWKEY scancodes[64];
    static KeyMap keymap[17];

};

#endif // KEYBOARD_H
