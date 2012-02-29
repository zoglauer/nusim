PRO get_livetime


COMMON nusim_data, data, err, header_data, infile

comp_t1 = systime(1)
print, 'Computing livetime...'

conv_path = getenv('CONV_PATH')
;print, conv_path
outpath = getenv('OUTPATH')+'/'+file_basename(infile, '.dat')

outpath = outpath +'/'+header_data.obsid+'/event_uf'
;print, outpath

file_mkdir, outpath

; Setup the output file names
out_a = outpath+'/nu'+header_data.obsid+'A_livetime.txt'
out_b = outpath+'/nu'+header_data.obsid+'B_livetime.txt'

print, out_a
print, out_b

; Sort into FPMA and FPMB data from the NuSIM input...
fpma = where(data.fpm EQ 1, nfpma)
fpmb = where(data.fpm EQ 2, nfpmb)

; Replicate the L1A arrays into the right number of events...
;data_a = replicate(data_a, nfpma)
;data_b = replicate(data_b, nfpmb)

data_a_time = data[fpma].time
data_a_prior = data[fpma].livetime

livetime_a = calc_livetime(data_a_time, data_a_prior, tstart =tstart_a, $
                          live2 = livetime_2_a)
print, 'Average livetime for FPMA: ', mean(livetime_a), mean(livetime_2_a)

tindex = lindgen(n_elements(livetime_a)) + tstart_a

openw, lun, out_a, /get_lun

for t = 0l, n_elements(tindex)  - 1 do $
   printf, lun, tindex[t], livetime_a[t], livetime_2_a[t], format = '(i0, f16.5, f16.5)'

close, lun
free_lun, lun

data_b_time = data[fpmb].time
data_b_prior = data[fpmb].livetime

livetime_b = calc_livetime(data_b_time, data_b_prior, tstart = tstart_b, $
                          live2 = livetime_2_b)

tindex = lindgen(n_elements(livetime_b)) + tstart_b
print, 'Average livetime for FPMB: ', mean(livetime_b), mean(livetime_2_b)

openw, lun, out_b, /get_lun

for t = 0l, n_elements(tindex)  - 1 do $
   printf, lun, tindex[t], livetime_b[t],livetime_2_b[t], format = '(i0, f16.5, f16.5)'

close, lun
free_lun, lun

; Starting with the first integral second, compute the livetime per
; second:

print, 'Done computing livetime...'
comp_t2 = systime(1)
print, 'Livetime took ', (comp_t2 - comp_t1), '  seconds'




END

