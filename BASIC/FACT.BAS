func fact(n)
 if n=1 then return 1 end if
 return n*fact(n-1)
end func

? "factorielle\n"
while 1
 input "number", n
 if n<1 then bye end if
 ? fact(n)
wend
bye
