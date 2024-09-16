macro org addr
  if $ = 0
     org addr
  else if addr >= $
     rb addr - $
  else
    display "origin error", 0xd, 0xa
  end if
end macro
