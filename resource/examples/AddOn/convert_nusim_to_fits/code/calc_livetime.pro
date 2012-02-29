function calc_livetime, time, prior, tstart = tstart, live2 = live2
; Starting with the first integral second, compute the livetime per
; second over the data set. This accounts for livetimes that bridge
; between seconds.

if ~keyword_set(tstart) then tstart = floor(min(time))    ; assume seconds


thist = histogram(time, min = tstart, binsize = 1.0, $
                  reverse_indices = tinds)

live2 = 1.d - thist * 2.5e-3

nsec = n_elements(thist) 
livetime = fltarr(nsec)
for t = 0l, nsec - 1 do begin
   if thist[t] eq 0 then begin
;      livetime[t] = 1.
      continue
   endif

   thisbin = tinds[tinds[t]:tinds[t+1]-1]
   
   for i = 0l, thist[t] -1 do begin

      residual = prior[thisbin[i]] - (time[thisbin[i]] mod 1)
      if residual lt 0 then livetime[t] += prior[thisbin[i]] else begin
         livetime[t] += time[thisbin[i]] mod 1
         rev = 0
         while residual gt 0l do begin
            rev++
            if t - rev lt 0 then break
            if residual gt 1 then livetime[t-rev] += 1 $
            else livetime[t-rev] += residual
            residual--
          
         endwhile
;         if rev gt 1 then stop
      endelse
      ;; if time[thisbin[i]] mod 1 gt prior[thisbin[i]] then livetime[t] += prior[thisbin[i]] $
      ;;    else begin
         
      ;;    if prior[thisbin[i]] gt 1

      ;;    livetime[t] += time[thisbin[i]] mod 1
      ;;    livetime[t-1] += prior[thisbin[i]] - (time[thisbin[i]] mod 1)
      ;; endelse
   endfor
endfor


return, livetime

end

