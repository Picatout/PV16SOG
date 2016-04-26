' test vitesse SPI RAM
const size=10000
dim d#(size)
t0=ticks()
srread(0,@d#,size)
t0=ticks()-t0
? "read",t0
t0=ticks()
srwrite(0,@d#,size)
t0=ticks()-t0
? "write",t0
