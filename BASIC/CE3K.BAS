dim ce3k#(10)=(13,200,15,200,11,200,
1,200,6,200)

? "Close encounter of the 3rd kind."

for i=1 to ubound(ce3k#) step 2
 tone(ce3k#(i)+12,3*ce3k#(i+1),1)
next i
