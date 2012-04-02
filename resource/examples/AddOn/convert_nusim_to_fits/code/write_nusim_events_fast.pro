PRO write_nusim_events_fast


COMMON nusim_data, data, err, header_data, infile

; Max number of count dues to memory issues...
limit = 2.5e6

conv_path = getenv('CONV_PATH')
;print, conv_path
outpath = getenv('OUTPATH')+'/'+file_basename(infile, '.dat')

outpath = outpath +'/'+header_data.obsid+'/event_uf'
print, outpath

file_mkdir, outpath

; Read in the template for the L1A file:
template_a = conv_path+'/templates/NUSTAR_L2_post_env_stim_HV_on_2_EVENTS_FPMA.fits'
template_b = conv_path+'/templates/NUSTAR_L2_post_env_stim_HV_on_2_EVENTS_FPMB.fits'
; Get the headers and the data formats
data_a = mrdfits(template_a,'EVENTS', header_a, range = [0, 10], /silent)
data_b = mrdfits(template_b, 'EVENTS', header_b, range = [0, 10], /silent)

; Get the GTI formats
gti_a = mrdfits(template_a, 'GTI', gti_header_a, /silent)
gti_b = mrdfits(template_b, 'GTI', gti_header_b, /silent)


; Get the primary header
primary_header_a = headfits(template_a, exten = 0)
primary_header_b = headfits(template_b, exten = 0)

; Setup the output file names
out_a = outpath+'/nu'+header_data.obsid+'A_uf.evt'
out_b = outpath+'/nu'+header_data.obsid+'B_uf.evt'
;bad_a = outpath+'/nu'+header_data.obsid+'A_bad.sav'
;bad_b = outpath+'/nu'+header_data.obsid+'B_bad.sav'
badfile = outpath+'/nu'+header_data.obsid+'_bad.sav'

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

tstart = cmsystime(date_conv(date_start, 'Modified'), /from_mjd, /sec) - t_ref
tend = cmsystime(max(data.time + 1.), /sec)


; Overwrite primary header keywords for time stamps:
tstart_string = string(tstart, format = '(i0)')
tend_string = string(tend, format = '(i0)')

SXADDPAR, primary_header_a, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, primary_header_a, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, primary_header_a, "DATE-OBS", date_start, /SaveComment
SXADDPAR, primary_header_a, "DATE-END", date_end, /SaveComment
SXADDPAR, header_a, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, header_a, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, header_a, "DATE-OBS", date_start, /SaveComment
SXADDPAR, header_a, "DATE-END", date_end, /SaveComment


SXADDPAR, primary_header_b, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, primary_header_b, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, primary_header_b, "DATE-OBS", date_start, /SaveComment
SXADDPAR, primary_header_b, "DATE-END", date_end, /SaveComment
SXADDPAR, header_b, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, header_b, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, header_b, "DATE-OBS", date_start, /SaveComment
SXADDPAR, header_b, "DATE-END", date_end, /SaveComment

; Add the min/max values for the PI column:TLMIN29 and TLMAX29

SXADDPAR, header_a, "TLMIN29", 0, after ="TFORM29" , '0 keV'
SXADDPAR, header_a, "TLMAX29", 2e4,after ="TLMIN29" , '200 keV' ; 200 keV

SXADDPAR, header_b, "TLMIN29", 0, after ="TFORM29" , '0 keV'
SXADDPAR, header_b, "TLMAX29", 2e4,after ="TLMIN29" ,'200 keV' ; 200 keV

; Now do the GTI header changes. Set the default GTI to [TSTART, TEND]

telapse = tend - tstart
SXADDPAR, gti_header_a, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_a, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_a, "TELAPSE", telapse, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_a, "DATE-OBS", date_start, /SaveComment
SXADDPAR, gti_header_a, "DATE-END", date_end, /SaveComment
gti_a.start = double(tstart)
gti_a.stop = double(tend)

SXADDPAR, gti_header_b, "TSTART", tstart, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_b, "TSTOP", tend, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_b, "TELAPSE", telapse, /SaveComment, format = 'F12.0'
SXADDPAR, gti_header_b, "DATE-OBS", date_start, /SaveComment
SXADDPAR, gti_header_b, "DATE-END", date_end, /SaveComment
gti_b.start = double(tstart)
gti_b.stop = double(tend)



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
;; pospath = getenv('CALDB')+'/data/nustar/fpm/bcf/pixpos/'
;; pos_a_0 = mrdfits(pospath+'nuApixpos20100101v001.fits', 1, /silent)
;; pos_a_1 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 2, /silent)
;; pos_a_2 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 3, /silent)
;; pos_a_3 =  mrdfits(pospath+'nuApixpos20100101v001.fits', 4, /silent)
;; pos_b_0 = mrdfits(pospath+'nuBpixpos20100101v001.fits', 1, /silent)
;; pos_b_1 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 2, /silent)
;; pos_b_2 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 3, /silent)
;; pos_b_3 =  mrdfits(pospath+'nuBpixpos20100101v001.fits', 4, /silent)

;
; Define the transformation from the DET1 origin to the
; FPM origin. This is in the alignments FITS file, but
; we can just use the real number here. FPM is offset wrt
; DET1 by 180 subcells, or...
fpm_res = 604.8 / 5.
fpm_origin = 180 * fpm_res; microns


; Trim the event data to just a single example of each...
;data_a = data_a[0]
;data_b = data_b[0]

template_a = (temporary(data_a))[0]
template_b = (temporary(data_b))[0]


; Sort into FPMA and FPMB data from the NuSIM input...
fpma = where(data.fpm EQ 1, nfpma)
fpmb = where(data.fpm EQ 2, nfpmb)

print, 'Number of FPMA events: ', nfpma
print, 'Number of FPMB events: ', nfpmb
print, 'Total events: ', nfpma+nfpmb

; Replicate the L1A arrays into the right number of events...
;data_a = replicate(data_a, nfpma)
;data_b = replicate(data_b, nfpmb)


blocksize = 1e5
data_block = replicate(template_a, blocksize)



; Copy over all of the times, livetimes, and energy for each FPM:

; Set the energy resolution here:
eres = 0.1 ; keV / channel. for now 100 eV / PI channel
pioffset = 30 ; to shift the PI channels to the left..WHY?!?!?!?!
; ...because I said so?


;data_a.pi = round(data[fpma].energy / eres) - pioffset
;data_b.pi = round(data[fpmb].energy / eres) - pioffset

;data_a.time = data[fpma].time
;data_a.prior = data[fpma].livetime

;data_b.time = data[fpmb].time
;data_b.prior = data[fpmb].livetime

;data_a.shield = data[fpma].shield
;data_b.shield = data[fpmb].shield


; This is the main loop where you do all the heavy lifting...

; Just read in the inverted file. This contains the probability
; distribution for each software pixel and tells you where the count
; likely actually hit. For now, just have invert_fpmb.sav. Once you
; get the FPMA stuff, this call will be to load invert_fpma.sav
restore, 'invert_new_fpma.sav'
; loads invert_data


badones = [-1]
badimage_a = fltarr(360, 360)
n_a = 0.d
blocks = 0.
FOR i = 0l, nfpma - 1 DO BEGIN
   ; First step, figure out which pixel/grade you've hit...
   if n_elements(data_a)  gt limit then break

   ; Convert to DET1 coordinates...
   thisx = round( (data[fpma[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpma[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)


   thispdf = invert_data[thisx, thisy].pdf

   if max(thispdf) eq 0 then begin
      badones = [badones, fpma[i]]
      badimage_a[thisx, thisy]++
      continue
   endif


   ; If you made it here, good for you.
   thispdf = total(thispdf, /cumu)
   thispdf /= max(thispdf)
   foo = randomu(seed)
   goodones = where(thispdf gt foo, ngood)
   if ngood eq 0 then pick = n_elements(thispdf) -1 else $
      pick = min(goodones)



   ;; if n_a eq 0 then data_a = template_a else $
   ;;    data_a = [temporary(data_a), template_a]


   data_block[n_a].pi = round(data[fpma[i]].energy / eres) - pioffset
   data_block[n_a].time = data[fpma[i]].time
   data_block[n_a].prior = data[fpma[i]].livetime
   data_block[n_a].shield = data[fpma[i]].shield
   data_block[n_a].rawx = invert_data[thisx, thisy].rawx[pick]
   data_block[n_a].rawy = invert_data[thisx, thisy].rawy[pick]
   data_block[n_a].det_id = invert_data[thisx, thisy].det[pick]
   data_block[n_a].grade = invert_data[thisx, thisy].grade[pick]
   
   if n_a eq blocksize - 1 then begin
      if n_elements(data_a) eq 0 then data_a = data_block else $
         data_a = [temporary(data_a), data_block]
      print, blocks, n_elements(data_a), i 
      blocks++
;      stop
      n_a = -1.d
   endif
      
   n_a++


ENDFOR
;stop

if n_elements(data_a) eq 0 then data_a = data_block[0:n_a-1] else $
   if n_a gt 0 then data_a = [temporary(data_a), data_block[0:n_a-1]]

; Write the EVENT data and the EVENT header
mwrfits, data_a, out_a, $
         header_a, /no_types, /no_comment
mwrfits, gti_a, out_a, $
         gti_header_a, /no_types, /no_comment

print, 'Done with FPMA...'
print, 'Used: ', n_elements(data_a) 
print, 'Bad ones: ', total(badimage_a)
print, 'Sum: ',  n_elements(data_a) + total(badimage_a)
print, 'Input: ', nfpma

; Free the memory from the first run...
foo = n_elements(temporary(data_a))

restore, 'invert_new_fpmb.sav'
;use = fltarr(nfpmb)
badimage_b = fltarr(360, 360)
n_b = 0.d

blocks = 0
FOR i = 0l, nfpmb - 1 DO BEGIN


   if n_elements(data_b)  gt limit then break
   ; First step, figure out which pixel/grade you've hit...
;   IF i MOD 1e4 EQ 0 THEN print, i, " of ", nfpmb

   ; Convert to DET1 coordinates...
   thisx = round( (data[fpmb[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpmb[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)


   thispdf = invert_data[thisx, thisy].pdf
   if max(thispdf) eq 0 then begin
      badones = [badones, fpmb[i]]
      badimage_b[thisx, thisy]++

      continue
   endif

   ; If you made it here, good for you.
   thispdf = total(thispdf, /cumu)
   thispdf /= max(thispdf)
   foo = randomu(seed)
   goodones = where(thispdf gt foo, ngood)
   if ngood eq 0 then pick = n_elements(thispdf) -1 else $
      pick = min(goodones)

   ;; if n_b eq 0 then data_b = template_a else $
   ;;    data_b = [temporary(data_b), template_a]


   data_block[n_b].pi = round(data[fpmb[i]].energy / eres) - pioffset
   data_block[n_b].time = data[fpmb[i]].time
   data_block[n_b].prior = data[fpmb[i]].livetime
   data_block[n_b].shield = data[fpmb[i]].shield
   data_block[n_b].rawx = invert_data[thisx, thisy].rawx[pick]
   data_block[n_b].rawy = invert_data[thisx, thisy].rawy[pick]
   data_block[n_b].det_id = invert_data[thisx, thisy].det[pick]
   data_block[n_b].grade = invert_data[thisx, thisy].grade[pick]
   
   if n_b eq blocksize - 1 then begin
      if n_elements(data_b) eq 0 then data_b = data_block else $
         data_b = [temporary(data_b), data_block]
      print, 'Block: ', blocks, n_elements(data_b), i 
      blocks++
;      stop
      n_b = -1.d
   endif
      
   n_b++


ENDFOR

if n_elements(data_b) eq 0 then data_b = data_block[0:n_b-1] else  $
   if n_b gt 0 then data_b = [temporary(data_b), data_block[0:n_b-1]]

mwrfits, data_b, out_b, $
         header_b, /no_types, /no_comment
mwrfits, gti_b, out_b, $
         gti_header_b, /no_types, /no_comment

print, 'Done with FPMB'
print, 'Used: ', n_elements(data_b) 
print, 'Bad ones: ', total(badimage_b)
print, 'Sum: ',  n_elements(data_b) +n_elements(badimage_b)
print, 'Input: ', nfpmb



if n_elements(badones) gt 1 then begin
   badones = badones[1:*]
   badtimes = data[badones].time
   save, badones,badimage_a, badimage_b, badtimes, file = badfile
   print, 'Total bad ones: ', n_elements(badones)
endif else print, 'No bad ones!'

;print, 'Total used for FPMA: ', n_elements(data_a) , ' of ', nfpma
;print, 'Total used for FPMB: ', n_elements(data_b) , ' of' ,nfpmb 

END
