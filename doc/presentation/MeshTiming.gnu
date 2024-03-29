#!/usr/local/bin/gnuplot -persist
#
#    
#    	G N U P L O T
#    	Version 4.2 patchlevel 2 
#    	last modified 31 Aug 2007
#    	System: Darwin 9.5.0
#    
#    	Copyright (C) 1986 - 1993, 1998, 2004, 2007
#    	Thomas Williams, Colin Kelley and many others
#    
#    	Type `help` to access the on-line reference manual.
#    	The gnuplot FAQ is available from http://www.gnuplot.info/faq/
#    
#    	Send bug reports and suggestions to <http://sourceforge.net/projects/gnuplot>
#    
set terminal aqua 0 title "Figure 0" size 846 594 font "Helvetica,18" enhanced dashed
# set output
unset clip points
set clip one
unset clip two
set bar 1.000000
set border 31 front linetype -1 linewidth 1.000
set xdata
set ydata
set zdata
set x2data
set y2data
set timefmt x "%d/%m/%y,%H:%M"
set timefmt y "%d/%m/%y,%H:%M"
set timefmt z "%d/%m/%y,%H:%M"
set timefmt x2 "%d/%m/%y,%H:%M"
set timefmt y2 "%d/%m/%y,%H:%M"
set timefmt cb "%d/%m/%y,%H:%M"
set boxwidth
set style fill  empty border
set style rectangle back fc lt -3 fillstyle  solid 1.00 border -1
set dummy x,y
set format x "% g"
set format y "% g"
set format x2 "% g"
set format y2 "% g"
set format z "% g"
set format cb "% g"
set angles radians
unset grid
set key title ""
set key inside right bottom vertical Right noreverse enhanced autotitles box linetype -1 linewidth 1.000
set key noinvert samplen 4 spacing 1 width 0 height 0 
unset label
unset arrow
set style increment default
unset style line
unset style arrow
set style histogram clustered gap 2 title  offset character 0, 0, 0
unset logscale
set logscale x 10
set logscale y 10
set offsets 0, 0, 0, 0
set pointsize 1
set encoding default
unset polar
unset parametric
unset decimalsign
set view 60, 30, 1, 1
set samples 100, 100
set isosamples 10, 10
set surface
unset contour
set clabel '%8.3g'
set mapping cartesian
set datafile separator whitespace
unset hidden3d
set cntrparam order 4
set cntrparam linear
set cntrparam levels auto 5
set cntrparam points 5
set size ratio 0 1,1
set origin 0,0
set style data points
set style function lines
set xzeroaxis linetype -2 linewidth 1.000
set yzeroaxis linetype -2 linewidth 1.000
set zzeroaxis linetype -2 linewidth 1.000
set x2zeroaxis linetype -2 linewidth 1.000
set y2zeroaxis linetype -2 linewidth 1.000
set ticslevel 0.5
set mxtics default
set mytics default
set mztics default
set mx2tics default
set my2tics default
set mcbtics default
set xtics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0
set xtics autofreq 
set ytics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0
set ytics autofreq 
set ztics border in scale 1,0.5 nomirror norotate  offset character 0, 0, 0
set ztics autofreq 
set nox2tics
set noy2tics
set cbtics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0
set cbtics autofreq 
set title "" 
set title  offset character 0, 0, 0 font "" norotate
set timestamp bottom 
set timestamp "" 
set timestamp  offset character 0, 0, 0 font "" norotate
set rrange [ * : * ] noreverse nowriteback  # (currently [0.00000:10.0000] )
set trange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set urange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set vrange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )
set xlabel "Number of Cells" 
set xlabel  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set x2label "" 
set x2label  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set xrange [ * : 125000. ] noreverse nowriteback  # (currently [-10.0000:] )
set x2range [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set ylabel "Computation Time (ms)" 
set ylabel  offset character 0, 0, 0 font "" textcolor lt -1 rotate by 90
set y2label "" 
set y2label  offset character 0, 0, 0 font "" textcolor lt -1 rotate by 90
set yrange [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set y2range [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set zlabel "" 
set zlabel  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set zrange [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set cblabel "" 
set cblabel  offset character 0, 0, 0 font "" textcolor lt -1 norotate
set cbrange [ * : * ] noreverse nowriteback  # (currently [-10.0000:10.0000] )
set zero 1e-08
set lmargin -1
set bmargin -1
set rmargin -1
set tmargin -1
set locale "C"
set pm3d explicit at s
set pm3d scansautomatic
set pm3d interpolate 1,1 flush begin noftriangles nohidden3d corners2color mean
set palette positive nops_allcF maxcolors 0 gamma 1.5 color model RGB 
set palette rgbformulae 7, 5, 15
set colorbox default
set colorbox vertical origin screen 0.9, 0.2, 0 size screen 0.05, 0.6, 0 bdefault
set loadpath 
set fontpath 
set fit noerrorvariables
GNUTERM = "aqua"
plot "MeshTiming.dat" using 2:3 with lines title "Geometry Creation" lw 1.5 lt 1, "MeshTiming.dat" using 2:4 with lines title "Forward Sweep" lw 1.5 lt 1 lc 3, "MeshTiming.dat" using 2:5 with lines title "Backward Sweep" lw 1.5 lt 1 lc 2

F(x) = x**mF*10**bF   # fit line to data for log-log plot
fit F(x) "MeshTiming.dat" using 2:4 via mF, bF
replot F(x) lt 6 lc -1 title "ForwardFit"
FFit = gprintf("slope = %3.1f", mF)
set label FFit at 15000,100000

B(x) = x**mB*10**bB   # fit line to data for log-log plot
fit B(x) "MeshTiming.dat" using 2:5 via mB, bB
replot B(x) lt 5 lc -1 title "BackwardFit"
BFit = gprintf("slope = %3.1f", mB)
set label BFit at 15000, 2500
replot
#    EOF
