#!/bin/bash

PCP_PATH="build/pcp/pcp"
VPCC_PATH="external/vpcc/bin/PccAppEncoder"
TMP_FOLDER="tmp"

VPCC_CFG_FOLDER="build/vpcc-prefix/src/vpcc/cfg/"

DEFAULT_VPCC_CTX="
--configurationFolder=${VPCC_CFG_FOLDER}
--config=${VPCC_CFG_FOLDER}/common/ctc-common.cfg
--config=${VPCC_CFG_FOLDER}/condition/ctc-all-intra.cfg"

_VPCC_START_FRAME_OPT_="startFrameNumber"

unset -v input
unset -v num_in
unset -v start_in
unset -v tiling
unset -v encode
unset -v context
unset -v output
unset -v rarray
unset -v duration
declare -a rarray  # Declare an array for -r arguments

show_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -i <input>        Input file pattern (required)"
    echo "  -n <num>          Number of input files (required)"
    echo "  -s <start>        Start frame number (required)"
    echo "  -t <tiling>       Tiling scheme (required)"
    echo "  -e <encoder>      Encoder to use (vpcc or gpcc, default: vpcc)"
    echo "  -c <context>      Encoding context file (optional)"
    echo "  -o <output>       Output directory (required)"
    echo "  -r <rate>         Compression rate (can be used multiple times)"
    echo "  -d <duration>     Frame count per encode (required)"
    echo "  -h                Show this help message"
    echo ""
    echo "Available Encoders:"
    echo "  - vpcc (default)"
    echo "  - gpcc (not yet supported, planned for future release)"
    echo ""
    echo "Example usage:"
    echo "  $0 -i longdress%04d.ply -n 300 -s 0 -t 2,2,2 -e vpcc -o output -r 1 -r 5 -d 30"
    exit 0
}

# Check if no arguments were provided
if [[ $# -eq 0 ]]; then
    show_help
fi

# Parse options
while getopts "i:n:s:t:e:c:o:r:d:h" opt; do
    case $opt in
        i) input=$OPTARG ;;
        n) num_in=$OPTARG ;;
        s) start_in=$OPTARG ;;
        t) tiling=$OPTARG ;;
        e) encode=$OPTARG ;;
        c) context=$OPTARG ;;
        o) output=$OPTARG ;;
        d) duration=$OPTARG ;;
        r) rarray+=($OPTARG) ;;
        h) show_help ;;
        *) echo "Invalid option: -$OPTARG" >&2; show_help ;;
    esac
done

# Validate required arguments
missing=0
for var in input num_in start_in tiling output duration; do
    if [[ -z "${!var}" ]]; then
        echo "Error: -${var:0:1} <${var}> is required" >&2
        missing=1
    fi
done

[[ -z "$rarray" ]] && echo "Error: -r <rate> is required" >&2 && missing=1
[[ "$missing" -eq 1 ]] && exit 1

# Check if encoder is valid
if [[ -n "$encode" && "$encode" != "vpcc" && "$encode" != "gpcc" ]]; then
    echo "Error: Invalid encoder '$encode'. Use 'vpcc' or 'gpcc'." >&2
    exit 1
fi

echo "All checks passed. Running the main script..."

TILE_COUNT=$(echo "$tiling" | awk -F',' '{print $1 * $2 * $3}')
SEGM_COUNT=$(( (num_in + duration - 1) / duration ))

PCP=$PCP_PATH

if [[ "$encode" == "gpcc" ]]; then
    ENCODER="$GPCC_PATH"
    CTX="${DEFAULT_GPCC_CTX}"
    START_FRAME_OPT="$_GPCC_START_FRAME_OPT_"
else
    ENCODER="$VPCC_PATH"
    CTX="${DEFAULT_VPCC_CTX}"
    START_FRAME_OPT="$_VPCC_START_FRAME_OPT_"
fi

if [[ -n "$context" ]]; then
    CTX=$(<"$context")
fi


mkdir $TMP_FOLDER
for i in $(seq $start_in $(($start_in + $num_in - 1))); do
    file=$(printf "$input" "$i")
    i_padded=$(printf "%04d" "$i")
    cmd="${PCP}
    -i ${file}
    -o ${TMP_FOLDER}/t%d.f${i_padded}.ply
    --pre-process=TILE
    -t ${tiling}"
    eval $cmd
done

mkdir "${output}"
for ((t=0; t<TILE_COUNT; t++)); do
    mkdir "${output}/tile${t}"

    for ((s=0; s<SEGM_COUNT; s++)); do
        mkdir "${output}/tile${t}/segment${s}"

        startframe=$((s * duration))

        for r in ${rarray[@]}; do
            mkdir "${output}/tile${t}/segment${s}/rate${r}"

            cmd="${ENCODER}
            ${CTX}
            --frameCount=${duration}
            --config=${VPCC_CFG_FOLDER}/rate/ctc-r${r}.cfg
            --${START_FRAME_OPT}=${startframe}
            --uncompressedDataPath=${TMP_FOLDER}/t${t}.f%04d.ply
            --compressedStreamPath=${output}/tile${t}/segment${s}/rate${r}/t${t}.s${s}.r${r}.bin"
            eval $cmd
        done
    done
done

rm -rf ${TMP_FOLDER}

echo "input: $input"
echo "input count: $num_in"
echo "input start frame num: $start_in"
echo "tiling scheme: $tiling"
echo "encoder: $ENCODER"
echo "encode context: $CTX"
echo "output folder: $output"
echo "rates: ${rarray[@]}"
echo "frame count per encode: $duration"

