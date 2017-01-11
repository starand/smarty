#ifndef __TRANSPORT_H__INCLUDED
#define __TRANSPORT_h__INCLUDED

/*
Request format.
The first byte is a command :

 -- 0x00 -- get light status
 -- 0x01 -- turn on lights,  where the second byte is byte with lights.  In case
	    	all eight light is on, we have to use next command;
	-- 0x00 .. 0xFE -- bits of state, where every bit is  :  0 - if light is
		 of, 1 - if light is on;
 -- 0x02 -- turn off lights, where bits in  the  second  byte  determines  which
		light should be turned off.  See  previous  command for details.
 -- 0x03 -- set all lights, where 2nd  byte  determine if light will be tuned of
		or on.
	-- 0x00 .. 0x01 -- state : 0x00 - turn  off  all  lights, 0x01 - tun on;

Third - fifth  bytes  should be 0x00.  Those bytes is used to check if packet is
correct. The last bye always is 0xFF and determines the and of command. So every
command should consist of five bytes.

Request samples :

  0x00 0x00 0x00 0x00 0x00 0xFF -- get light statuses. See appropriate responses
				below.
  0x01 0x04 0x00 0x00 0x00 0xFF -- turn  on  third  light, because 0x04 is equal
				0b00000100 (third byte from right).
  0x02 0x09 0x00 0x00 0x00 0xFF -- turn off 1st and 4th lights. 0x09 is equal to
				 0b00001001.
  0x03 0x00 0x00 0x00 0x00 0xFF -- turn off all lights because  the second  byte
				is 0x00.
  0x04 0xFF 0x00 0x00 0x00 0xFF -- turn off  all buttons.  You can specify which
 				button to turn specifying second byte.
  0x05 0xFF 0x00 0x00 0x00 0xFF -- turn on buttons. Second  byte specifies which
 				button to turn on.


Response format.
The first and the second bytes  should  be  0x00.  The third byte should contain
light  statuses byte, where each bit is cresponsible for one light status.  Next
byte ( 4th ) is responsible for button states and each bit should contains state
of button is pressed - 1 if pressed, 0 if not.  The fifth byte should contain IR
control command if IR control  exists.
If command is wrong first two bytes in response should be 0xEE.


Response samples :

  0x00 0x00 0x05 0x04 0x00 -- the first and the second bytes should be 0x00; the
				third byte shows that 1st and 3rd light  are  on
				(0x05 = 0b00000101);
  0x00 0x00 0x00 0x00 0x09 -- lights and buttons are off, but the IR button with
				code 0x09 was pressed.
*/


#endif __TRANSPORT_H__INCLUDED
