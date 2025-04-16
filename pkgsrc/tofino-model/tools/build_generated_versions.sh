#!/bin/bash

function log_and_exit() {
    echo "Error: $1"
    exit 1
}

function get_version_text() {
    local REPO_DIR=$1
    local REPO_NAME=$2
    # Note: MATCH is optional
    local MATCH=$3
    local TAG=$(cd ${REPO_DIR} && git describe --tags --always --dirty ${MATCH} 2>/dev/null)
    local BRANCH=$(cd ${REPO_DIR} && git rev-parse --abbrev-ref HEAD 2>/dev/null)
    local COMMIT=$(cd ${REPO_DIR} && git rev-parse HEAD 2>/dev/null)
    local UNK="unknown"

    echo "#define ${REPO_NAME}_TAG \"${TAG:-$UNK}\""
    echo "#ifndef ${REPO_NAME}_TAG"
    echo "#define ${REPO_NAME}_TAG \"000\""
    echo "#endif"
    echo "// If ${REPO_NAME}_BRANCH is HEAD, it indicates that git repo was in"
    echo "// detached head state."
    echo "#define ${REPO_NAME}_BRANCH ${BRANCH:-$UNK}"
    echo "#define ${REPO_NAME}_COMMIT_ID ${COMMIT:-$UNK}"
}

[ -z $1 ] && log_and_exit "Specify git repository directory as first argument"
[ -d $1 ] || log_and_exit "Invalid git repository directory $1"
REPO_DIR=$1
shift
[ -z ${1} ] && log_and_exit "Specify git repository name as second argument"
REPO_NAME=$1
shift

# --match option, if given, will be passed through to git describe
MATCH=
OUTPUT_FILE=
while [[ -n "$1" ]]; do
    if [ -n "${1}" ]; then
        if [ "${1}" = "--match" ]; then
            [ -z "${2}" ] && log_and_exit "match regex expected after --match option"
            MATCH="${1} ${2}"
            shift
            shift
        elif [ "${1}" = "--output" ]; then
            [ -z "${2}" ] && log_and_exit "path to file expected after --output option"
            OUTPUT_FILE="${2}"
            shift
            shift
        else
            log_and_exit "Unrecognised option ${1}"
        fi
    fi
done


if [ -z "${OUTPUT_FILE}" ]; then
    get_version_text ${REPO_DIR} ${REPO_NAME} "${MATCH}"
else
    # write version text to given file but only if the new text differs from
    # the existing content of the file
    VERSION_TEXT=$(get_version_text ${REPO_DIR} ${REPO_NAME} "${MATCH}")
    if [ ! -e ${OUTPUT_FILE} ]; then
        echo -e "${VERSION_TEXT}" > ${OUTPUT_FILE}
        [ $? != 0 ] && log_and_exit "problem writing ${OUTPUT_FILE}"
        echo "Created ${OUTPUT_FILE}"
    else
        CURRENT=$(< $OUTPUT_FILE)
        if [ "${VERSION_TEXT}" = "${CURRENT}" ]; then
            echo "No change required to ${OUTPUT_FILE}"
        else
            echo -e "${VERSION_TEXT}" > ${OUTPUT_FILE}
            [ $? != 0 ] && log_and_exit "problem writing ${OUTPUT_FILE}"
            echo "Modified ${OUTPUT_FILE}"
        fi
    fi
fi

