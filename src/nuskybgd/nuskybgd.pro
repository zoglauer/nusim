;;;;;;;;;;;;
; nuskybgd ;
;;;;;;;;;;;;
;
;   ABC-like example script for using these tasks
;
;   Questions, comments, or bugs should (please!) be reported to Dan at
;     daniel.r.wik@nasa.gov
;
; nuskybgd tasks are primarily meant to determine the background from blank sky
;  regions within an observation and use that to produce background spectra for
;  source regions and to produce background images.  It is also possible to 
;  create 'default' background spectra and images if there are no pure background
;  regions in an observation.  However, one must take care because the various
;  background components vary; for example, the unfocused, stray light 'aperture 
;  bgd' component that dominates below ~20 keV can vary +/-10% due to cosmic 
;  variance.  That said, for a quick background subtraction, especially of images
;  for presentation purposes, the default parameters do a good job.
;
;  The CXB components assume a fiducial normalization from HEAO-1 by default.
;  For the Boldt '87 CXB normalization from HEAO-1 in the 10-15 keV band,
;  the expected count rate is 3.2e-4 cts/s/cm^2/deg^2 (pulled from simulated
;  aperture spectra created below).  The image created by projinitbgds is in
;  the 10-15 keV band assuming a 100 ks exposure; it's just a counts image,
;  but the function that produces it has a normalization in units of
;  cts/deg^2/mm^2, so the normalization factor should be 0.32 cts/deg^2/mm^2.
;  This is the value given in the data file nomapbgdparams.dat.


; SETUP
;  Two environmental variables need to be correctly set:
;    CALDB --> points to your nustar caldb, which should already be set
;    NUSKYBGD_AUXIL --> points to the 'auxil' directory in this tarball
;      -this directory contains auxiliary data files and is also where
;       new files generated by tasks, but not specific to a particular
;       observation, are stored

;-----------------

; 1) Make reference spectra for the various background components
;    Assuming you will run nuskybgd_image, you need to make reference spectra
;    of the various background components.  Only needs to be run once, but
;    should be rerun (along with everything else) if the nuabs parameters change.

nuskybgd_imrefspec

;-----------------

; From here on this example observation is used:

dir='50002031_NGC253'
obsid='50002031004'

;-----------------

; 2) Make instrument map
;    This grabs the instrument map from the caldb and makes a 'pixel map' that
;    assigns each RAW pixel to a unique DET1 pixel.  This allows bad pixels to 
;    be excluded in the same way as is done in the pipeline.  It automatically 
;    excludes all identified bad pixels -- including any supplied by the
;    user -- during pipeline processing.  You can exclude additional pixels,
;    but in practice should not -- instead, rerun the pipeline with the
;    updated bad pixel list and run this again.

nuskybgd_instrmap,dir,obsid,'A','bgd'
nuskybgd_instrmap,dir,obsid,'B','bgd'

;    The last input, 'bgd', specifies the directory all the ancilliary 
;    observation-specific files are placed.  This directory is placed under
;    the observation's event_cl/ and will need to be fed to later routines
;    so it puts all the intermediate files in the same place and knows where
;    to grab them from.

;-----------------

; 3) Create an image, background regions with DS9, and extract bgd spectra
;    The image is necessary because its header is pasted on the reference
;    and background images so it has WCS coords.  No arfs are necessary to
;    go with the background spectra.  Examples of how to create these are in
;    python scripts (but these specifically do not need to be used):

; ./mkimgs.py 50002031_NGC253 50002031004 3 20
; ./getspecnoarf.py 50002031_NGC253 50002031004 bgd1A bgdspec A
; ./getspecnoarf.py 50002031_NGC253 50002031004 bgd1B bgdspec B
; ./getspecnoarf.py 50002031_NGC253 50002031004 bgd2A bgdspec A
; ./getspecnoarf.py 50002031_NGC253 50002031004 bgd2B bgdspec B

;    The images generated are im[A/B]3to20keV.fits and the spectra are extracted
;    from regions bgd1A.reg, etc.  The region files will be used later and they
;    are assumed to be in the event_cl directory, in this case:
;       50002031_NGC253/50002031004/event_cl/bgd1A.reg
;    All of the spectra and related products are placed in the directory:
;       50002031_NGC253/50002031004/event_cl/bgdspec/
;    Although not strictly necessary, it helps to have multiple bgd regions
;    that span the gradient of the aperture component, so it can be isolated
;    from the other two low energy components.  Annuli are good choices, with
;    perhaps two or three concentric annuli around the source.
;
;    NOTE: The chi^2 statistic is used, so be sure to group the bgd spectra.
;          In the above script, the output spectra are bgd1A_sr_g30.pha, etc.,
;          with bins grouped to at least 30 counts.  Also, background regions
;          do not need to avoid gaps, dead pixels, or areas where there is no
;          data -- these areas are weighted appropriately.  You are limited to
;          ds9 region types 'circle', 'ellipse', 'box', and 'polygon', and
;          multiple include and exclude regions can be used (they are 'ORed' 
;          together).

;-----------------

; 4) Fit the background spectra
;    In this step, all the background spectra are fit simultaneously to an
;    empirical model in xspec, assuming the spatial shape of the aperture bgd
;    and the relative instrumental line strengths between detectors.
;
;    NOTE: The current assumptions are based on ~0.5 Msec of data, some of which
;          are from early in the mission, and will likely be revised.

fits_read,dir+'/'+obsid+'/event_cl/imA3to20keV.fits',blah,header
bgdcore=['bgd1A','bgd2A']
bgdreg=bgdcore+'.reg'
bgdspec=bgdcore+'_sr_g30.pha'
nuskybgd_fit,dir,obsid,bgdreg,'bgdspec',bgdspec,'A',header,'bgd'

fits_read,dir+'/'+obsid+'/event_cl/imB3to20keV.fits',blah,header
bgdcore=['bgd1B','bgd2B']
bgdreg=bgdcore+'.reg'
bgdspec=bgdcore+'_sr_g30.pha'
nuskybgd_fit,dir,obsid,bgdreg,'bgdspec',bgdspec,'B',header,'bgd'

;    The last input, 'bgd', specifies the directory all the ancilliary 
;    observation-specific files are placed.  The files take some time to create,
;    but only need to be created once as long as the PA used doesn't change.
;    If you are unhappy with the PA given in the header because the background
;    images don't seem to be lined up quite perfectly, you can remake the
;    ancilliary files:
;
; projinitbgds,dir,obsid,header,'A','bgd',pa=42.,/clobber
; projinitbgds,dir,obsid,header,'B','bgd',pa=42.,/clobber
;
;    WARNING!  If the projected images in the 'bgd' directory are rotated
;    completely insensibly, then the PA in the header probably uses the old
;    PA definition.  To correct, simply run:
;
; projinitbgds,dir,obsid,header,'A','bgd',/oldpa,/clobber
; projinitbgds,dir,obsid,header,'B','bgd',/oldpa,/clobber
;
;    In the call to nuskybgd_fit, if you haven't set the 'nocheck' keyword,
;    the routine will stop in the middle inside xspec so you can evaluate the
;    fit to the background.  More detailed instructions about how to do that
;    will have to wait, but you are free to fiddle.  IF there is non-bgd emission
;    in your regions, such as from the wings of a bright source, you are free to
;    add a model that describes the spectrum of the additional component.
;    Appropriately, this part of the fit will not be used to generate the bgd
;    for a source.  When finished, run the script it tells you to (before you
;    started fiddling), which will save the parameters and exit:
;
; XSPEC12> @50002031_NGC253/50002031004/event_cl/bgdspec/bgdparams.xcm
;
;    The output parameter file is used to create spectra and images and by
;    default is called bgdfitparams[A/B].dat and is placed in the same
;    directory as your bgd spectra.  It is assumed to be there unless the
;    file is directly pointed to using the paramfile keyword.
    
;-----------------

; 5) Make spectra for your source region
;    In principle we now know what the background is doing everywhere, so we can
;    generate (using fakeit in xspec) a spectrum for any arbitrary region.
;    Assuming a source region called 'src.reg' in your event_cl directory and
;    the actual spectra in event_cl/src/:

nuskybgd_spec,dir,obsid,'srcA.reg','src','bgdspec','A','bgd'
nuskybgd_spec,dir,obsid,'srcA.reg','src','bgdspec','B','bgd'

;    The spectrum is assumed to follow its directory name, in this case it
;    is called 'src[A/B]_sr_g30.pha'.  If you don't follow my naming convention,
;    use the keyword 'specname'.
;
;    The background spectrum takes the same name as your source spectrum, with
;    'bgd' prefixed to it.  Header keywords are also updated, but likely not
;    very usefully for you, so beware.  What is produced is a simulated spectrum
;    based on the best-fit model created in 4), with parameters adjusted to be
;    appropriate for the source region.  By default, it uses counting statistics
;    and an exposure time 100x longer than the actual observation.  Both of
;    these can be adjusted with keywords.
;
;    If you want to produce a generic spectrum, simply point to the
;    bgdfitparams[A/B].dat files provided in the auxil directory:
;
; auxildir=getenv('NUSKYBGD_AUXIL')+'/'
; nuskybgd_spec,indir,obsid,'src.reg','src','','A','bgd',$
;      paramfile=auxildir+'bgdfitparamsA.dat'
;  (the bgd spectrum directory is only used to point to the param file, so in
;  this case nothing is specified)

;-----------------

; 6) Make background images
;    The same as 5), but to make a bgd image:

nuskybgd_image,dir,obsid,'imA3to20keV.fits',3,20,'A','bgd','bgdspec'
nuskybgd_image,dir,obsid,'imB3to20keV.fits',3,20,'B','bgd','bgdspec'

;    This produces both a bgd image (same name as the input image, but with
;    'bgd' prefixed to it) and a bgd-subtracted image (in this case,
;    'imA3to20keVbsub.fits').  The normalizations of the four bgd components
;    can be adjusted by hand with keywords, and a default background image
;    can be made just as done in 5) without doing 4) first.  The bgd image is
;    smooth (not simulated counts), so you must do appropriate smoothing to
;    check that you are getting a flat subtraction of all the bgd.

;-----------------

; *) Make default background images
;    For observations where there are no local background regions, it is
;    difficult (but potentially not impossible) to use nuskybgd_fit to
;    determine the background.  In this case, only steps 1), 2), and 6)
;    need be executed:

nuskybgd_instrmap,'A'
nuskybgd_instrmap,'B'
nuskybgd_imrefspec
nuskybgd_image,dir,obsid,'imA3to20keV.fits',3,20,'A','bgd','',$
      getenv('NUSKYBGD_AUXIL')+'/bgdfitparamsA.dat'
nuskybgd_image,dir,obsid,'imB3to20keV.fits',3,20,'B','bgd','',$
      getenv('NUSKYBGD_AUXIL')+'/bgdfitparamsB.dat'

;    You can also adjust the various bgd components by hand -- see
;    nuskybgd_image.pro for details.  If you see large negative holes in
;    a bgd-subtracted image, it probably means the bad pixel list is
;    incomplete.  I have some code that can add pixels to a bad pixel file 
;    in a user-friendly way -- if you need it, let me know.

;-----------------

; That's it!  See the individual routines for additional info, and send me
; an email if you get frustrated trying to track things down in the code.


end
