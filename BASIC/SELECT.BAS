cls
? "test select case"
while 1
 select case jstick()
 case 1
  bye
 case 2
  ? "E";
 case 4
  ? "O";
 case 8
  ? "S";
 case 16
  ? "N";
 case 18
  ? "NE";
 case 20
  ? "NO";
 case 10
  ? "SE";
 case 12
  ? "SO";
 end select
 pause(17)
wend

