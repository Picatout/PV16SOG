while 1
 x=abs(rnd())%240
 y=abs(rnd())%170
 x1=abs(rnd())%240
 y1=abs(rnd())%170
 c=abs(rnd())%15+1
 line(x,y,x1,y1,c)
 if key() then cls: bye end if
wend
