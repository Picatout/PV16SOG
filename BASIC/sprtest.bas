' test sprite
dim lem#(64)=(
$00,$00,$00,$ff,$ff,$00,$00,$00
$00,$00,$0f,$f0,$0f,$f0,$00,$00
$00,$0f,$ff,$ff,$ff,$ff,$f0,$00
$00,$ff,$ff,$ff,$ff,$ff,$ff,$00
$00,$f0,$00,$00,$00,$00,$0f,$00
$f0,$f0,$ff,$ff,$ff,$ff,$0f,$0f
$0f,$f0,$ff,$00,$00,$ff,$0f,$f0
$00,$f0,$ff,$00,$00,$ff,$0f,$00
$0f,$f0,$ff,$ff,$ff,$ff,$0f,$f0
$f0,$f0,$00,$00,$00,$00,$0f,$0f
$00,$ff,$ff,$ff,$ff,$ff,$ff,$00
$00,$ff,$ff,$ff,$ff,$ff,$ff,$00
$00,$0f,$00,$f0,$f0,$0f,$00,$00
$00,$f0,$0f,$00,$0f,$00,$f0,$00
$0f,$f0,$00,$00,$00,$00,$ff,$00
$ff,$00,$00,$00,$00,$00,$0f,$f0
)

dim btn,x=112,y=77
while 1
 sprite(x,y,16,16,@lem#)
 pause(20)
 sprite(x,y,16,16,@lem#)
 btn=jstick()
 if btn=1 then bye end if
 if btn=2 then x=(x+1)%224 end if
 if btn=4 then 
  if x>0 then x=x-1 end if
 end if
 if btn=8 then 
  if y>0 then y=y-1 end if
 end if
 if btn=16 then y=(y+1) % 154 end if
wend



