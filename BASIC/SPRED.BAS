REM editeur de sprite

dim name$, w, h, c=0
dim spr#(512)
dim c#(8)=($f0,$0f,
           $0f,$f0,
           $0f,$f0,
           $f0,$0f)
dim xo,yo,x,y
const psize=4
const xres=240
const yres=170
cls
input "sprite name", name$
input "sprite width", w
input "sprite height", h

sub draw_bounds()
 cls
 xo=(xres-psize*w)/2
 yo=(yres-psize*h)/2
 rect(xo-1,yo-1,psize*w+2,psize*h+2,7)
 x=w/2
 y=h/2
end sub

sub prtxy()
  locate(0,0)
  print "x:",x," y:",y ;
end sub

sub save_sprite()
  srwrite(0,@w,2)
  srwrite(2,@h,2)
  srwrite(4,@spr#,w*h/2)
  srsave name$,w*h/2+4
  locate(19,0)
  ? name$ ,"saved"
  ? w,"x",h,"pixels";
end sub

sub fill_canevas()
local i,x,y,c
  for x=0 to w-1
   for y=0 to h-1
    i=(y*w+x)/2+1
    if btest(x,0) then
      c=spr#(i)%16
    else
      c=spr#(i)/16
    end if
    box(xo+x*psize,yo+y*psize,psize,
    psize,c)
    setpixel(10+x,10+y,c)
   next y
  next x
end sub

sub load_sprite()
local size
 size=srload name$
 if size then
   srread(0,@w,2)
   srread(2,@h,2)
   srread(4,@spr#,size-4)
   draw_bounds()
   fill_canevas()
   locate(19,0)
   ? name$, "loaded"
   ? "width",w,"height",h ;
 else
   locate(19,0)
   ? "load failed";
 end if
end sub

sub let_pixel(x,y,c)
local idx
  idx=(y*w+x)/2+1
  if btest(x,0) then 'impair
   spr#(idx)=spr#(idx)/16*16+c
  else ' pair
   spr#(idx)=spr#(idx)%16+c*16
  end if
end sub


draw_bounds()

while 1
 prtxy()
 sprite(xo+x*psize,yo+y*psize,psize,
 psize,@c#)
 pause(20)
 sprite(xo+x*psize,yo+y*psize,psize,
 psize,@c#)
 k=key()
 select case k
 case \0,\1,\2,\3,\4,\5,\6,\7,\8,\9
  c=k-\0
 case \a,\b,\c,\d,\e,\f
  c=k-\a+10
 case 32
  box(xo+x*psize,yo+y*psize,psize,
  psize,c)
  setpixel(10+x,10+y,c)
  let_pixel(x,y,c)
 case 141
  if y>0 then y=y-1 end if
 case 142
  if y<h-1 then y=y+1 end if
 case 143
  if x>0 then x=x-1 end if
 case 144
  if x<h-1 then x=x+1 end if
 case 27 ' quit
  cls
  bye
 case \s ' save
  save_sprite()
 case \l' load
  load_sprite()
 end select
wend

