# simple_test_harness

The idea behind the simple test harness is to provide a simple way to
write tests for P4 programs using the model.  The harness reads a simple
script of commands inserting entries into tables and injecting packets
and runs the model with those commands, recording the packets that
result.

The harness is run with the names of one or more script files on the
command line, and it reads each script in turn, executing the commands
in each.  It can also be run interactively, reading commands from stdin
by specifying "-" as the script.

Each command is a single line beginning with a command word, followed
by optional arguments.  A line beginning with "#" is a comment and is
ignored.  The "help" command lists all known commands.  A line ending with a ‘＼’
(backslash) is continued on the next line.

Commands may contain references to environment variables as `$NAME` or `${NAME}` --
the name will be looked up in the environment and the value substituted
prior to command processing.  The various `${NAME:...}` variations to specify default
or alternate values may be used as well.  Shell escapes (`$(shell command)`) may be used
to run external commands and substitute their output in commands.

### Commands

##### help

List the available commands with a one-line description of each

##### load <filename>

Loads a file, which can be a tofino binary image (loaded into the model),
a context json file (containing info about tables and names used in the
P4 program), or a shared library (containing table packing code used to
encode entries in tables).

##### add _table_ _address_ { _match_:_value_ } _action_ ( [ _address_ ] { _param_:_value_ } ) [ = _NAME_ ]

Adds an entry to a table.  For a ternary table, <address> is the priority, while
for an exact match table <address> is the way to use.  Punctuation (commas, parens)
is optional -- key names before the action are match keys, while those after the
action name are param names.  Match values may contain wildcards for ternary
tables.  Match keys and params that are not specified will be wildcard or 0.

For a ternary table, valid addresses are 0-<entries>-1 where <entries> is the total
number of entries in the table.  Higher numbered entries are higher priority.

For an exact match table, valid addresses are 0-<ways>-1 where <ways> is the number
of different entries a specific key could occupy accross all the ways and stages
in a table.  For example, if a table extends across two stages and each stage has
3 way stage tables, and each way stage table has two entries per table word, that
is a total of 12 "ways", which means the valid addresses to use for the table are
0-11.  0 and 1 will be the two entries in the first way table of the last stage;
6-11 will be the entries in the first stage.  The numbering here is supposed to
mirror the priority numbering of ternary tables (higher number is higher priority),
but it is not clear if that works within a stage.

The initial address argument for the action is only for tables with an indirect
action data table (an action profile) and specifies the address in the action data
table to use for the (non-immediate) action args.  It is required if there are
any such action args.

If the `= NAME` suffix appears, the match address used in the table is stored in the
specified environment variable, where it may be referenced subsequently as `$NAME`

##### check\_counter [ _pipe_: ] _table_ ( _index_ ) [ _field_ _rel_ _value_ ]

read a counter (statistics) table and print or check its value.  If <field> is present,
it must generally be `packets` or `bytes` (depending on what the counter is counting),
and <rel> may be `=`, `<`, `<=`, `>`, or `>=`.  If the pipe is not specified, will
check the counter in the first pipe where it is present

##### check\_register [ _pipe_: ] _table_ ( _index_ ) [ _field_ _rel_ _value_ ]

read a register (stateful) table and print or check its value.  <field> is `lo` or `hi`
for a register with unnamed fields (named fields with a layout struct use the names of
the fields in the struct), and <rel> may be `=`, `<`, `<=`, `>`, or `>=`.  If the pipe
is not specified, will check the counter in the first pipe where it is present

##### expect _port_ _hex pattern data_

Expects an output packet on a specific port.  When a packet its output on a port,
it is checked against the next packet expected on that port.  If it doesn't match,
an error is output.  If no packet is expected, an error occurs.  If expected packets
aren't output, an error occurs.

##### ghost run|queue [pipe _n_] _qid_:_qlength_...

Queue and optionally run one or more ghost thread queue updates.  If 'queue' is
specified, ghost threads will be run with packets as packets become available, but will
not run on their own (even if 'wait' is used).  If 'run' is specified, all queued
ghost packets will run by themselves.  Each <qid>:<qlength> pair will queue two
ghost threads (0 and 1 ping\_pong). Pipe 0 will be used if pipe is not specified.

##### packet _port_ _hex data_

Injects a raw packet.  Note that this will generally just queue the packet on
the port for input -- processing won't happen until a "wait" occurs.

##### selector [ _pipe_: ] _table_ ( _index_ ) add|remove _bit_ { , _bit_ }

Modify a selector entry.  The command can add or remove one or more bits from the
selector.  It can modify the selector in all pipes, or in one pipe.

##### setdefault _table_ _action_ ( { _param_:_value_ } )

Sets the default (miss) action to be executed for a table.  Currently does not work
if the action requires an action data table entry (all params must be immediate).

##### sleep _seconds_

Pause script processing for the specified time.  Packets will be processed by the
model threads.

##### tcam\_2bit\_mode true | false

Enable (or disable) tcam\_2bit\_mode.  When enabled, non-range tcams are assumed to be
programmed in 2bit range mode rather than "normal" mode.

##### wait

Wait for packet processing to complete.  Waits until all model internal queues are
empty before continuing.  If any expected packets are not output, issue errors about
them and clear all the expected output queues.

An implicit wait is always issued at the end of the input script.

##### wire _port_ -> _port_

All packets output on the first port will be sent back as input on the second port.
Packets output on the first port will not be flagged as errors if there are no expect
commands queued for the port.

##### write\_raw\_tcam _stage_:_unit_:_address_ _data_

Writes data into a specific tcam.

##### write\_raw\_sram _stage_:_unit_:_address_ _data_

Writes data into a specific sram.
