PRO invert_pixpos


; Read in the positions...
pospath = getenv('CALDB')+'/data/nustar/fpm/bcf/pixpos/'
pos_a_0 = mrdfits(pospath+'nuApixpos20100101v001.fits', 1, /silent)
pos_a_1 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 2, /silent)
pos_a_2 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 3, /silent)
pos_a_3 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 4, /silent)

pos_b_0 = mrdfits(pospath+'nuBpixpos20100101v001.fits', 1, /silent)
pos_b_1 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 2, /silent)
pos_b_2 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 3, /silent)
pos_b_3 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 4, /silent)


invert_struct = {rawx:intarr(10), rawy:intarr(10), det:intarr(10), grade:intarr(10),pdf:fltarr(10)}
invert_data = replicate(invert_struct, 360, 360)
grade_scale = [0.49, fltarr(4) + 0.1, fltarr(8) + 0.01]

lasttime = systime(1)
for thisx = 0, 359 do begin
   for thisy = 0, 359 do begin

      for p = 0, n_elements(pos_b_0)-1 do begin

         if pos_b_0[p].ref_det1x ne - 1 then begin

            x = thisx - pos_b_0[p].ref_det1x
            y = thisy - pos_b_0[p].ref_det1y

            if x ge 0 and x le 6 and $
               y ge 0 and y le 6 then begin
               thispdf = pos_b_0[p].pdf[x, y]
               
               if thispdf gt min(invert_data[thisx, thisy].pdf, minind) then begin
           
                  invert_data[thisx, thisy].rawx[minind] =  pos_b_0[p].rawx
                  invert_data[thisx, thisy].rawy[minind] = pos_b_0[p].rawy
                  invert_data[thisx, thisy].pdf[minind] = pos_b_0[p].pdf[x, y] * grade_scale[pos_b_0[p].grade]
                  invert_data[thisx, thisy].det[minind] = 0
                  invert_data[thisx, thisy].grade[minind] = pos_b_0[p].grade
               endif
            endif

         endif

         if pos_b_1[p].ref_det1x ne - 1 then begin

            x = thisx - pos_b_1[p].ref_det1x
            y = thisy - pos_b_1[p].ref_det1y
 
            if x ge 0 and x le 6 and $
               y ge 0 and y le 6 then begin 
               thispdf = pos_b_1[p].pdf[x, y]
               
               if thispdf gt min(invert_data[thisx, thisy].pdf, minind) then begin
           
                  invert_data[thisx, thisy].rawx[minind] =  pos_b_1[p].rawx
                  invert_data[thisx, thisy].rawy[minind] = pos_b_1[p].rawy
                  invert_data[thisx, thisy].pdf[minind] = pos_b_1[p].pdf[x, y] * grade_scale[pos_b_1[p].grade]
                  invert_data[thisx, thisy].det[minind] = 1
                  invert_data[thisx, thisy].grade[minind] = pos_b_1[p].grade
               endif
            endif

         endif

         if pos_b_2[p].ref_det1x ne - 1 then begin

            x = thisx - pos_b_2[p].ref_det1x
            y = thisy - pos_b_2[p].ref_det1y

            if x ge 0 and x le 6 and $
               y ge 0 and y le 6 then begin
               thispdf = pos_b_2[p].pdf[x, y]

               if thispdf gt min(invert_data[thisx, thisy].pdf, minind) then begin
           
                  invert_data[thisx, thisy].rawx[minind] =  pos_b_2[p].rawx
                  invert_data[thisx, thisy].rawy[minind] = pos_b_2[p].rawy
                  invert_data[thisx, thisy].pdf[minind] = pos_b_2[p].pdf[x, y] * grade_scale[pos_b_2[p].grade]
                  invert_data[thisx, thisy].det[minind] = 2
                  invert_data[thisx, thisy].grade[minind] = pos_b_2[p].grade
               endif
            endif

         endif

         if pos_b_3[p].ref_det1x ne - 1 then begin

            x = thisx - pos_b_3[p].ref_det1x
            y = thisy - pos_b_3[p].ref_det1y

            if x ge 0 and x le 6 and $
               y ge 0 and y le 6 then begin
               thispdf = pos_b_3[p].pdf[x, y]

               if thispdf gt min(invert_data[thisx, thisy].pdf, minind) then begin
                  
                  invert_data[thisx, thisy].rawx[minind] =  pos_b_3[p].rawx
                  invert_data[thisx, thisy].rawy[minind] = pos_b_3[p].rawy
                  invert_data[thisx, thisy].pdf[minind] = pos_b_3[p].pdf[x, y] * grade_scale[pos_b_3[p].grade]
                  invert_data[thisx, thisy].det[minind] = 3
                  invert_data[thisx, thisy].grade[minind] = pos_b_3[p].grade
               endif
            endif
         endif

      endfor

      if max(invert_data[thisx, thisy].pdf ) gt 0 then begin
;         print,thisx, thisy;, total(invert_data[thisx, thisy].pdf)
 

         sortInd = reverse(sort(invert_data[thisx, thisy].pdf))
         invert_data[thisx, thisy].rawx =  invert_data[thisx, thisy].rawx[sortind]
         invert_data[thisx, thisy].rawy =  invert_data[thisx, thisy].rawy[sortind]
         invert_data[thisx, thisy].det =  invert_data[thisx, thisy].det[sortind]
         invert_data[thisx, thisy].pdf =  invert_data[thisx, thisy].pdf[sortind] / total(invert_data[thisx, thisy].pdf)
          
         ;; for i = 0, 9 do print, invert_data[thisx, thisy].rawx[i], $
         ;;                        invert_data[thisx, thisy].rawy[i], $
         ;;                        invert_data[thisx, thisy].det[i], $
         ;;                        invert_data[thisx, thisy].grade[i], $
         ;;                        invert_data[thisx, thisy].pdf[i]
         
      endif

   endfor
   print, "This col:", thisx
   thistime = systime(1)
   print, "Time to do this column: ", thistime - lasttime
   lasttime = thistime
   save, invert_data, file = 'invert_fpmb.sav'

endfor

save, invert_data, file = 'invert_fpmb.sav'



END
