#!/bin/bash
# Start running PTF tests associated with a P4 program

function print_help() {
  echo "USAGE: $(basename ""$0"") -p <...> [OPTIONS -- PTF_OPTIONS]"
  echo "Options for running PTF tests:"
  echo "  -f PORTINFO_FILE"
  echo "    Read port to veth mapping information from PORTINFO_FILE"
  echo "  -h"
  echo "    Print this message"
  echo "  -k PKG_SIZE"
  echo "    Number of Subdevices per Device (1-2) package size"
  echo "  -m MAX_PORTS"
  echo "    Maximum ports passed to PTF"
  echo "  -p <p4_program_name>"
  echo "    Run PTF tests associated with P4 program"
  echo "  -s TEST_SUITE"
  echo "    Name of the test suite to execute passed to PTF"
  echo "  -t TEST_DIR"
  echo "    TEST_DIR contains test cases executed by PTF."
  echo "  --arch <tf1|tf2|tf2m|tf3>"
  echo "    Specifiy the chip architecture, defaults to Tofino (tf1)"
  echo "  --cleanup"
  echo "    Run test cleanup only"
  echo "  --drv-test-info <file>"
  echo "    Specify the driver combination test config file"
  echo "  --grpc-server GRPC_SERVER"
  echo "    Grpc server IP address, localhost by default"
  echo "  --ip <target switch IP address>"
  echo "    Target switch's IP address, localhost by default. Sets status, gRPC, and thrift server IP address"
  echo "  --no-status-srv"
  echo "    Do not query bf_switchd's status server"
  echo "  --no-veth"
  echo "    Skip veth setup and special CPU port setup"
  echo "  --num-pipes NUM_PIPES"
  echo "    Set num of pipes to use for test, the default is 4"
  echo "  --num-shards NUM_SHARDS"
  echo "    Number of shards that can be used to parallelize test execution"
  echo "  --port-mode <25G or 10G or 100G>"
  echo "    Specify the port mode for testing"
  echo "  --seed <number>"
  echo "    Specify the driver combination test random seed"
  echo "  --setup"
  echo "    Run test setup only"
  echo "  --socket-recv-size <socket bytes size>"
  echo "    socket buffer size for ptf scapy verification "
  echo "  --status-host <host name>"
  echo "    Specify bf_switchd's status server address; the default is localhost"
  echo "  --status-port <port number>"
  echo "    Specify bf_switchd's status server port number; the default is 7777"
  echo "  --shard-id <shard-id>"
  echo "    Index of shard (>= 0 and < number of shards)"
  echo "  --target <TARGET>"
  echo "    Target (asic-model, hw or bmv2), the default is asic-model"
  echo "  --test-params <ptf_test_params>"
  echo "    PTF test params as a string, e.g. arch='Tofino';target='hw';"
  echo "  --thrift-server THRIFT_SERVER"
  echo "    Thrift server IP address, localhost by default"
  echo "  --gen-xml-output <gen_xml_output>"
  echo "    Specify this flag to generate xml output for tests"
  echo "  --db-prefix"
  echo "    Database prefix to pass to PTF"
  echo "  --p4info"
  echo "    Path to P4Info Protobuf text file for P4Runtime tests"
  exit 0
}

trap 'exit' ERR

[ -z ${SDE} ] && echo "Environment variable SDE not set" && exit 1
[ -z ${SDE_INSTALL} ] && echo "Environment variable SDE_INSTALL not set" && exit 1

echo "Using SDE ${SDE}"
echo "Using SDE_INSTALL ${SDE_INSTALL}"

opts=`getopt -o f:hk:m:p:s:t: --long arch:,cleanup,drv-test-info:,grpc-server:,no-status-srv,no-veth,num-pipes:,num-shards:,port-mode:,seed:,setup,socket-recv-size:,status-host:,status-port:,shard-id:,target:,test-params:,thrift-server:,gen-xml-output,db-prefix:,p4info:,ip: -- "$@"`
if [ $? != 0 ]; then
  exit 1
fi
eval set -- "$opts"

# P4 program name
P4_NAME=""
# Number of subdevices per device (Package size)
NUM_SUBDEVICES=1
# json file specifying model port to veth mapping info
PORTINFO=None
MAX_PORTS=0
NUM_PIPES=4
CPUPORT=64
CPUVETH=251

NO_VETH=false
HELP=false
SETUP=""
CLEANUP=""
ARCH="Tofino"
TARGET="asic-model"

SOCKET_RECV_SIZE="10240"
SKIP_STATUS_SRV=false
TEST_PARAMS=""
DRV_TEST_INFO=""
DRV_TEST_SEED=""
PORTMODE=""
GEN_XML_OUTPUT=0
DB_PREFIX=""
P4INFO_PATH=""
P4_NAME_OPTION=""
PMM=""
NUM_SHARDS=""
SHARD_ID=""

THRIFT_SERVER='localhost'
GRPC_SERVER='localhost'
STATUS_SERVER='localhost'

while true; do
    case "$1" in
      -f) PORTINFO=$2; shift 2;;
      -h) HELP=true; shift 1;;
      -k) NUM_SUBDEVICES=$2; shift 2;;
      -m) MAX_PORTS=$2; shift 2;;
      -p) P4_NAME=$2; shift 2;;
      -s) TEST_SUITE="$2"; shift 2;;
      -t) TEST_DIR="$2"; shift 2;;
      --arch) ARCH=$2; shift 2;;
      --cleanup) CLEANUP="--cleanup"; shift 1;;
      --drv-test-info) DRV_TEST_INFO="--drivers-test-info $2"; shift 2;;
      --grpc-server) GRPC_SERVER=$2; shift 2;;
      --ip) GRPC_SERVER=$2; THRIFT_SERVER=$2; STATUS_SERVER=$2; shift 2;;
      --no-status-srv) SKIP_STATUS_SRV=true; shift 1;;
      --no-veth) NO_VETH=true; shift 1;;
      --num-pipes) NUM_PIPES=$2; shift 2;;
      --num-shards) NUM_SHARDS="--num-shards $2"; shift 2;;
      --port-mode) PORTMODE="--port-mode $2"; shift 2;;
      --seed) DRV_TEST_SEED="--seed $2"; shift 2;;
      --setup) SETUP="--setup"; shift 1;;
      --socket-recv-size) SOCKET_RECV_SIZE=$2; shift 2;;
      --status-host) STATUS_SERVER=$2; shift 2;;
      --status-port) STS_PORT=$2; shift 2;;
      --shard-id) SHARD_ID="--shard-id $2"; shift 2;;
      --target) TARGET=$2; shift 2;;
      --test-params) TEST_PARAMS=$2; shift 2;;
      --thrift-server) THRIFT_SERVER=$2; shift 2;;
      --gen-xml-output) GEN_XML_OUTPUT=1; shift 1;;
      --db-prefix) DB_PREFIX="--db-prefix $2"; shift 2;;
      --p4info) P4INFO_PATH=$2; shift 2;;
      --) shift; break;;
    esac
done

if [ $HELP = true ] || ( [ -z $P4_NAME ] && [ -z $TEST_DIR ] ); then
  print_help
fi

ARCH=`echo $ARCH | tr '[:upper:]' '[:lower:]'`
case "$ARCH" in
  "tofino3")  CPUPORT=2;  CHIP_FAMILY="tofino3";;
  "tf3")      CPUPORT=2;  CHIP_FAMILY="tofino3"; ARCH="tofino3";;
  "tofino2")  CPUPORT=2;  CHIP_FAMILY="tofino2";;
  "tofino2m") CPUPORT=2;  CHIP_FAMILY="tofino2";;
  "tf2")      CPUPORT=2;  CHIP_FAMILY="tofino2"; ARCH="tofino2";;
  "tf2m")     CPUPORT=2;  CHIP_FAMILY="tofino2"; ARCH="tofino2m";;
  "tofino")   CPUPORT=64; CHIP_FAMILY="tofino";;
  "tf1")      CPUPORT=64; CHIP_FAMILY="tofino"; ARCH="tofino";;
  *) echo "Invalid arch option specified ${ARCH}"; exit 1;;
esac

if [ $MAX_PORTS == 0 ]; then
    if [[ $CHIP_FAMILY == "tofino3" ]]; then
      # Max 16 ports per die on tofino3
      MAX_PORTS=32
    else
      MAX_PORTS=17
    fi
fi

if [ $NO_VETH = true ]; then
  CPUPORT=None
  CPUVETH=None
else
  # Setup veth interfaces
  echo "Setting up veth interfaces"
  sudo env "PATH=$PATH" $SDE_INSTALL/bin/veth_setup.sh
fi

if [ -z ${TEST_DIR} ]; then
  if [[ $P4_NAME == "switch" ]]; then
    TEST_DIR=`find $SDE/pkgsrc -type d -path "*switch*/ptf/api"`
  else
    TEST_DIR=`find $SDE/pkgsrc -type d -path "*p4-examples*/ptf-tests/$P4_NAME" | head -n1`
    if [[ ! -d $TEST_DIR ]]; then
      TEST_DIR=`find $SDE/pkgsrc -type d -path "*p4-examples*/p4_16_programs/$P4_NAME" | head -n1`
    fi
  fi
fi

P4_NAME_OPTION="--p4-name=$P4_NAME"

if [ ! -d "$TEST_DIR" ]; then
  echo "Test directory \"$TEST_DIR\" does not exist"
  exit 1
fi

if [[ ! -r $PORTINFO ]]; then
  if [[ $CHIP_FAMILY == "tofino3" ]]; then
    if [[ "$NUM_SUBDEVICES" == "2" ]]; then
      PORTINFO_FILE=$TEST_DIR/ports_tof3_2dev.json
      if [[ -r $PORTINFO_FILE ]]; then
        PORTINFO=$PORTINFO_FILE
      fi
    fi
    if [[ ! -r $PORTINFO ]]; then
      PORTINFO_FILE=$TEST_DIR/ports_tof3.json
      if [[ -r $PORTINFO_FILE ]]; then
        PORTINFO=$PORTINFO_FILE
      fi
    fi
  fi
  if [[ $CHIP_FAMILY == "tofino2" ]]; then
    PORTINFO_FILE=$TEST_DIR/ports_tof2.json
    if [[ -r $PORTINFO_FILE ]]; then
      PORTINFO=$PORTINFO_FILE
    fi
  fi
  if [[ ! -r $PORTINFO ]]; then
    PORTINFO_FILE=$TEST_DIR/ports.json
    if [[ -r $PORTINFO_FILE ]]; then
      PORTINFO=$PORTINFO_FILE
    fi
  fi
fi

if [[ $PORTINFO != *None ]]; then
  CPUPORT=None
  CPUVETH=None
fi

export PATH=$SDE_INSTALL/bin:$PATH
echo "Using TEST_DIR ${TEST_DIR}"
if [[ -r $PORTINFO ]]; then
  echo "Using Port Info File $PORTINFO"
fi
echo "Using PATH ${PATH}"
echo "Arch is $ARCH"
echo "Target is $TARGET"
PYTHON_LIB_DIR=$(python3 -c "from distutils import sysconfig; print(sysconfig.get_python_lib(prefix='', standard_lib=True, plat_specific=True))")

export PYTHONPATH=$SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/tofino/bfrt_grpc:$PYTHONPATH
export PYTHONPATH=$SDE_INSTALL/$PYTHON_LIB_DIR/site-packages:$PYTHONPATH
export PYTHONPATH=$SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/tofino:$PYTHONPATH
export PYTHONPATH=$SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/${ARCH}pd:$PYTHONPATH
export PYTHONPATH=$SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/p4testutils:$PYTHONPATH
PYTHONPATH=$($SDE_INSTALL/bin/sdepythonpath.py):$PYTHONPATH

if [ "${PKTPY,,}" = "false" ]; then
  echo "Using scapy."
else
  echo "Using bf_pktpy."
  PMM="-pmm bf_pktpy.ptf.packet_pktpy"
fi

# Check in with bf_switchd's status server to make sure it is ready
STS_PORT_STR="--port 7777"
if [ "$STS_PORT" != "" ]; then
  STS_PORT_STR="--port $STS_PORT"
fi
STS_HOST_STR="--host $STATUS_SERVER"
if [ $SKIP_STATUS_SRV = false ]; then
  python3 $SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/p4testutils/bf_switchd_dev_status.py \
    $STS_HOST_STR $STS_PORT_STR
fi

# P4Runtime PTF tests require the P4Info path to be provided as a PTF KV
# parameter. If the script was invoked with --p4info, we use the provided
# value. Otherwise we look for the P4Info file in the install directory.
if [ -z "$P4INFO_PATH" ]; then
    if [ -n "$P4_NAME" ]; then
        p4info=$SDE_INSTALL/share/${ARCH}pd/$P4_NAME/p4info.pb.txt
        if [ -f $p4info ]; then
            P4INFO_PATH=$p4info
        fi
    fi
fi
if [ "$P4INFO_PATH" != "" ]; then
    if [ "$TEST_PARAMS" != "" ]; then
        TEST_PARAMS="$TEST_PARAMS;p4info='$P4INFO_PATH'"
    else
        TEST_PARAMS="p4info='$P4INFO_PATH'"
    fi
fi

TEST_PARAMS_STR=""
if [ "$TEST_PARAMS" != "" ]; then
    TEST_PARAMS_STR="--test-params $TEST_PARAMS"
fi

#Run PTF tests
sudo env "PATH=$PATH" "PYTHONPATH=$PYTHONPATH" "GEN_XML_OUTPUT=$GEN_XML_OUTPUT" "PKTPY=$PKTPY" python3 \
    $SDE_INSTALL/$PYTHON_LIB_DIR/site-packages/p4testutils/run_ptf_tests.py \
    $PMM \
    $NUM_SHARDS $SHARD_ID \
    --arch $CHIP_FAMILY \
    --target $TARGET \
    --test-dir $TEST_DIR \
    --port-info $PORTINFO \
    $PORTMODE \
    $P4_NAME_OPTION \
    --thrift-server $THRIFT_SERVER \
    --grpc-server $GRPC_SERVER \
    --cpu-port $CPUPORT \
    --cpu-veth $CPUVETH \
    --max-ports $MAX_PORTS \
    --num-pipes $NUM_PIPES \
    --socket-recv-size $SOCKET_RECV_SIZE \
    $DRV_TEST_INFO $DRV_TEST_SEED \
    $TEST_SUITE $SETUP $CLEANUP $TEST_PARAMS_STR $DB_PREFIX "$@"
res=$?
if [[ $res == 0 ]]; then
    exit $res
else
    exit 1
fi
