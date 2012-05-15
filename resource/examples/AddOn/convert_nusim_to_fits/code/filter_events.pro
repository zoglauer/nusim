PRO filter_events


COMMON nusim_data, data, err, header_data, infile

; Read in the instrument map...
fpma_map = '/home/nustar1/SOC/CALDB/data/nustar/fpm/bcf/instrmap/nuAinstrmap20100101v001.fits'
fpmb_map = '/home/nustar1/SOC/CALDB/data/nustar/fpm/bcf/instrmap/nuBinstrmap20100101v001.fits'

gaps_a = mrdfits(fpma_map, 'INSTRMAP')
gaps_b = mrdfits(fpmb_map, 'INSTRMAP')

fpm_res = 604.8 / 5.
fpm_origin = 180 * fpm_res; microns

use = intarr(n_elements(data))

print, 'Applying deadtime...'


t0 = systime(1)
; Sort into FPMA and FPMB data from the NuSIM input...
fpma = where(data.fpm EQ 1, nfpma)
lasttime = min(data.time) - 1.0
for i = 0l, nfpma - 1 do begin
   thisx = round( (data[fpma[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpma[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)
   if gaps_a[thisx, thisy] eq 0 then continue
   dt = data[fpma[i]].time - lasttime
   if dt lt 2.5e-3  then begin
      use[fpma[i]] = -1
      continue
   endif

   ; If you made it here, then use this one.
   use[fpma[i]] = 1

   lasttime = data[fpma[i]].time        ; copy over this event into the "last" event time
   data[fpma[i]].livetime = dt - 2.5e-3 ; set the livetime to be the difference between
                                        ; less 2.5 ms
endfor

lasttime = min(data.time) - 1.0
fpmb = where(data.fpm EQ 2, nfpmb)
for i = 0l, nfpmb - 1 do begin
   thisx = round( (data[fpmb[i]].pos[0] * 1e3 + fpm_origin) / fpm_res)
   thisy = round( (data[fpmb[i]].pos[1] * 1e3 + fpm_origin) / fpm_res)
   if gaps_b[thisx, thisy] eq 0 then continue
   dt = data[fpmb[i]].time - lasttime
   if dt lt 2.5e-3  then begin
      use[fpmb[i]] = -1
      continue
   endif

   ; If you made it here, then use this one.
   use[fpmb[i]] = 1

   lasttime = data[fpmb[i]].time ; copy over this event into the "last" event time
   data[fpmb[i]].livetime = dt - 2.5e-3 ; set the livetime to be the difference between
                                ; less 2.5 ms
endfor

goodones = where(use eq 1, ngood)
gaps = where(use eq 0, nlost)
dead = where(use eq -1, ndead)

ntot = n_elements(data) 
print, 'Total events: ', ntot
print, 'Lost in gaps: ', nlost, 100. * nlost/ ntot
print, 'Lost in deadtime: ', ndead, 100. * ndead / ntot

data = (temporary(data))[goodones]

; Ensure that events are time-ordered:
sortInd = sort(data.time)
data = (temporary(data))[sortInd]


print, 'Done applying deadtime and sorting...'
print, 'Took: ', systime(1) - t0


END

