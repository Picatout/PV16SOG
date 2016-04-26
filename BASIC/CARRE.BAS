rem test de recursion
rem carre fractal
cls
lm=(240-3*56)/2
sub fractal(x,y,n)
local x0,y0
 if n<=3 then
  for x0=x to x+2*n step n
   for y0=y to y+2*n step n
    if x0<>x+n or y0<>y+n then
      box(lm+x0,y0,n,n,15)
    end if       
   next y0
  next x0
 else
  for x0=x to x+2*n step n
   for y0=y to y+2*n step n
    if x0<>x+n or y0<>y+n then
     fractal(x0,y0,n/3)
    end if
   next y0
  next x0
 end if
end sub
fractal(0,0,56)
while not key() wend
cls

