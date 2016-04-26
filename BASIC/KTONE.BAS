rem clavier musical
while 1
  k=key()
  if k=27 then bye end if
  if k>=97 and k<=97+25 then
    tone(k-81,100,0)
  end if
wend