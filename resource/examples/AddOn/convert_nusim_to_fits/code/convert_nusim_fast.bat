!quiet=1

.run read_nusim_events_fast
.run filter_events
.run write_nusim_events_fast
.run calc_livetime
.run get_livetime

read_nusim_events_fast
filter_events
write_nusim_events_fast
get_livetime


exit
