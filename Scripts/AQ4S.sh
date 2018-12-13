# Save all command line arguments even if shift is used
argument_counter="$#";
arguments="$@";
element="$1";
calibration="$2";

function usage {
	echo "Usage of $0:"
	echo "Provide two command line arguments; 'element calibration'"
	echo "e.g. 'Sm online' or 'Ni online' to calibrate the respective data."
	echo "Element choice: 'Sm' or 'Ni'."
	echo "Calibration choice: 'online', 'zero' or 'user'."
	echo "Number of arguments: $argument_counter arguments."
	echo "Argument list: $arguments"
}

if [ $# == 0 ] || [ $# != 2 ]; then
	usage;
	exit;
else
	# Read first command line argument
    case "$1" in
        Sm)
            run_number=(25 26 31);
            ;;  # ;; indicates end of case
        Ni)
            run_number=(47 50 52 53 54 55 56 57 58 59); 
            ;;
        *)
            echo "$0: invalid option \"$1\""; 
            usage;
            exit ;;
    esac
	# Read second command line argument
    case "$2" in
        online)
			calibrationfile="../../Miniball/MiniballCoulexSort/config/calibration-online-is558-aug2017.cal"
			;;
		zero)
			calibrationfile="../../Miniball/MiniballCoulexSort/config/zero.cal"
			;;
		user)
			calibrationfile="../../Miniball/MiniballCoulexSort/config/calibration-des2018.cal"
			;;
        *)
            echo "$0: invalid option \"$2\""; 
            usage;
            exit ;;
    esac
fi

filearray=()

for number in ${run_number[*]};
    do
    	if [ $element == "Sm" ]; then
    		directory="../Raw_data/Sm"
        	rootfile="$directory/140Sm_208Pb_pos6_0${number}_OnBeam.root"
        elif [ $element == "Ni" ]; then
        	directory="../Raw_data/Ni"
        	rootfile="$directory/144Ba_58Ni_pos5_0${number}_OnBeam.root"
		fi
    	for entry in $directory/*;
			do
				if [ "$entry" == "$rootfile" ]; then
					filearray+=("$entry")
				fi
			done
    done 

filearray=${filearray[@]}
#command="AQ4Sort -i $filearray -o ${element}_run.root -c $calibrationfile"
echo "--- Treebuilder ---"
command="TreeBuilder -i $filearray -o ${element}_run_treebuilder_${calibration}.root -c $calibrationfile"
# Call AQ4Sort to calibrate data
$command
