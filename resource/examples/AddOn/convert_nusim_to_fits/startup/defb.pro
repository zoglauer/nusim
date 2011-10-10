base = getenv('CONV_DIR')
!path = expand_path('+'+base+'/startup/')+':'+$
        expand_path('+/usr/local/rsi/lib/')+':'+$
        !path
!quiet = 1
.run idl_routines
