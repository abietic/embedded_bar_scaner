#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "keyboard.h"
#include "kbd_types.h"




KeyMap KeyBoard::keymap[17] = {
      {MWKEY_KP0,  11},
      {MWKEY_KP1,  0x2},
      {MWKEY_KP2,  3},
      {MWKEY_KP3,  4},
      {MWKEY_KP4,  5},
      {MWKEY_KP5,  6},
      {MWKEY_KP6,  7},
      {MWKEY_KP7,  8},
      {MWKEY_KP8,  9},
      {MWKEY_KP9,  0xa},
      {MWKEY_NUMLOCK, 42},
      {MWKEY_KP_DIVIDE, 53},
      {MWKEY_KP_MULTIPLY,55},
      {MWKEY_KP_MINUS, 74},
      {MWKEY_KP_PLUS, 78},
      {MWKEY_KP_ENTER, 28},
      {MWKEY_KP_DEL, 83},
};

KeyBoard KeyBoard::kbd;

KeyBoard::KeyBoard()
{
    if (this->kbd_init() == 0)
    {}
    else
    {
        exit(-1);
    }
}

KeyBoard::~KeyBoard()
{
    this->kbd_close();
#ifdef DEBUG
    printf("keyboard closed!\n");
#endif
}


/**********************************************************/
int KeyBoard::kbd_init()
{
    pWrite=0; //write key buffer point
    pRead =0;
    pHead =0;
    KEY_BUF_FULL=0;
 //char key;
   //void * retval;
 if (KBD_Open() < 0) {
  printf("Can't open keyboard!\n");
  return -1;
 }else
  printf("keyboard opend!\n");
 return 0;

}
/**********************************************************/
int KeyBoard::kbd_close()
{
 KBD_Close();
 return 0;
}
/**********************************************************/
//static void * read_keyboard(void * data)
char KeyBoard::get_key(void)
{
 int keydown=0/*,old_keydown*/;
 char  key=0;
 MWKEYMOD  modifiers;
 MWSCANCODE  scancode;
 while(1){
  keydown=KBD_Read(&key, &modifiers, &scancode); //block read
 // printf("waiting keydown : %d\n",keydown);
  if(keydown==1){//key press down  2: key up
/*   keybuf[pWrite++]=key;
   if( pWrite >= KEY_BUF_LEN){
    pWrite %= KEY_BUF_LEN;
    KEY_BUF_FULL=1;
   }
*/
   return key;
  }
 }
}
/**********************************************************/
int KeyBoard::get_line(char *cmd)
{
 int i=0;
 while(1){
  cmd[i]=get_key();
  if(cmd[i]==13){
   cmd[i]=0;
   break;
  }
  printf("%c",cmd[i]);
  fflush(stdout);
  i++;
 }
 return (int)cmd;
}

int
KeyBoard::KBD_Open(void)
{
 int i;
// printf("open %d ing add by lyj_uptech\n\n",KEYBOARD);
 /* Open the keyboard and get it ready for use */
 fd = open(KEYBOARD, O_RDONLY | O_NONBLOCK);
// printf("open %d ing add by lyj_uptech ok! fd : %d \n\n",KEYBOARD,fd);

 if (fd < 0) {
  printf("%s - Can't open keyboard!\n", __FUNCTION__);
  return -1;
 }else
  printf("keyboard is opened\n");

 for (i=0; i<sizeof(scancodes)/sizeof(scancodes[0]); i++)
  scancodes[i]=MWKEY_UNKNOWN;

 for (i=0; i< sizeof(keymap)/sizeof(keymap[0]); i++)
 { scancodes[keymap[i].scancode]=keymap[i].mwkey;
//  printf("scancodes[%d]\n", keymap[i].scancode);
 }
 return fd;
}


void
KeyBoard::KBD_Close(void)
{
 close(fd);
 fd = -1;
}

void
KeyBoard::KBD_GetModifierInfo(MWKEYMOD * modifiers, MWKEYMOD * curmodifiers)
{
 if (modifiers)
  *modifiers = 0; /* no modifiers available */
 if (curmodifiers)
  *curmodifiers = 0;
}


int
KeyBoard::KBD_Read(char* kbuf, MWKEYMOD * modifiers, MWSCANCODE * scancode)
{
 int keydown = 0;
 int cc = 0;
 char buf/*,key*/;
 cc = read(fd, &buf, 1);
// printf("cc = %d\n",cc);
 if (cc < 0) {
  if ((errno != EINTR) && (errno != EAGAIN)
      && (errno != EINVAL)) {
   perror("KBD KEY");
   return (-1);
  } else {
   return (0);
  }
 }
// printf("KBD_Read begin\n");
 if (cc == 0)
  return (0);

 /* If the code is big than 127, then we know that */
 /* we have a key down.  Figure out what we've got */

 *modifiers = 0;

 if (buf & 0x80) {
  keydown = 1; /* Key pressed but not released */
 } else {
  keydown = 2; /* key released */
 }

 buf &= (~0x80);
 if( buf >= sizeof(scancodes) ) *kbuf = MWKEY_UNKNOWN;
 *scancode = scancodes[(int) buf];
      //  printf("scancodes[%d]\n",(int) buf);
 *kbuf = *scancode ;
// printf("%c",*kbuf);
// printf("by threewater: orgvalue=%d  key=%d  keystatus=%d, scancode=%d\n",buf, *kbuf,keydown, *scancode);
 return keydown;

}

