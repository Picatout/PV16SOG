' pour eliste, Beethoven
dim score#(28)=(27,100,26,100,27,100,
26,100,27,100,22,100,25,100,23,100,
20,100,15,100,8,100,11,100,15,100,
23,100)

for i=1 to ubound(score#) step 2
 tone(score#(i)+12,3*score#(i+1),1)
next i
