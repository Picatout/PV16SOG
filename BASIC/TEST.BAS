use "lem.spr"
dim spd,dx,dy, x=112, y=67
cls

sub rnd_dir()
 dx=0
 dy=0
 while dx=0 and dy=0
  dx=rnd()%2
  dy=rnd()%2
 wend
end sub

rnd_dir()
spd=20
while 1
 sprite(x,y,16,16,@lem#)
 pause(spd)
 sprite(x,y,16,16,@lem#)
 x=x+dx
 y=y+dy
 if x<1 or x>224 then
   dx=-dx
   tone(10,80,0)
 end if
 if y<1 or y>154 then
   dy=-dy
   tone(10,80,0)
 end if
 select case key()
 case 27 ' ESC, quitte
  cls
  bye
 case 32 'SPACE, nouvelle direction
  rnd_dir()
 case \+ ' augmente vitesse
  if spd>1 then spd=spd-1 end if
 case \- ' diminu vitesse
  if spd<50 then spd=spd+1 end if
 end select
wend


