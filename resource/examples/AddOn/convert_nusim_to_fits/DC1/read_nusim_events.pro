PRO read_nusim_events


infile = getenv('FILENAME')
f = file_info(infile)
IF ~f.exists THEN message, 'File does not exist!'


COMMON nusim_data, data, err, header_data

; Reads in a NuSIM ASCII file and returns a structure with the
; relevant information.
err = 0

f = file_info(infile)
IF ~f.exists THEN BEGIN
   print, "File not found"
   err = 1
   return
ENDIF

proto = {fpm:0,pos:fltarr(3),energy:1.d,time:0.d,livetime:0.d}
header_data = {date_start:"string", $
               target_name:"string", obsid:"string", $
               tstart:-1.d}


first_time = 0

openr, lun, infile, /get_lun

n = -1.
WHILE(~EOF(lun)) DO begin
   dataline = 'string'
   readf, lun, dataline


   IF stregex(dataline, "TARGETNAME", /boolean) THEN BEGIN
      split = strsplit(dataline, /extract)
      header_data.target_name = split[1]
;      stop
   ENDIF

   IF stregex(dataline, "OBSID", /boolean) THEN BEGIN
      split = strsplit(dataline, /extract)
      header_data.obsid = split[1]
 ;     stop
   ENDIF
   
   IF stregex(dataline, "DATE-OBS", /boolean) THEN BEGIN
      split = strsplit(dataline, /extract)
      header_data.date_start = split[1]
  ;    stop
   ENDIF
   
   IF stregex(dataline, "TSTART", /boolean) THEN BEGIN
      split = strsplit(dataline, /extract)
      header_data.tstart = double(split[1])
   ;   stop
   ENDIF



   if stregex(dataline, 'SE', /boolean) then n++

   IF stregex(dataline, "HT", /boolean) THEN BEGIN
      IF n_elements(data) EQ 0 THEN data = proto ELSE data = [temporary(data), proto]

      thisline = strsplit(dataline, /extract)
      IF n_elements(thisline) LT 11 THEN stop 
      data[n].fpm = fix(thisline[1])
;      data[n].detid = fix(thisline[2])

      FOR i = 0, 2 DO $
         data[n].pos[i] = float(thisline[3+i])

      data[n].energy = float(thisline[9])
      data[n].time = float(thisline[11])
      
 
      IF n MOD 1e4 EQ 0 THEN print, n
   ENDIF

   if stregex(dataline, 'LT', /boolean) then begin
      thisline = strsplit(dataline, /extract)
      data[n].livetime = float(thisline[1])
   endif


endwhile

print, 'Done parsing ASCII file...'
print, 'Read in ', n, ' counts...'
close, lun
free_lun, lun

END

