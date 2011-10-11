PRO write_nusim_events


COMMON nusim_data, data, err, header_data


conv_path = getenv('CONV_PATH')
;print, conv_path
outpath = getenv('OUTPATH')
;print, outpath
;stop
;path = '/users/bwgref/nusim/'
;outpath = '/home/nustar1/Web'
;outpath = '/users/bwgref/nusim/DC1/'+header_data.obsid+'/event_uf/'
outpath = outpath +'/'+header_data.obsid+'/event_uf'
print, outpath

file_mkdir, outpath

; Read in the template for the L1A file:
template_a = conv_path+'/templates/NUSTAR_L2_cold4_CPT_heatcheck_EVENTS_FPMA.fits'
template_b = conv_path+'/templates/NUSTAR_L2_cold4_CPT_heatcheck_EVENTS_FPMB.fits'
; Get the headers...
data_a = mrdfits(template_a,'EVENTS', header_a, range = [0, 10], /silent)
data_b = mrdfits(template_b, 'EVENTS', header_b, range = [0, 10], /silent)


; Get the primary header
primary_header_a = headfits(template_a, exten = 0)
primary_header_b = headfits(template_b, exten = 0)

; Setup the output file names
out_a = outpath+'/nu'+header_data.obsid+'A01.evt'
out_b = outpath+'/nu'+header_data.obsid+'B01.evt'

print, out_a
print, out_b

; Rewrite the date to be the "NUSIM" test:

; Seconds start:
; Read from NUSIM output
date_start = header_data.date_start

; This is in NuSTAR epoch time (relative to Jan 1, 2010), 
; which is how it should be...
data.time += header_data.tstart

; Figure out the last event, add 1 seconds, and declare this
; the end of the observation:
t_ref = date_conv('2010-01-01T00:00:00', 'Modified')
t_ref = cmsystime(t_ref, /from_mjd, /sec)
date_end = date_conv(cmsystime(ceil(max(data.time + 1.)+t_ref), /julian), 'FITS')
date_end = (strsplit(date_end, '.', /extract))[0]


;tstart = cmsystime(tstart, /from_mjd, /sec) - t_ref
;tend = cmsystime(tend, /from_mjd, /sec) - t_ref + 1
tstart = cmsystime(date_conv(date_start, 'Modified'), /from_mjd, /sec) - t_ref
tend = cmsystime(max(data.time + 1.), /sec)


; For now, edit everyting below by hand if you change the time of the observation...
tstart_string = string(tstart, format = '(i0)')
tend_string = string(tend, format = '(i0)')
;print, tstart ; 66916800
;print, tend ; 66920400
;stop

FOR h = 0, n_elements(primary_header_a) DO BEGIN
   IF stregex(primary_header_a[h], "TSTART", /boolean) THEN BREAK
ENDFOR

primary_header_a[h] = 'TSTART  =            '+tstart_string+'. / Start time (seconds since Jan 2010 00:00:00 UTC'
primary_header_a[h+1] = 'TSTOP   =            '+tend_string+'. / Stop time (seconds since Jan 2010 00:00:00 UTC)'
primary_header_a[h+2] = "DATE-OBS= '"+date_start+"' / Start date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC"
primary_header_a[h+3] = "DATE-END= '"+date_end+"' / Stop date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC)"

FOR h = 0, n_elements(header_a) DO BEGIN
   IF stregex(header_a[h], "TSTART", /boolean) THEN BREAK
ENDFOR
header_a[h] = 'TSTART  =            '+tstart_string+'. / Start time (seconds since Jan 2010 00:00:00 UTC'
header_a[h+1] = 'TSTOP   =            '+tend_string+'. / Stop time (seconds since Jan 2010 00:00:00 UTC)'
header_a[h+2] = "DATE-OBS= '"+date_start+"' / Start date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC"
header_a[h+3] = "DATE-END= '"+date_end+"' / Stop date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC)"



FOR h = 0, n_elements(primary_header_a) DO BEGIN
   IF stregex(primary_header_a[h], "TSTART", /boolean) THEN BREAK
ENDFOR

primary_header_b[h] = 'TSTART  =            '+tstart_string+'. / Start time (seconds since Jan 2010 00:00:00 UTC'
primary_header_b[h+1] = 'TSTOP   =            '+tend_string+'. / Stop time (seconds since Jan 2010 00:00:00 UTC)'
primary_header_b[h+2] = "DATE-OBS= '"+date_start+"' / Start date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC"
primary_header_b[h+3] = "DATE-END= '"+date_end+"' / Stop date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC)"


FOR h = 0, n_elements(header_b) DO BEGIN
   IF stregex(header_b[h], "TSTART", /boolean) THEN BREAK
ENDFOR
header_b[h] = 'TSTART  =            '+tstart_string+'. / Start time (seconds since Jan 2010 00:00:00 UTC'
header_b[h+1] = 'TSTOP   =            '+tend_string+'. / Stop time (seconds since Jan 2010 00:00:00 UTC)'
header_b[h+2] = "DATE-OBS= '"+date_start+"' / Start date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC"
header_b[h+3] = "DATE-END= '"+date_end+"' / Stop date/time, e.g. (YYYY-MM-DDThh:mm:ss UTC)"




; Write the primary header
writefits, out_a, 0, primary_header_a
writefits, out_b, 0, primary_header_b

;
;
;
;
;
;
;
;
; Okay, done with the headers. Now to move on the events...
;
;
;
;
;
;
;
;
;
;; gainpath = getenv('CALDB')+'/data/nustar/fpm/bcf/gain/'

;; ; Get the gains...
;; gain_a_0 = mrdfits(gainpath+'nuAgain20100101v001.fits', 1, /silent)
;; gain_a_1 =  mrdfits(gainpath+'nuAgain20100101v001.fits', 2, /silent)
;; gain_a_2 =  mrdfits(gainpath+'nuAgain20100101v001.fits', 3, /silent)
;; gain_a_3 =  mrdfits(gainpath+'nuAgain20100101v001.fits', 4, /silent)
;; gain_b_0 = mrdfits(gainpath+'nuBgain20100101v001.fits', 1, /silent)
;; gain_b_1 =  mrdfits(gainpath+'nuBgain20100101v001.fits', 2, /silent)
;; gain_b_2 =  mrdfits(gainpath+'nuBgain20100101v001.fits', 3, /silent)
;; gain_b_3 =  mrdfits(gainpath+'nuBgain20100101v001.fits', 4, /silent)

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


; Define the transformation from the DET1 origin to the
; FPM origin. This is in the alignments FITS file, but
; we can just use the real number here. FPM is offset wrt
; DET1 by 180 subcells, or...
fpm_res = 604.8 / 5.
fpm_origin = 180 * fpm_res; microns


; Trim the event data to just a single example of each...
data_a = data_a[0]
data_b = data_b[0]


; Sort into FPMA and FPMB data from the NuSIM input...
fpma = where(data.fpm EQ 1, nfpma)
fpmb = where(data.fpm EQ 2, nfpmb)

; Replicate the L1A arrays into the right number of events...
data_a = replicate(data_a, nfpma)
data_b = replicate(data_b, nfpmb)



; Now for the hard part...

; This is the main loop where you do all the heavy lifting...
use = fltarr(nfpma)
FOR i = 0, nfpma - 1 DO BEGIN
   ; First step, figure out which pixel/grade you've hit...

   IF i MOD 100 EQ 0 THEN print, i, " of ", nfpma

   ; Convert to DET1 coordinates...
   thisx = round( (data[fpma[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpma[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)
;   print, i, thisx, thisy
   ; Figure out any pos_b_N that has the distance between ref_det1x
   ; and ref_det1y greater than zero and less than 7 software pixels
   ; away...
   IF n_elements(tocheck) GT 0 THEN BEGIN
      foo = temporary(tocheck) 
      foo = temporary(found)
   ENDIF


   FOR det = 0, 3 DO BEGIN
      CASE det OF
         0: BEGIN
            inrange = where(thisx - pos_a_0.ref_det1x Ge 0 AND thisx - pos_a_0.ref_det1x LT 7 AND $
                            thisy - pos_a_0.ref_det1y Ge 0 AND thisy - pos_a_0.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               IF n_elements(totcheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound)
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound)]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
         1: BEGIN
            inrange = where(thisx - pos_a_1.ref_det1x Ge 0 AND thisx - pos_a_1.ref_det1x LT 7 AND $
                            thisy - pos_a_1.ref_det1y Ge 0 AND thisy - pos_a_1.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 1
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 1]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF

         END
         2: BEGIN
            inrange = where(thisx - pos_a_2.ref_det1x Ge 0 AND thisx - pos_a_2.ref_det1x LT 7 AND $
                            thisy - pos_a_2.ref_det1y Ge 0 AND thisy - pos_a_2.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 2
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 2]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
         3: BEGIN
            inrange = where(thisx - pos_a_3.ref_det1x Ge 0 AND thisx - pos_a_3.ref_det1x LT 7 AND $
                            thisy - pos_a_3.ref_det1y Ge 0 AND thisy - pos_a_3.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 3
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 3]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
      ENDCASE
   ENDFOR


   ; Okay, now throw this count into one of those bins...

   nfound = n_elements(tocheck)

   IF nfound EQ 0 THEN continue



   prob = fltarr(nfound)
   rawpos = fltarr(nfound, 2)
   grade = fltarr(nfound)
   FOR n = 0, nfound - 1 DO BEGIN
      CASE found[n] OF
         0: BEGIN
            offx = floor(thisx - pos_a_0[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_a_0[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_a_0[tocheck[n]].rawx
            rawpos[n, 1] = pos_a_0[tocheck[n]].rawy
            grade[n] = pos_a_0[tocheck[n]].grade
            prob[n] = pos_a_0[tocheck[n]].pdf[offx, offy]
         END
         1: BEGIN
            offx = floor(thisx - pos_a_1[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_a_1[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_a_1[tocheck[n]].rawx
            rawpos[n, 1] = pos_a_1[tocheck[n]].rawy
            grade[n] = pos_a_1[tocheck[n]].grade
            prob[n] = pos_a_1[tocheck[n]].pdf[offx, offy]    
         END
         2: BEGIN
            offx = floor(thisx - pos_a_2[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_a_2[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_a_2[tocheck[n]].rawx
            rawpos[n, 1] = pos_a_2[tocheck[n]].rawy
            grade[n] = pos_a_2[tocheck[n]].grade
            prob[n] = pos_a_2[tocheck[n]].pdf[offx, offy]  
         END
         3: BEGIN
            offx = floor(thisx - pos_a_3[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_a_3[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_a_3[tocheck[n]].rawx
            rawpos[n, 1] = pos_a_3[tocheck[n]].rawy
            grade[n] = pos_a_3[tocheck[n]].grade
            prob[n] = pos_a_3[tocheck[n]].pdf[offx, offy]  
         END
      ENDCASE
   ENDFOR

   ; Throw away ones that can't happen (prob == 0)
   goodones = where(prob GT 0, ngood)
   IF ngood EQ 0 THEN continue

   ; If you make it here, use me!
   use[i] = 1

   found = found[goodones]
   prob = prob[goodones]
   rawpos = rawpos[goodones, *]
   grade = grade[goodones]
   

   ; Here we do an artifical toss into a particular grade, since the
   ; pixpos files aren't rescaled across each other...
   FOR n = 0, ngood - 1 DO BEGIN
      IF grade[n] EQ 0 THEN grade[n] *= 0.5
      IF grade[n] GT 0 AND grade[n] LE 4 THEN prob[n] *= 0.09
      IF grade[n] GT 4 AND grade[n] LE 12 THEN prob[n] *= 0.01
   ENDFOR

   ; Now, sort based on prob:
   sortInd = sort(prob)
   prob = prob[sortInd]
   rawpos = rawpos[sortInd, *]
   grade = grade[sortInd]
   found = found[sortInd]

   ; Do a cumulative sum:
   prob = total(prob, /cumu)
   prob /= max(prob)
   
   ; Pick one...
   thisone = min(where(prob GE randomu(seed)))

   rawpos = reform(rawpos[thisone, *])
   grade = reform(grade[thisone])

   data_a[i].rawx = rawpos[0]
   data_a[i].rawy = rawpos[1]
   data_a[i].det_id = found[thisone]
   data_a[i].grade = grade
   data_a[i].pi = round(data[fpma[i]].energy * 100.)
   data_a[i].prior = data[fpma[i]].livetime
   
   
   ; Skip below and just assign the PI:
   ;; CASE found[thisone] OF
   ;;    0: BEGIN
   ;;       thisgain = where(gain_a_0.rawx EQ data_a[i].rawx AND $
   ;;                        gain_a_0.rawy EQ data_a[i].rawy)
   ;;       data_a[i].pha = (data[fpma[i]].energy * 100 - $
   ;;                        gain_a_0[thisgain].offset[13] ) / $
   ;;                       gain_a_0[thisgain].slope[13]
   ;;    END
   ;;    1: BEGIN
   ;;       thisgain = where(gain_a_1.rawx EQ data_a[i].rawx AND $
   ;;                        gain_a_1.rawy EQ data_a[i].rawy)
   ;;       data_a[i].pha = ( data[fpma[i]].energy * 100 - $
   ;;                         gain_a_1[thisgain].offset[13] ) / $
   ;;                       gain_a_1[thisgain].slope[13]
   ;;    END
   ;;    2: BEGIN
   ;;       thisgain = where(gain_a_2.rawx EQ data_a[i].rawx AND $
   ;;                        gain_a_2.rawy EQ data_a[i].rawy)
   ;;       data_a[i].pha = ( data[fpma[i]].energy * 100 - $
   ;;                         gain_a_2[thisgain].offset[13] ) / $
   ;;                       gain_a_2[thisgain].slope[13]
   ;;    END
   ;;    3: BEGIN
   ;;       thisgain = where(gain_a_3.rawx EQ data_a[i].rawx AND $
   ;;                        gain_a_3.rawy EQ data_a[i].rawy)
   ;;       data_a[i].pha = ( data[fpma[i]].energy * 100 - $
   ;;                         gain_a_3[thisgain].offset[13] ) / $
   ;;                       gain_a_3[thisgain].slope[13]
   ;;    END
   ;; ENDCASE
ENDFOR

data_a = data_a[where(use EQ 1)]
; Write the EVENT data and the EVENT header
mwrfits, data_a, out_a, $
         header_a, /no_types, /no_comment

print, 'Done with FPMA...'

foo = temporary(found)
foo = temporary(tocheck)


; This is the main loop where you do all the heavy lifting...
use = fltarr(nfpmb)
FOR i = 0, nfpmb - 1 DO BEGIN

   ; First step, figure out which pixel/grade you've hit...
   IF i MOD 100 EQ 0 THEN print, i, " of ", nfpmb

   ; Convert to DET1 coordinates...
   thisx = round( (data[fpmb[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpmb[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)
   IF n_elements(tocheck) GT 0 THEN BEGIN
      foo = temporary(tocheck) 
      foo = temporary(found)
   ENDIF

   ; Figure out any pos_b_N that has the distance between ref_det1x
   ; and ref_det1y greater than zero and less than 7 software pixels
   ; away...
   FOR det = 0, 3 DO BEGIN
      CASE det OF
         0: BEGIN
            inrange = where(thisx - pos_b_0.ref_det1x Ge 0 AND thisx - pos_b_0.ref_det1x LT 7 AND $
                            thisy - pos_b_0.ref_det1y Ge 0 AND thisy - pos_b_0.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               IF n_elements(totcheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound)
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound)]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
         1: BEGIN
            inrange = where(thisx - pos_b_1.ref_det1x Ge 0 AND thisx - pos_b_1.ref_det1x LT 7 AND $
                            thisy - pos_b_1.ref_det1y Ge 0 AND thisy - pos_b_1.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 1
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 1]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF

         END
         2: BEGIN
            inrange = where(thisx - pos_b_2.ref_det1x Ge 0 AND thisx - pos_b_2.ref_det1x LT 7 AND $
                            thisy - pos_b_2.ref_det1y Ge 0 AND thisy - pos_b_2.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 2
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 2]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
         3: BEGIN
            inrange = where(thisx - pos_b_3.ref_det1x Ge 0 AND thisx - pos_b_3.ref_det1x LT 7 AND $
                            thisy - pos_b_3.ref_det1y Ge 0 AND thisy - pos_b_3.ref_det1y LT 7, nfound)
            IF nfound GT 0 THEN BEGIN
               if n_elements(tocheck) EQ 0 THEN BEGIN
                  found = fltarr(nfound) + 3
                  tocheck = inrange
               ENDIF ELSE BEGIN
                  found = [found, fltarr(nfound) + 3]
                  tocheck = [tocheck, inrange]
               ENDELSE
            ENDIF
         END
      ENDCASE
   ENDFOR


   ; Okay, now throw this count into one of those bins...

   nfound = n_elements(found)
   IF nfound EQ 0 THEN BEGIN
;      print, 'Nothing found here for', i
      CONTINUE
   ENDIF


   prob = fltarr(nfound)
   rawpos = fltarr(nfound, 2)
   grade = fltarr(nfound)
   FOR n = 0, nfound - 1 DO BEGIN
      CASE found[n] OF
         0: BEGIN
            offx = floor(thisx - pos_b_0[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_b_0[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_b_0[tocheck[n]].rawx
            rawpos[n, 1] = pos_b_0[tocheck[n]].rawy
            grade[n] = pos_b_0[tocheck[n]].grade
            prob[n] = pos_b_0[tocheck[n]].pdf[offx, offy]  

         END
         1: BEGIN
            offx = floor(thisx - pos_b_1[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_b_1[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_b_1[tocheck[n]].rawx
            rawpos[n, 1] = pos_b_1[tocheck[n]].rawy
            grade[n] = pos_b_1[tocheck[n]].grade
            prob[n] = pos_b_1[tocheck[n]].pdf[offx, offy]
         ENd
         2: BEGIN
            offx = floor(thisx - pos_b_2[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_b_2[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_b_2[tocheck[n]].rawx
            rawpos[n, 1] = pos_b_2[tocheck[n]].rawy
            grade[n] = pos_b_2[tocheck[n]].grade
            prob[n] = pos_b_2[tocheck[n]].pdf[offx, offy]  


         END
         3: BEGIN

            offx = floor(thisx - pos_b_3[tocheck[n]].ref_det1x)
            offy = floor(thisy - pos_b_3[tocheck[n]].ref_det1y)
            rawpos[n, 0] = pos_b_3[tocheck[n]].rawx
            rawpos[n, 1] = pos_b_3[tocheck[n]].rawy
            grade[n] = pos_b_3[tocheck[n]].grade
            prob[n] = pos_b_3[tocheck[n]].pdf[offx, offy]  

         END
      ENDCASE
   ENDFOR

   ; Throw away ones that can't happen (prob == 0)
   goodones = where(prob GT 0, ngood)
   IF ngood EQ 0 THEN BEGIN
;      print, 'No likely pixels for ', i
      CONTINUE
   ENDIF

   ; If you made it here, use me!
   use[i] = 1

   found = found[goodones]
   prob = prob[goodones]
   rawpos = rawpos[goodones, *]
   grade = grade[goodones]
   ; Account for the fact that the PDFs are normalized. Grade 0 really
   ; accounts for about half the counts, 1-4 about 9% each, and the
   ; rest in grade 4-12. This is fudgy, but it should smooth out the
   ; "hot corner" issue that I've been seeing...
   FOR n = 0, ngood - 1 DO BEGIN
      IF grade[n] EQ 0 THEN grade[n] *= 0.5
      IF grade[n] GT 0 AND grade[n] LE 4 THEN prob[n] *= 0.09
      IF grade[n] GT 4 AND grade[n] LE 12 THEN prob[n] *= 0.01
   ENDFOR

   ; Now, sort based on prob:
   sortInd = sort(prob)
   prob = prob[sortInd]
   rawpos = rawpos[sortInd, *]
   grade = grade[sortInd]
   found = found[sortInd]

   ; Do a cumulative sum:
   prob = total(prob, /cumu)
   prob /= max(prob)
   
   ; Pick one...
   thisone = min(where(prob GE randomu(seed)))

   rawpos = reform(rawpos[thisone, *])
   grade = reform(grade[thisone])

   data_b[i].rawx = rawpos[0]
   data_b[i].rawy = rawpos[1]
   data_b[i].det_id = found[thisone]
   data_b[i].grade = grade   
   data_b[i].pi = round(data[fpmb[i]].energy * 100.)
   data_b[i].prior = data[fpmb[i]].livetime

 
   

   ;; CASE found[thisone] OF
   ;;    0: BEGIN
   ;;       thisgain = where(gain_b_0.rawx EQ data_b[i].rawx AND $
   ;;                        gain_b_0.rawy EQ data_b[i].rawy)
   ;;       data_b[i].pha = (data[fpmb[i]].energy * 100 - $
   ;;                        gain_b_0[thisgain].offset[13] ) / $
   ;;                       gain_b_0[thisgain].slope[13]
   ;;    END
   ;;    1: BEGIN
   ;;       thisgain = where(gain_b_1.rawx EQ data_b[i].rawx AND $
   ;;                        gain_b_1.rawy EQ data_b[i].rawy)
   ;;       data_b[i].pha = ( data[fpmb[i]].energy * 100 - $
   ;;                         gain_b_1[thisgain].offset[13] ) / $
   ;;                       gain_b_1[thisgain].slope[13]
   ;;    END
   ;;    2: BEGIN
   ;;       thisgain = where(gain_b_2.rawx EQ data_b[i].rawx AND $
   ;;                        gain_b_2.rawy EQ data_b[i].rawy)
   ;;       data_b[i].pha = ( data[fpmb[i]].energy * 100 - $
   ;;                         gain_b_2[thisgain].offset[13] ) / $
   ;;                       gain_b_2[thisgain].slope[13]
   ;;    END
   ;;    3: BEGIN
   ;;       thisgain = where(gain_b_3.rawx EQ data_b[i].rawx AND $
   ;;                        gain_b_3.rawy EQ data_b[i].rawy)
   ;;       data_b[i].pha = ( data[fpmb[i]].energy * 100 - $
   ;;                         gain_b_3[thisgain].offset[13] ) / $
   ;;                       gain_b_3[thisgain].slope[13]
   ;;    END
   ;; ENDCASE

ENDFOR

data_b = data_b[where(use EQ 1)]
mwrfits, data_b, out_b, $
         header_b, /no_types, /no_comment

spawn, 'chmod 664 '+out_a
spawn, 'chmod 664 '+out_b
spawn, 'chown nustar '+out_a
spawn, 'chown nustar '+out_b
END
