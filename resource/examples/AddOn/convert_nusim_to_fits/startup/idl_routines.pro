pro upgrade
 ssw_upgrade,/hessi,/spex,/xray,/gen,/spawn,/loud
end

pro quit
exit
end


pro gr
set_plot,'x'
loadct,0
gamma_ct,.5
!x.style = 1
!y.style = 17
!p.color = 190
!p.charsize=1.3
!x.margin=[13.,3.]
end


pro fu_gauss, x,a,f,pder
ex = exp(-(x-a(2))^2./(2.*a(1)^2))
f = a(0)*ex
if n_params(0) LE 3 then return
pder = dblarr(n_elements(x),n_elements(a))
pder(*,0) = ex
pder(*,1) = a(0)*ex*(x-a(2))^2./a(1)^3.
pder(*,2) = a(0)*ex*(x-a(2))/a(1)^2.
end



function datin,fname,ncol,d
openr,1,fname
s=''
first=-1l
last=-1l
i=0l
while not eof(1) and last EQ -1l do begin
  readf,1,s
  ss = strmid(strcompress(s,/remove_all),0,1)
  linetype=strpos('0123456789.-',ss)
  if strlen(ss) EQ 0 then linetype=-1
  if first GE  0l and linetype EQ -1 then last=i-1l
  if first EQ -1l and linetype GE  0l then first=i
  i=i+1
end
if last EQ -1l then last=i-1l
close,1
openr,1,fname
d=dblarr(ncol,last-first+1)
for i=0,first-1 do readf,1,s 
readf,1,d
close,1
return,last-first+1l
end

function datin_str, file, d
; function reads in a file with an
; unspecified number of rows of strings
; returns the number of lines in the file.
openr, 1, file
nStrings = 0
while ~eof(1) do begin
	s = ''
	readf, 1, s
	if n_elements(d) eq 0 then d = s $
	else d = [d, s]
	nStrings++
endwhile
close, 1
return, nStrings
end



pro bin_plot, x, y, z, overplot = overplot, position = position
  ; X and Y are 1-D, with Z = [size(x), size(y)]
  ; This will make a color scale based on the current colortable.
  
  maxz = max(z)
  minz = min(z)
  ncolors = !d.table_size
  cscale = (ncolors - 20.) / (maxz - minz) ; not quite the full scale
  offset = (ncolors - 20.) - (maxz * cscale) ; make the top Z be the top color
  colors = z * cscale + offset + 4 ; adjust so that you stay clear of the bottom of the colortable

  nx = n_elements(x)
  ny = n_elements(y)
  nz = n_elements(z)
  if nx * ny ne nz then message, 'Wrong dimensions on X and Y.'

;  if ~keyword_set(overplot) then
;plot, x, y, /nodata, noerase = noerase, position = positon

  for i = 0, nx-2 do begin
     dx = x[i+1] - x[i]
     for j = 0, ny-2 do begin
        dy = y[j+1] - y[j]
        xb = [i*dx, i*dx, (i+1)*dx, (i+1)*dx]
        yb = [j*dy, (j+1)*dy, (j+1)*dy, j*dy]
        polyfill, xb, yb, color = colors[i, j]
     endfor
  endfor
end

; idl really needs this....
function strrepl, instring, replace_this, with_this

  x=str_sep(instring,replace_this)
  n=n_elements(x)
  y = x[0]
  for j=1,n-1 do begin
      y = y + with_this + x[j]
  endfor
  return,y
end
