# Save all command line arguments even if shift is used
argument_counter="$#";
arguments="$@";
element="$1";
calibration="$2";
sorting="$3";
optional_flag="$4"
the_date=$(date +%F)

function usage {
    echo "Usage of $0:"
    echo "Provide three command line arguments; 'element calibration sorter'"
    echo "e.g. 'Sm online TB' to calibrate the respective data."
    # Sm is used for Samarium. If you have other elements, it can be extended quite easily.
    echo "Element choice: 'Sm'."
    echo "Calibration choice: 'online', 'zero' or 'user'."
    echo "Sorting choice: 'Q4' (AQ4Sort) or 'TB' (Treebuilder)."
    echo "Flag choice (optional, only for TreeBuilder): '-s' (singles), '-a' (addback), '-r' (reject)."
    echo "Number of arguments: $argument_counter arguments."
    echo "Argument list: $arguments"
}

if [ $# == 0 ] || [ $# -lt 3 ] || [ $# -gt 4 ]; then
    echo "Error: number of command line arguments not supported."
    usage;
    exit;
else
    # Read first command line argument
    case "$1" in
        Sm)
            run_number=(8 9 10 11 12 16 17 18 19 20 25 26 29 30 31 35 37 39 40 41 42 43 44 45 51 52 53 57 60);
            ;;  # ;; indicates end of case
        *)
            echo "$0: invalid option \"$1\""; 
            usage;
            exit ;;
    esac
    # Read second command line argument
    # By default, the config files lie in the folder "../../../Miniball/MiniballCoulexSort/config/"
    case "$2" in
        online)
            calibrationfile="../../Miniball-config/IS558-online.cal"
            ;;
        user)
            calibrationfile="../../Miniball-config/IS558-user.cal"
            ;;
        zero)
            calibrationfile="../../Miniball-config/IS558-zero.cal"
            ;;
        *)
            echo "$0: invalid option \"$2\""; 
            usage;
            exit ;;
    esac
    # Read third command line argument
    case "$3" in
        Q4)
            sort_choice="AQ4Sort"
            default_flag="";
            ;;
        TB)
            sort_choice="TreeBuilder";
            default_flag="-cdpad"
            ;;
        *)
            echo "$0: invalid option \"$3\""; 
            usage;
            exit ;;
    esac
    # Read fourth command line argument
    if [ "$3" == "TB" ]; then
        case "$4" in
            -s)
                flag="-s";  # singles
                flag_choice="-singles";
                ;;
            -a)
                flag="-addback";
                flag_choice="-addback";
                ;;
            -r)
                flag="-reject";
                flag_choice="-reject";
                ;;
            *)
                ;;  # no flag is also an option
        esac
    elif [ "$3" == "Q4" ]; then
        case "$4" in
            *)
                echo "Info: No flag option for 'AQ4Sort'. Ignoring optional flag."
                ;;
        esac
    fi
fi

filearray=()

for number in ${run_number[*]};
    do
        if [ $number -lt 10 ]; then
                zeros="00";
            else
                zeros="0";
        fi
        if [ $element == "Sm" ]; then
            directory="../../Raw_data/Sm"
            rootfile="$directory/140Sm_208Pb_pos6_${zeros}${number}_OnBeam.root"
        fi
        for entry in $directory/*;
            do
                if [ "$entry" == "$rootfile" ]; then
                    filearray+=("$entry")
                fi
            done
    done 

filearray=${filearray[@]}
echo "--- $sort_choice ---"
command="$sort_choice -i $filearray -o ${element}_${calibration}-${sort_choice}${flag_choice}-${the_date}.root -c $calibrationfile $default_flag $flag -vl"
# Call sorter to calibrate data
$command
