dim width,height,xofs,yofs
dim x,y
const psize=4 
xofs=0 
yofs=60 
width=1 
height=1 
t0=ticks()
video(0)
for x=0 to 240 step psize
  box(x+xofs,y+yofs,width*psize,height
*
psize,7)
next x
video(1)
? ticks()-t0

