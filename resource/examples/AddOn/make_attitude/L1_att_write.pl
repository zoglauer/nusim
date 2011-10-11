#!/usr/bin/perl -w
# $Rev: 3073 $ $Date: 2011-09-27 15:43:28 -0700 (Tue, 27 Sep 2011) $ $Author: marchant $
#
# Warning: the flags and bus Voltage are just kludged in for now.
#
# started with
# L1_chu4_read.pl Rev: 2962 Date: 2011-07-15 11:39:58 -0700 (Fri, 15 Jul 2011) Author: adavis
# and
# L1_chu4_write.pl Rev: 3030 Date: 2011-08-19 08:58:02 -0700 (Fri, 19 Aug 2011) Author: adavis 
# and
# tle2pos.pl Rev: 3036 Date: 2011-08-24 10:24:03 -0700 (Wed, 24 Aug 2011) Author: adavis

# usage: L1_chu4_read.pl infile.fits ./tle_nustar_2012_034.dat outfile.fits
#

use strict;

use Math::Trig;
use Astro::FITS::CFITSIO qw( :shortnames :constants );
use Carp;
use Data::Dumper;
use Math::Quaternion qw(slerp);

use Astro::Coord::ECI;
use Astro::Coord::ECI::TLE;
use Astro::Coord::ECI::TLE::Set;

# NuSTAR library routines and constants
use L1_subs;

my ($tstart,$tstop) = (0.0,0.0);
my $generator;	# Load with CREATOR keyword

my ( $chu4file, $TLEfile, $outfile ) = @ARGV;

# printf( "Will read from %s \n", $chu4file);
# printf( "Will use TLEs from %s \n", $TLEfile);
# printf( "Will write to %s\n", $outfile);

# read all of the input file
my $nevt = -1;

my $finptr;
my $status=0;



my $time;		# array reference
my $sourceflag;		# array reference
my $flags;		# array reference
my $solution;		# array reference

my @scbusq = ();	# array reference
my @chu4q = ();		# array reference
my $pointing;		# array reference
my $position;		# array reference
my $seq;		# array reference
my $corr;		# array reference
my $chuid;		# array reference
my $hirate;		# array reference
my $bbo;		# array reference
my $tref;		# array reference
my $valid;		# array reference
my $res;		# array reference
my $locks;		# array reference
my $objs;		# array reference
my $stars;		# array reference

my $ii;
my $jj;

ffopen($finptr,$chu4file,READONLY, $status);
check_status($status) or die;

ffgky($finptr,TSTRING,"CREATOR",$generator,undef,$status);
check_status($status) or die;
# printf "Generator was $generator \n";

if ( $generator eq "NuSIM" ) {
    $finptr->movnam_hdu(ANY_HDU,'ATTITUDE',0,$status);
} else {
    $finptr->movnam_hdu(ANY_HDU,'CHU4',0,$status);
}

ffgky($finptr,TDOUBLE,"TSTART",$tstart,undef,$status);
check_status($status) or die;

ffgky($finptr,TDOUBLE,"TSTOP",$tstop,undef,$status);
check_status($status) or die;

ffgky($finptr,TDOUBLE,"NAXIS2",$nevt,undef,$status);
check_status($status) or die;

# printf("tstart=%.1f   tstop=%.1f    nevt=%d\n",$tstart,$tstop,$nevt);

my $idx=0;
ffgcno($finptr,1,'TIME',$idx,$status);
ffgcv($finptr,TDOUBLE,$idx,1,1,$nevt,0,$time,undef,$status);
check_status($status) or die;

my $c_Deg= 180/pi;

for ($ii=0;$ii<$nevt;$ii++) {
   if ( $generator eq "NuSIM" ) {
	$chu4q[$ii] = 0;
	ffgcv($finptr,TDOUBLE,2,$ii+1,1,4,0,$chu4q[$ii],undef,$status);
   } else {
	$scbusq[$ii] = 0;
	ffgcv($finptr,TDOUBLE,2,$ii+1,1,4,0,$scbusq[$ii],undef,$status);
	$chu4q[$ii] = 0;
	ffgcv($finptr,TDOUBLE,3,$ii+1,1,4,0,$chu4q[$ii],undef,$status);
   }

    $sourceflag->[$ii]= 1;	# a kludge flag to say the data is from CHU4
    $flags->[$ii]= 1;	# a kludge
    $solution->[$ii]= 0;	# say this is the best solution we can get

    my @quat = ( $chu4q[$ii][0], $chu4q[$ii][1], $chu4q[$ii][2], $chu4q[$ii][3] );
#    my @quat = ( 0.7330089373 , 0.6675827795 , -0.0598872002 , 0.1159510781 ); # example from Bryce's memo
    # print "quat is ", join(" ", @quat),"\n";

    my @d = ( 0, 0, 1.0 );  # Vector pointing in the Z direction.
    # print "d is ", join(" ",@d),"\n";

    my  $q3=-1.0*$quat[0]*$d[0] - $quat[1]*$d[1] - $quat[2]*$d[2];
    my  $q0= 1.0*$quat[3]*$d[0] + $quat[1]*$d[2] - $quat[2]*$d[1];
    my  $q1= 1.0*$quat[3]*$d[1] - $quat[0]*$d[2] + $quat[2]*$d[0];
    my  $q2= 1.0*$quat[3]*$d[2] + $quat[0]*$d[1] - $quat[1]*$d[0];

    my @dThis= ( 0, 0, 0 );
      $dThis[0]=-1.0*$q3*$quat[0] + $q0*$quat[3] - $q1*$quat[2] + $q2*$quat[1];
      $dThis[1]=-1.0*$q3*$quat[1] + $q0*$quat[2] + $q1*$quat[3] - $q2*$quat[0];
      $dThis[2]=-1.0*$q3*$quat[2] - $q0*$quat[1] + $q1*$quat[0] + $q2*$quat[3];
    # print "dThis is ", join(" ",@dThis),"\n";

    my $m_Ra = $c_Deg*atan2( $dThis[1], $dThis[0]);
    if ( $m_Ra < 0) { $m_Ra+=360.0 };
    # print "m_Ra is $m_Ra \n";

    my $m_Dec = $c_Deg*asin( $dThis[2]); 
#    if ( $m_Dec < 0) { $m_Dec+=360.0 };
    # print "m_Dec is $m_Dec \n";

    my $m_Yaw = $c_Deg * atan2( $quat[2], $quat[3] );
    # print "m_Yaw is $m_Yaw \n";

    $pointing->[$ii][0]= $m_Ra;
    $pointing->[$ii][1]= $m_Dec;
    $pointing->[$ii][2]= $m_Yaw;
	# print "pointing is $pointing->[$ii][0] $pointing->[$ii][1] $pointing->[$ii][2] \n";
}

if ( $generator ne "NuSIM" ) {
ffgcv($finptr,TBYTE,4,1,1,$nevt,0,$seq,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,5,1,1,$nevt,0,$corr,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,6,1,1,$nevt,0,$chuid,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,7,1,1,$nevt,0,$hirate,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,8,1,1,$nevt,0,$bbo,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,9,1,1,$nevt,0,$tref,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,10,1,1,$nevt,0,$valid,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,11,1,1,$nevt,0,$res,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,12,1,1,$nevt,0,$locks,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,13,1,1,$nevt,0,$objs,undef,$status);
check_status($status) or die;

ffgcv($finptr,TBYTE,14,1,1,$nevt,0,$stars,undef,$status);
check_status($status) or die;
}


# for ($ii=0;$ii<$nevt;$ii++) {
#    my $dtime=NuSTARepoch2UTC($time->[$ii]);
#    printf( "%s\t%12.3f ",$dtime,$time->[$ii]);
#    printf( "% 5.3f % 5.3f % 5.3f % 5.3f ",$scbusq[$ii][0],$scbusq[$ii][1],$scbusq[$ii][2],$scbusq[$ii][3]);
#    printf( "% 5.3f % 5.3f % 5.3f % 5.3f ",$chu4q[$ii][0],$chu4q[$ii][1],$chu4q[$ii][2],$chu4q[$ii][3]);
#    printf("%1d %1d %1d %1d %1d %1d %1d %3d %3d %3d %3d\n",$seq->[$ii],$corr->[$ii],$chuid->[$ii],$hirate->[$ii],
#    $bbo->[$ii],$tref->[$ii],$valid->[$ii],$res->[$ii],$locks->[$ii],$objs->[$ii],$stars->[$ii]);
# }

# The End... of reading in the input file

# code to convert Quaternions into a new column of RA, Dec, and Yaw

my $debug = 0;
my $debug2 = 0;

my $pktlen = 60;		# Length of a CHU4 packet, in bytes
my $chu4apid = 0x12;
my $buffer;
my $nchu4pkt_read = 0;
$ii= 0;
$jj= 0;

our $chunksz = 40000;

my $foutptr;

# CHU4 structures
our %chu4 = ();

# CHU4 counters
our $nCHU4 = 0;
our $nCHU4_tot = 0;

# Setup global items declared in L1_subs.pm

$datetlm = `date -u +%Y-%m-%dT%H:%M:%S`;
chomp $datetlm;
$sepoch = $tstart;
$eepoch = $tstop;

$creator =  '$Id$';	# Use SVN Id as version number for this file
$tlm2fits = '$Id$'; # Use SVN Id as version number for this file

$status= 0;
ffinit( $foutptr, '!'.$outfile, $status);
check_status($status) or die;

get_observation();

prepare_firstHDU($foutptr,0);

# Read, unpack, and insert CHU4 data
ffmnhd($foutptr,BINARY_TBL,'ATTITUDE',0,$status);
check_status($status) or die;

insert_chu4_data($foutptr,$nevt,0);

# HDU checksums
ffpcks($foutptr,$status);
check_status($status) or die;

# Write HDU 3 with position, flags, nad bus Voltage
prepare_secondHDU($foutptr,0);
ffmnhd($foutptr,BINARY_TBL,'POSITION',0,$status);
insert_position_data( $foutptr);

# HDU checksums
ffpcks($foutptr,$status);
check_status($status) or die;

$foutptr->close_file($status);
check_status($status) or die;

# The End...



# Prepare output FITS file
sub prepare_firstHDU {
my ($foutptr, $fpm) = @_;

my $status=0;


my $bitpix=8;
my $naxis=0;
my $naxes=[0,0];

ffphps($foutptr,$bitpix,$naxis,$naxes,$status);
check_status($status) or die;

ffpky($foutptr,TSTRING,'TELESCOP','NuSTAR','Telescope (mission) name',$status);
check_status($status) or die;

insert_usual_keywords($foutptr);

# Primary HDU checksums
ffpcks($foutptr,$status);

# CHU4 HDU
my $binname = "ATTITUDE";
my ($ttype,$tunit,$tform,$tcomments,$tfields);

$tfields = 4;
$ttype = [ qw( TIME QPARAM POINTING SOURCE SOLUTION ) ];
$tform = [ qw( 1D 4D 3D 1B 1B ) ];
$tunit = [ ( 's', 'NA', 'Deg, Deg, Deg', 'NA', 'NA' ) ];
$tcomments = [ (
'Attitude timestamp: Secs since Jan 1 2010 00:00:00 UTC',
'CHU4 Attitude wrt. the J2000 heliocentric inertial ref. frame',
'RA, Dec, and Yaw calculated from QPARAM',
'1 = CHU4, 2 = other CHU',
'0 = best, nonzero = degraded'
) ];


ffcrtb($foutptr,BINARY_TBL,0,$tfields,$ttype,$tform,$tunit,$binname,$status);
check_status($status) or die;

# Add in custom comment fields to bintable columns
for ($ii=0;$ii<$tfields;$ii++) {
   my $ttype = sprintf("TTYPE%d",$ii+1);
   ffmcom($foutptr,$ttype,$tcomments->[$ii],$status);
   check_status($status) or die;
}

# Add in some extra comments
my $kindex;
ffgcno($foutptr,1,'TIME',$kindex,$status);
ffirec($foutptr,($kindex*3)+7,"COMMENT = 'TIME refers to the midpoint of the starlight integration-interval.'",$status);
check_status($status) or die;
ffirec($foutptr,($kindex*3)+8,"COMMENT = 'Refer to the MicroASC User Manual and TM/TC ICD'",$status);
check_status($status) or die;
ffirec($foutptr,($kindex*3)+9,"COMMENT = 'for detailed documentation: ASC-DTU-MA-3001 and ASC-DTU-ICD-3004.'",$status);
check_status($status) or die;

ffgcno($foutptr,1,'QPARAM',$kindex,$status);
ffirec($foutptr,($kindex*3) +11,"COMMENT = 'This is the raw CHU Quaternian'",$status);
check_status($status) or die;

# Append CHDU-specific keywords, and the usual keywords
ffpky($foutptr,TSTRING,'HDUCLASS','OGIP','Format conforms to OGIP standards',$status);
check_status($status) or die;
ffpky($foutptr,TSTRING,'HDUCLAS1','TEMPORALDATA','Time ordered data',$status);
check_status($status) or die;
ffpky($foutptr,TSTRING,'HDUCLAS2','HKP','Housekeeping data',$status);
check_status($status) or die;
ffpky($foutptr,TSTRING,'TELESCOP','NuSTAR','Telescope (mission) name',$status);
check_status($status) or die;

ffpky($foutptr,TFLOAT,'TIMEPIXR',0.5,'Bin time beginning=0 middle=0.5 end=1',$status);
check_status($status) or die;

insert_usual_keywords($foutptr);

}

sub insert_chu4_data {
my ($foutptr,$nchu4,$fpm) = @_;

our %chu4;
our $nCHU4_tot;

my $status=0;
my ($ii,$jj) = (0,0);

if ($debug) {
   print STDERR "pos = $nCHU4_tot   nchu4 = $nchu4\n";
}

ffpcl($foutptr,TDOUBLE,1,$nCHU4_tot+1,1,$nchu4, $time, $status);
check_status($status) or die;

ffpcl($foutptr,TDOUBLE, 4,$nCHU4_tot+1,1,$nchu4, $sourceflag, $status);
check_status($status) or die;

for ($ii=0;$ii<$nevt;$ii++) {
   ffpcl($foutptr,TDOUBLE,2,$ii+1,1,4, $chu4q[$ii], $status);
   check_status($status) or die;
   ffpcl($foutptr,TDOUBLE,3,$ii+1,1,3, $pointing->[$ii], $status);
   check_status($status) or die;
}

}

sub prepare_secondHDU {
my ($foutptr, $fpm) = @_;

my $status=0;


my $bitpix=8;
my $naxis=0;
my $naxes=[0,0];

#ffphps($foutptr,$bitpix,$naxis,$naxes,$status);
#check_status($status) or die;

my $binname = "POSITION";
my ($ttype,$tunit,$tform,$tcomments,$tfields);

$tfields = 4;
$ttype = [ qw( TIME POSITION FLAGS SOURCE ) ];
$tform = [ qw( 1D 3E 8X 1B ) ];
$tunit = [ ( 's', 'Deg, Deg, m', 'NA', 'NA' ) ];
$tcomments = [ (
'Attitude timestamp: Secs since Jan 1 2010 00:00:00 UTC',
'Lat, Lon, and Alt',
'Undefined flags',
'1 = position estimated from TLE'
) ];


ffcrtb($foutptr,BINARY_TBL,0,$tfields,$ttype,$tform,$tunit,$binname,$status);
check_status($status) or die;

# Add in custom comment fields to bintable columns
for ($ii=0;$ii<$tfields;$ii++) {
   my $ttype = sprintf("TTYPE%d",$ii+1);
   ffmcom($foutptr,$ttype,$tcomments->[$ii],$status);
   check_status($status) or die;
}

insert_usual_keywords($foutptr);

}

sub insert_position_data {
my ( $foutptr ) = @_;

my $status=0;
my ($ii,$jj) = (0,0);

ffpcl( $foutptr, TDOUBLE, 1, 1, 1, $nevt, $time, $status);
check_status($status) or die;

ffpcl( $foutptr, TDOUBLE, 3, 1, 1, $nevt, $flags, $status);
check_status($status) or die;

ffpcl( $foutptr, TBYTE, 4, 1, 1, $nevt, $sourceflag, $status);
check_status($status) or die;

#for ($ii=0;$ii<$nevt;$ii++) {
#   ffpcl($foutptr,TDOUBLE,2,$ii+1,1,4, $chu4q[$ii], $status);
#   check_status($status) or die;
#   ffpcl($foutptr,TDOUBLE,3,$ii+1,1,3, $pointing->[$ii], $status);
#   check_status($status) or die;
#}

# read in TLE data file
open TLEINF, $TLEfile or die "\nCan't open $TLEfile for reading: $!\n";
my @TLEdata = <TLEINF>;

# parse the input TLE data
my @elems = Astro::Coord::ECI::TLE->parse (@TLEdata);

# Find appropriate TLE to use, from TLE file
my $epoch = 0;
my $good_tle;
my $diff = 1.0E10;
foreach my $tle (@elems) {
     $epoch = $tle->get('epoch');
     my $cdiff = abs($epoch - ($tstart+$NuFSET));
     if ($cdiff < $diff) {
	$good_tle = $tle;
	$diff = $cdiff;
     }
}

 for ($ii=0;$ii<$nevt;$ii++) {
   $good_tle = $good_tle->sgp4r( $time->[$ii] + $NuFSET);
   my ($lat, $long, $alt) = $good_tle->geodetic ('WGS84');
   my $latdeg = $lat * 180 / pi;
   my $longdeg = $long * 180 / pi;
   if ($longdeg <0) {$longdeg = 360 + $longdeg;}
#   print "$time $longdeg $latdeg $alt\n";
   $position->[$ii][0]= $latdeg;
   $position->[$ii][1]= $longdeg;
   $position->[$ii][2]= $alt * 1000;
 }
 ffpcl( $foutptr, TDOUBLE, 2, 1, 1, $nevt, $position, $status);
 check_status($status) or die;

}

