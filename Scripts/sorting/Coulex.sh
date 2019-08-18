# Save all command line arguments even if shift is used
argument_counter="$#";
arguments="$@";
the_date=$(date +%F)

function usage {
    echo "Usage of $0:"
    echo "Provide one command line argument; '-flag'"
    echo "e.g. '-d' to run Coulomb excitation analysis on data."
    echo "Argument flag choice: '-d' (default), '-s' (singles), '-a' (addback), '-r' (reject)."
    echo "Flags are based on which sorted input file you want to run Coulomb excitation analysis on."
    echo "Number of arguments: $argument_counter arguments."
    echo "Argument list: $arguments"
}

if [ $# != 1 ]; then
    echo "Error: number of command line arguments not supported."
    usage;
    exit;
else
    # Read first command line argument
    case "$1" in
        -d)
            input_file="../../Sorted_data/Sm_user-TreeBuilder-2019-06-20.root"
            config_file="../../Miniball-config/config-IS558.dat"
            cut_file="../../Sorted_data/outputfile.root"
            output_file_flag="default"
            ;;
        -s)
            input_file="../../Sorted_data/Sm_user-TreeBuilder-singles-2019-06-20.root"
            config_file="../../Miniball-config/config-IS558.dat"
            cut_file="../../Sorted_data/outputfile.root"
            output_file_flag="singles"
            ;;
        -a)
            input_file="../../Sorted_data/Sm_user-TreeBuilder-addback-2019-06-20.root"
            config_file="../../Miniball-config/config-IS558.dat"
            cut_file="../../Sorted_data/outputfile.root"
            output_file_flag="addback"
            ;;
        -r)
            input_file="../../Sorted_data/Sm_user-TreeBuilder-reject-2019-06-20.root"
            config_file="../../Miniball-config/config-IS558.dat"
            cut_file="../../Sorted_data/outputfile.root"
            output_file_flag="reject"
            ;;
        *)
            echo "$0: invalid option \"$2\""; 
            usage;
            exit ;;
    esac
fi


echo "--- Coulex: $output_file_flag ---"
command="CLXAna -i ${input_file} -o Coulex_${output_file_flag}-${the_date}.root -c ${config_file} -cut ${cut_file}:Bcut:Tcut"

# Call sorter to calibrate data
$command
