' test sprite
use "lem.spr"
use "joystick.inc"

cls
dim x=112,y=77

sub move_left()
 if x> 0 then x=x-1 end if
end sub

sub move_right()
 if x<224 then x=x+1 end if
end sub

sub move_up()
  if y>0 then y=y-1 end if
end sub

sub move_down()
  if y<154 then y=y+1 end if
end sub

while 1
 void=sprite(x,y,16,16,@lem#)
 pause(20)
 vod=sprite(x,y,16,16,@lem#)
 select case jstick()
 case UP
  move_up()
 case DOWN
  move_down()
 case RIGHT
  move_right()
 case LEFT
  move_left()
 case UPRIGHT
  move_up()
  move_right()
 case UPLEFT
  move_up()
  move_left()
 case DOWNRIGHT
  move_down()
  move_right()
 case DOWNLEFT
  move_down()
  move_left()
 case BUTTON
  cls
  bye
 end select
wend
