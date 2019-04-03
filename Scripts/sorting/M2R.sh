# Save all command line arguments even if shift is used
argument_counter="$#";
arguments="$@";
element="$1";

function usage {
	echo "Usage of $0:"
	# Sm is used for Samarium. If you have other elements, it can be extended quite easily.
	echo "Provide one command line argument 'Sm' or 'Ni' to convert the respective data."
	echo "Number of arguments: $argument_counter arguments."
	echo "Argument list: $arguments"
}

if [ $# != 1 ]; then
	usage;
	exit;
else
	# Read variables from the command line, one by one
	while [ $# -gt 0 ]  # -gt: is greater than
	do
	    option=$1;  # load command line argument into option
	    shift;      # eat currently first command line argument
	    case "$option" in
	        Sm)
				# Choice of which run numbers to include
	            run_number=(8 9 10 11 12 16 17 18 19 20 25 26 29 30 31 35 37 39 40 41 42 43 44 45 51 52 53 57 60);
	            shift; ;;  # ;; indicates end of case
	        #Ni)
	        #    run_number=(47 50 52 53 54 55 56 57 58 59); 
	        #    shift; ;;
	        *)
	            echo "$0: invalid option \"$option\""; 
	            usage;
	            exit ;;
	    esac
	done
fi

settingfile="../../../Miniball/MiniballCoulexSort/config/MBSettings2017_CLX_IS558.dat"

for number in ${run_number[*]};
    do
		if [ $number -lt 10 ]; then
				zeros="00";
			else
				zeros="0";
		fi
        if [ $element == "Sm" ]; then
        	medfile="../../Raw_data/Sm/140Sm_208Pb_pos6_${zeros}${number}.med"
        #elif [ $element == "Ni" ]; then
        #	medfile="../../Raw_data/Ni/144Ba_58Ni_pos5_${zeros}${number}.med"
        fi
		command="MedToRoot -mf $medfile -sf $settingfile"
		# Call MedToRoot for converting
		$command
    done 
