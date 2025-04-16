#!/bin/bash

if [ -d include -a -d src ]; then
    P1=shared
    [[ "$1" = tofino* || "$1" = jbay ]] && P2="$1" || P2="*"
    STR="#include <register_includes"
    GREPSTR="${STR}/.*.h"
    SEDSTR="s|.*${STR}\/\(.*\)\.h>.*|\1|"
    
    grep -sE "$GREPSTR" {include,src}/$P1/*.{c,h,cpp,hpp} {include,src}/$P2/*.{c,h,cpp,hpp} |\
	grep -v notused | sed -e "$SEDSTR" | sed -e 's|_mutable||' | sed -e 's|_array[0-9]*||' |\
	sort | uniq 
else
    echo "$0: Run in top-level RefModel directory"
    echo "$0: Usage $0 <chip> (where chip=tofino,tofinoB0,jbay)"
fi

