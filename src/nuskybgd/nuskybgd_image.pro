; NUSKYBGD_IMAGE
;
; Takes the nominal aperture bgd shape and flat, detector-dependent shapes for
;   the other 3 bgd components and creates bgd and bgd-subtracted images in
;   sky coordinates.
;
; See nuskybgd.pro for typical inputs.
;
; OPTIONAL KEYWORDS
;
;   paramfile --> If paramdir is set and the parameter filename is the default
;                 (bgdfitparams[A/B].dat), this argument can be omitted.
;                 paramfile must include the full path, or at least the path
;                 relative to your IDL directory -- if set, paramdir is not used
;
;   pa --> user set PA: Only used if reference background images not yet created,
;          or clobber keyword set.  If your PA follows the old convention, you
;          need to run projinitbgds.pro with /oldpa set instead.
;
;   noremakeinstr --> If you run this multiple times without changing the model 
;                     parameters for the instrumental component, you can suppress
;                     the recreation of the instrumental reference spectrum by
;                     setting this keyword.
;
;   instrnorms --> A 4-element array allowing the normalization of the instrumental 
;                  background in each chip to be varied -- to set this component
;                  10% higher than assumed by the parameter file values, set
;                  instrnorms=[1.1,1.1,1.1,1.1]
;
;   normap/normfcxb/normneut --> Scalar adjustments (relative to 1) to the
;                                normalizations of the 3 other components.
;
;   outim --> Alternative name for the output bgd image.
;
;   nobsub --> Set to suppress the creation of a bgd-subtracted image


pro nuskybgd_image,indir,obsid,imname,elow,ehigh,ab,bgddir,paramdir,paramfile,$
                pa=pa,noremakeinstr=noremakeinstr,instrnorms=instrnorms,$
                normap=normap,normfcxb=normfcxb,normneut=normneut,$
                outim=outim,nobsub=nobsub,clobber=clobber
                
; USER SET DIRECTORY OF 'newinstrmap[A/B].fits' FILES
auxildir=getenv('NUSKYBGD_AUXIL')+'/'

dir=indir
if strmid(dir,strlen(dir)-1) ne '/' then dir=dir+'/' 
cldir=dir+obsid+'/event_cl/'
if not size(bgddir,/type) then bgddir=''
dir=cldir+bgddir+'/'

if not keyword_set(outim) then outim=cldir+'bgd'+imname $
      else outim=cldir+outim

if not size(paramfile,/type) then $
      paramfile=cldir+paramdir+'/bgdfitparams'+ab+'.dat'
instrspec=dir+'instrbgdfit'
if not keyword_set(noremakeinstr) or not file_test(instrspec+ab+'_det0.pha') then $
      fakinstr,indir,obsid,ab,paramfile,bgddir
if not keyword_set(instrnorms) then instrnorms=[1.0,1.0,1.0,1.0]

usernorm=[1.0,1.0,1.0]
if keyword_set(normap) then usernorm[0]=normap
if keyword_set(normfcxb) then usernorm[1]=normfxcb
if keyword_set(normneut) then usernorm[2]=normneut

fits_read,cldir+imname,data,header
livetime=sxpar(header,'LIVETIME')
projinitbgds,indir,obsid,header,ab,bgddir,pa=pa,clobber=clobber

bgdims=fltarr(1000,1000,4)
for j=0,3 do begin
    fits_read,dir+'det'+str(j)+ab+'im.fits',detim
    if j eq 0 then detall=detim else detall+=detim
    instrall=mrdfits(instrspec+ab+'_det'+str(j)+'.pha',$
          1,shead,/silent)
;    npix=sxpar(shead,'CORRSCAL')
    npix=total(detim)
    bgdims[*,*,3]+=total(instrall[where(instrall.channel ge $
          (elow-1.6)/0.04+1 and instrall.channel le $
          (ehigh-1.6)/0.04)].counts) * detim * $
          livetime/1e9*instrnorms[j]/npix
endfor

readcol,paramfile,norm,/silent
norm[1]=norm[1]/0.002353
prefixes=['aperbgd','fcxbbgd','neutbgd']
for k=0,2 do begin
    for j=0,3 do begin
        spec=mrdfits(auxildir+prefixes[k]+ab+'_det'+str(j)+'.pha',$
              1,shead,/silent)
        refcts=total(spec[where(spec.channel ge (10.0-1.6)/0.04+1 $
              and spec.channel le (15.0-1.6)/0.04)].counts)
        cts=total(spec[where(spec.channel ge (elow-1.6)/0.04+1 $
              and spec.channel le (ehigh-1.6)/0.04)].counts)
        if k eq 0 then begin
            fits_read,dir+'bgdap'+str(j)+ab+'.fits',img
            bgdims[*,*,k]+=cts/refcts*norm[k]*img*(livetime/1e5)*usernorm[k]
        endif else begin
            fits_read,dir+'det'+str(j)+ab+'im.fits',img
            bgdims[*,*,k]+=norm[k]*img*cts/1e9*livetime/total(detall)*usernorm[k]
        endelse
    endfor
endfor
bgdim=fltarr(1000,1000)
for i=0,3 do bgdim+=reform(bgdims[*,*,i],1000,1000)
;    for i=0,3 do print,total(bgdims[*,*,i])
fits_read,cldir+imname,data,header
fits_write,outim,bgdim,header
if not keyword_set(nobsub) then begin
    namesplit=strsplit(imname,'.',/extract)
    fits_write,cldir+namesplit[0]+'bsub.'+namesplit[1],data-bgdim,header
endif

end
