use "joystick.inc"
cls
? "test select case"
while 1
 select case jstick()
 case button
  bye
 case right
  ? "E";
 case left
  ? "O";
 case down
  ? "S";
 case up
  ? "N";
 case upright
  ? "NE";
 case upleft
  ? "NO";
 case downright
  ? "SE";
 case downleft
  ? "SO";
 end select
 pause(17)
wend

