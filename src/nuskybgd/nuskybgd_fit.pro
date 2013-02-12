; NUSKYBGD_FIT
;
; Simultaneously fits an empirical model to multiple bgd spectra and writes
; an output parameter file that is used by nuskybgd_spec.pro and nuskybgd_image.pro.
;
; See nuskybgd.pro for typical inputs.
;
; OPTIONAL KEYWORDS
;
;   pa --> user set PA: Only used if reference background images not yet created,
;          or clobber keyword set.  If your PA follows the old convention, you
;          need to run projinitbgds.pro with /oldpa set instead.
;
;   paramfile --> name and full path for the output file:  If not set, the
;                 parameter file is written to 
;                     indir+'/'+obsid+'/event_cl/'+specdir+'/bgdfitparams[A/B].dat
;
;   nocheck --> Set this if you trust that the fit will be good.  Otherwise
;               the routine pauses inside XSPEC so you can evaluate and tweak
;               the fit.
;
;   fixfcxb --> Set to fix the focused CXB normalization to its nominal value.
;               WARNING: The CXB variance could be 100% depending on the size 
;                        of the regions, so using the nominal value is dangerous.
;               NOTE: Despite the warning above, the fCXB is linked between all
;                     the spectra, which is also likely to be incorrect.  When in 
;                     XSPEC, you can untie these if you like.



pro nuskybgd_fit,indir,obsid,bgdreg,specdir,specname,ab,header,bgddir,$
      clobber=clobber,pa=pa,paramfile=paramfile,nocheck=nocheck,fixfcxb=fixfcxb

auxildir=getenv('NUSKYBGD_AUXIL')+'/'
caldbdir=getenv('CALDB')+'/'
dir=indir
if strmid(dir,strlen(dir)-1) ne '/' then dir=dir+'/'
cldir=dir+obsid+'/event_cl/'
clspecdir=dir+obsid+'/event_cl/'+specdir+'/'
if size(bgddir,/type) eq 0 then dir=cldir else begin
    dir=cldir+bgddir+'/'
    if not file_test(dir,/directory) then spawn,'mkdir '+dir
endelse
if ab eq 'A' then iab=0 else if ab eq 'B' then iab=1 else stop,'ab defined wrong'

if not keyword_set(paramfile) then paramfile=clspecdir+'bgdfitparams'+ab+'.dat'

projinitbgds,indir,obsid,header,ab,bgddir,pa=pa,clobber=clobber

xcmfile=clspecdir+specdir+ab+'.xcm'
openw,lun,xcmfile,/get_lun
;readcol,auxildir+'ratios_lineE.dat',eline,width,/silent
;readcol,auxildir+'ratios_lineE.dat',blah,index1,ebreak,index2,$
;      format='(A,F,F,F)',/silent
;readcol,auxildir+'ratios'+ab+'.dat',f0,f1,f2,f3,/silent
readcol,auxildir+'ratios'+ab+'.dat',eline,width,f0,f1,f2,f3,/silent
readcol,auxildir+'ratios'+ab+'.dat',index1,index2,b0,b1,b2,b3,ebreak,/silent
eline=eline[0:n_elements(width)-2]
width=width[0:n_elements(width)-2]
;nparint=(4+3*n_elements(eline)+4)
;nparap=10
ifactors=[[f0],[f1],[f2],[f3]]
ifactors_orig=ifactors

pt=loadnuabs(0)
czt=loadnuabs(1)

bgdbackscl=fltarr(n_elements(bgdreg))
bgddetfrac=fltarr(n_elements(bgdreg),4)
bgdapfrac=fltarr(n_elements(bgdreg),4)
aptot=fltarr(n_elements(bgdreg),4)
dettot=fltarr(n_elements(bgdreg),4)
detfrac=fltarr(n_elements(bgdreg),4)
bgdpt=fltarr(n_elements(bgdreg))
bgdczt=fltarr(n_elements(bgdreg))
instrnorms=fltarr(n_elements(bgdreg),n_elements(eline)+1,4)
for ibgd=0,n_elements(bgdreg)-1 do begin

;print,'Pulling BACKSCAL (assumed to be region area as percentage of image) from:'
print,'Checking that bgd spectrum exists:'
print,'  '+clspecdir+specname[ibgd]
if not file_test(clspecdir+specname[ibgd]) then begin
    stop,'Background spectrum does not exist.  Kinda hard to fit nothing...'
;    print,'Background spectrum does not exist.  Creating:'
;    namesplit=strsplit(bgdreg,'.',/extract)
;    spawn,'./getspecnoarf.py '+indir+' '+obsid+' '+namesplit[0]+' '+specdir+' '+ab
endif

;bgdpha=mrdfits(clspecdir+specname[ibgd],1,hh,/silent)
;bgdbackscl[ibgd]=sxpar(hh,'BACKSCAL')

bgdmask=reg2mask(dir+'bgdap0'+ab+'.fits',cldir+bgdreg[ibgd])

for i=0,3 do begin
    fits_read,dir+'det'+str(i)+ab+'im.fits',detim
    fits_read,dir+'bgdap'+str(i)+ab+'.fits',apim
    bgddetfrac[ibgd,i]=total(detim*bgdmask)
    dettot[ibgd,i]=total(detim)
    bgdapfrac[ibgd,i]=total(apim*bgdmask)
    aptot[ibgd,i]=total(apim)
endfor
bgdbackscl[ibgd]=total(bgddetfrac[ibgd,*])/1000.^2
detfrac[ibgd,*]=bgddetfrac[ibgd,*]/total(bgddetfrac[ibgd,*])

;print,detfrac[ibgd,*]
bgdpt[ibgd,*]=total(pt[*,iab]*detfrac[ibgd,*])
bgdczt[ibgd,*]=total(czt[*,iab]*detfrac[ibgd,*])

endfor

printf,lun,'lmod nuabs'
for ibgd=0,n_elements(specname)-1 do begin
  namesplit=strsplit(specname[ibgd],'.',/extract)
  if strmid(namesplit[0],strlen(namesplit[0])-4) eq '_g30' then $
        rmfname=strmid(namesplit[0],0,strlen(namesplit[0])-4)+'.rmf' $
        else rmfname=namesplit[0]+'.rmf'
  printf,lun,'data '+str(ibgd+1)+':'+str(ibgd+1)+' '+cldir+specdir+'/'+specname[ibgd]
  printf,lun,'back '+str(ibgd+1)+' none'
  printf,lun,'resp 2:'+str(ibgd+1)+' '+cldir+specdir+'/'+rmfname
  printf,lun,'arf 2:'+str(ibgd+1)+' '+auxildir+'be.arf'
  printf,lun,'resp 3:'+str(ibgd+1)+' '+cldir+specdir+'/'+rmfname
  printf,lun,'resp 4:'+str(ibgd+1)+' '+cldir+specdir+'/'+rmfname
  printf,lun,'arf 4:'+str(ibgd+1)+' '+caldbdir+'data/nustar/fpm/bcf/arf/'+$
      'nu'+ab+'20100101v004.arf'
endfor

printf,lun,'model 2:apbgd nuabs*(po*highecut+po)'
printf,lun,str(bgdpt[0])+' -1'
printf,lun,str(bgdczt[0])+' -1'
printf,lun,'0. -1'
printf,lun,'0.9 -1'
printf,lun,'1.29 -1'
printf,lun,'0.01 0.001'
printf,lun,'1e-4 -1'
printf,lun,'41.13 -1'
printf,lun,'4.8 -1'
printf,lun,'2.0 0.01'
if n_elements(specname) gt 1 then for ibgd=1,n_elements(specname)-1 do begin
  printf,lun,str(bgdpt[ibgd])+' -1'
  printf,lun,str(bgdczt[ibgd])+' -1'
  printf,lun,'0. -1'
  printf,lun,'0.9 -1'
  printf,lun,'1.29 -1'
  printf,lun,'=apbgd:6*'+str(total(bgdapfrac[ibgd,*])/total(bgdapfrac[0,*]))
  printf,lun,'1e-4 -1'
  printf,lun,'41.13 -1'
  printf,lun,'4.8 -1'
  printf,lun,'=apbgd:10*'+str(total(dettot[0,*])/total(dettot[ibgd,*])/(total(bgddetfrac[0,*])/total(bgddetfrac[ibgd,*])))
endfor

printf,lun,'model 3:intbgd nuabs*(',format='($,A)'
for i=0,n_elements(eline)-1 do printf,lun,'gauss+',format='($,A)'
printf,lun,'bknpo)'
printf,lun,'=apbgd:1'
printf,lun,'=apbgd:2'
printf,lun,'=apbgd:3'
printf,lun,'=apbgd:4'
for i=0,n_elements(eline)-1 do begin
    printf,lun,str(eline[i])+' -1'
    printf,lun,str(width[i])+' -1'
    printf,lun,'1e-3 1e-4'
endfor
printf,lun,str(index1[0])+' -1'
printf,lun,str(ebreak[0])+' -1'
printf,lun,str(index2[0])+' -1'
printf,lun,'5e-6'
if n_elements(specname) gt 1 then for ibgd=1,n_elements(specname)-1 do begin
  printf,lun,'=apbgd:'+str(ibgd*10+1)
  printf,lun,'=apbgd:'+str(ibgd*10+2)
  printf,lun,'=apbgd:'+str(ibgd*10+3)
  printf,lun,'=apbgd:'+str(ibgd*10+4)
  for i=0,n_elements(eline)-1 do begin
      printf,lun,str(eline[i])+' -1'
      printf,lun,str(width[i])+' -1'
      printf,lun,'=intbgd:'+str(7+i*3)+'*'+str(total(ifactors_orig[i,*]*bgddetfrac[ibgd,*])/total(ifactors_orig[i,*]*bgddetfrac[0,*]))
  endfor
  printf,lun,str(index1[0])+' -1'
  printf,lun,str(ebreak[0])+' -1'
  printf,lun,str(index2[0])+' -1'
  printf,lun,'=intbgd:'+str(4+n_elements(eline)*3+4)+'*'+str(total(ifactors_orig[n_elements(eline),*]*bgddetfrac[ibgd,*])/total(ifactors_orig[n_elements(eline),*]*bgddetfrac[0,*]))
endfor

printf,lun,'model 4:fcxb nuabs*(po*highecut)'
printf,lun,'=apbgd:1'
printf,lun,'=apbgd:2'
printf,lun,'=apbgd:3'
printf,lun,'=apbgd:4'
printf,lun,'=apbgd:5'
; uses backscale to get area --> this is fraction of image pixels inside region,
; with pixel scale 2.45810736 arcsec/pixel (1000x1000 pixel image)
if keyword_set(fixfcxb) then fixfree=' -1e-6' else fixfree=' 1e-6'
printf,lun,str(0.002353*(2.45810736/3600.*1000.)^2*bgdbackscl[0])+fixfree
printf,lun,'=apbgd:7'
printf,lun,'=apbgd:8'
if n_elements(specname) gt 1 then for ibgd=1,n_elements(specname)-1 do begin
  printf,lun,'=apbgd:'+str(ibgd*10+1)
  printf,lun,'=apbgd:'+str(ibgd*10+2)
  printf,lun,'=apbgd:'+str(ibgd*10+3)
  printf,lun,'=apbgd:'+str(ibgd*10+4)
  printf,lun,'=apbgd:5'
;  printf,lun,str(0.002353*(2.45810736/3600.*1000.)^2*bgdbackscl[ibgd])+' 1e-6'
  printf,lun,'=fcxb:6*'+str(bgdbackscl[ibgd]/bgdbackscl[0])
  printf,lun,'=apbgd:7'
  printf,lun,'=apbgd:8'
endfor

printf,lun,'ignore **:**-3.,94.-**'
printf,lun,'fit 100'

openw,lun2,clspecdir+'bgdparams.xcm',/get_lun
printf,lun2,'set id [open '+cldir+'bgdparams.dat w]'
printf,lun2,'tclout param apbgd:6'
printf,lun2,'scan $xspec_tclout "%20f" p'
printf,lun2,'puts $id "$p"'
printf,lun2,'tclout param fcxb:6'
printf,lun2,'scan $xspec_tclout "%20f" p'
printf,lun2,'puts $id "$p"'
printf,lun2,'tclout param apbgd:10'
printf,lun2,'scan $xspec_tclout "%20f" p'
printf,lun2,'puts $id "$p"'
for i=0,n_elements(eline)-1 do begin
    printf,lun2,'tclout param intbgd:'+str(7+3*i)
    printf,lun2,'scan $xspec_tclout "%20f" p'
    printf,lun2,'puts $id "$p"'
endfor
printf,lun2,'tclout param intbgd:'+str(4+n_elements(eline)*3+4)
printf,lun2,'scan $xspec_tclout "%20f" p'
printf,lun2,'puts $id "$p"'
printf,lun2,'close $id'
printf,lun2,'exit'
free_lun,lun2

printf,lun,'cpd /xs'
printf,lun,'setplot energy'
printf,lun,'setplot command res y 1e-4 0.04'
printf,lun,'plot ldata delchi'
if not keyword_set(nocheck) then $
      printf,lun,'puts "----------\nCheck fit and adjust if necessary\n'+$
      '  then run @'+clspecdir+'bgdparams.xcm to complete.\n----------"' $
  else printf,lun,'@'+clspecdir+'bgdparams.xcm'
free_lun,lun

spawn,'xspec - '+xcmfile


readcol,cldir+'bgdparams.dat',params,/silent

; below is ratio of norm to CXB norm assumed in reference ap images times the
;  ratio of the total cts in 10^5s (used to create reference ap images) to
;  the cts of that image that fall in the extraction region
apnorm=(params[0]/0.002353)*32./total(bgdapfrac[0,*])
fcxbnorm=params[1]*total(dettot[0,*])/total(bgddetfrac[0,*])
neutnorm=params[2]*total(dettot[0,*])/total(bgddetfrac[0,*])

blah=max(reform(bgddetfrac[0,*],4),iref)
for i=0,3 do ifactors[*,i]/=ifactors_orig[*,iref]
instrnorms=fltarr(n_elements(eline)+1,4)
for i=0,n_elements(instrnorms[*,0])-1 do $
      instrnorms[i,*]=params[i+3]/total(reform(ifactors[i,*],4)*reform(bgddetfrac[0,*],4)/reform(dettot[0,*],4))*reform(ifactors[i,*],4)

; write to file to be read for other programs to create images and spectra
openw,lun,paramfile,/get_lun
printf,lun,apnorm,$
  '   # Factor relative to reference norm of PL for aperture component',$
  format='(E12.4,A)'
printf,lun,fcxbnorm,$
  '   # Norm of PL in CXB xspec model for entire FOV',format='(E12.4,A)'
printf,lun,neutnorm,$
  '   # Norm of PL in xspec model of soft componet for entire FOV',format='(E12.4,A)'
printf,lun,'# Norms of instrumental line components for 4 detectors: 0 1 2 3'
for i=0,n_elements(eline)-1 do $
      printf,lun,instrnorms[i,*],format='(4E12.4)'
printf,lun,'# Norms of instrumental continua for 4 detectors: 0 1 2 3'
printf,lun,instrnorms[n_elements(eline),*],format='(4E12.4)'
free_lun,lun

end
