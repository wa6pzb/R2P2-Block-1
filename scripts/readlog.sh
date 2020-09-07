#!/bin/sh

# Read FT8 log file (ALL.txt)
#
# Integrated with Node-Red via Files
#
# WA6PZB 4/20/2019 - R2P2 Block 1
# 09/07/2020 - Modify for WSJT-X v2.2 - frame, ftime columns 
# 09/07/2020 - Added parsing for altitude to b2 file and non-hex seq file check
#



# Function to clear files
clear_files () {
   echo 0 > /cygdrive/c/temp/logreader/a1
   echo 0 > /cygdrive/c/temp/logreader/b1
   echo 0 > /cygdrive/c/temp/logreader/b2
   echo 0 > /cygdrive/c/temp/logreader/c1
   echo 0 > /cygdrive/c/temp/logreader/seq
   
}

# Initialize file varibles so they have the CR LF
start=$(</cygdrive/c/temp/logreader/start)
stop=$(</cygdrive/c/temp/logreader/stop)


# Main Loop
while true; do 

	#Read sync file
	sync=$(</cygdrive/c/temp/logreader/sync)
	
	#Get UTC time and put in time file
	date -u +%T > /cygdrive/c/temp/logreader/time
	

	
	if [ "$sync" == "$start" ];then
		#frame=`tail -n1 /cygdrive/c/temp/logreader/log|cut -c25-37`
		#frame=`tail -n1 /cygdrive/n/Local/WSJT-X/ALL.txt|cut -c25-37`
                frame=`tail -n1 /cygdrive/n/Local/WSJT-X/ALL.txt|cut -c49-61`
		
		ftime=`tail -n1 /cygdrive/n/Local/WSJT-X/ALL.txt|cut -c8-13`
		echo $ftime>/cygdrive/c/temp/logreader/ftime
		
		type=`echo $frame|cut -c1`
		seq=`echo $frame|cut -c2-3`
		
                if ! [[ $seq =~ ^[0-9A-Fa-f]{1,}$ ]] ;then
                       echo -e " E \c"
                       seq=`echo 00`
                fi

		
		if [ "$type" == "A" ];then			
			grid=`echo $frame|cut -c4-13`
			echo -e " g \c"

			#Put grid location in a1 file
			echo $grid>/cygdrive/c/temp/logreader/a1
			
			#Put frame seq number (flight minutes) in seq file
			echo $((16#$seq))>/cygdrive/c/temp/logreader/seq
		fi
		
		if [ "$type" == "B" ];then
			b1=`echo $frame|cut -c4-6`
                        b2=`echo $frame|cut -c7-11`
			echo -e " b \c"
				
			echo $b1>/cygdrive/c/temp/logreader/b1
                        echo $b2>/cygdrive/c/temp/logreader/b2
			#echo -e " B frame end\c"
				
			echo $stop>/cygdrive/c/temp/logreader/sync
			
			#Put frame seq number (flight minutes) in seq file
			echo $((16#$seq))>/cygdrive/c/temp/logreader/seq
		fi
		
		if [ "$type" == "C" ];then
			c1=`echo $frame|cut -c4-6`
			echo -e " c \c"
			
			echo $c1>/cygdrive/c/temp/logreader/c1
			#echo -e " C frame end\c"
			
			echo $stop>/cygdrive/c/temp/logreader/sync
			
			#Put frame seq number (flight minutes) in seq file
			echo $((16#$seq))>/cygdrive/c/temp/logreader/seq
		fi
				
		
	else
		echo -e " $ftime Cycle complete"
		#sleep 2
		#clear_files
		echo $start > /cygdrive/c/temp/logreader/sync
		
	fi
	
	sleep 1

done


