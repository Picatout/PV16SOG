rem test spiram
dim a#(5)=(\h,\e,\l,\l,\o)
dim b#(5)
srwrite(0,@a#,5)
srread(0,@b#,5)
putc 13
for i=1 to 5 
 putc b#(i)
next i
srclear(0,5)
srread(0,@a#,5)
putc 13
for i=1 to 5
 ? a#(i);
next i
