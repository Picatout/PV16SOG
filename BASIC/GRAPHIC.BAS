while 1
 x=abs(rnd())%240
 y=abs(rnd())%170
 w=abs(rnd())%120
 h=abs(rnd())%75
 c=abs(rnd())%15+1
 select case abs(rnd())%3
 case 0
  rect(x,y,w,h,c)
 case 1
  box(x,y,w,h,c)
 case 2
  w=abs(rnd())%240
  h=abs(rnd())%170
  line(x,y,w,h,c)
 end select
 if key() then cls: bye end if
wend
