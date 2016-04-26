while 1
 x=abs(rnd())%240
 y=abs(rnd())%170
 w=abs(rnd())%120
 h=abs(rnd())%75
 c=abs(rnd())%15+1
 rect(x,y,w,h,c)
 if key() then cls: bye end if
wend
