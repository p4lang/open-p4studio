#!/usr/bin/perl -w

use Getopt::Long;

print "#ifndef _REGISTER_MAP_\n";
print "#define _REGISTER_MAP_\n";
print "#include \"reg.h\"\n";

print "\nnamespace MODEL_CHIP_TEST_NAMESPACE {\n\n";

my $in_struct = 0;
my @lines =();
my $last_line="";
while (<>) {

    # sometimes semifore breaks lines that are too long,
    #   always after a comma I think
    $_ = $last_line . $_;
    $last_line="";
    if ( m/,\s*$/ ) {
        $last_line = $_;
        next;
    }

    if ( m/^typedef struct/ ) {
        $in_struct = 1;
    }
    if ( $in_struct ) {
        if ( m/^\s+(volatile)?\s+(\w+)\s+(\w+)\s*(\[[\[0-9a-fA-Fx\]]+)?/ ) {
            my $volatile = $1 ? 1: 0;
            my $type = $2;
            my $name = $3;
            my $indexes = $4 ? $4 : "";
            next if ($name =~ m/^_pad/);

            my $ptr = "0";
            if ( $type !~ m/^uint[0-9]+_t/ ) {
                $ptr = "new ${type}_map";
            }
            push @lines, { "substruct" => 0, "name" => $name, "type" => $type, "indexes" => $indexes, "ptr" => $ptr,
                           "volatile" => $volatile };

        }
        elsif ( m/^\}\s*(\w+)\s*,\s*\*(\w+)\s*/ ) {
            my $type = $1;
            my $ptr_type = $2;
            my @plines;
            foreach my $l (@lines) {
                if($l->{substruct} == 0) {
                    my $lname = $l->{name};
                    my $ltype = $l->{type};
                    my $indexes = $l->{indexes};
                    my $lptr  = $l->{ptr};

                    my $addr = "&${type}_base->$lname";


                    if ($l->{volatile}) {
                        # have to cast away the volatile!
                        $addr = "const_cast<$ltype(*)$indexes>($addr)";
                    }
                    $indexes =~ s/^\[(.*)\]$/$1/;
                    $ind = join(",",split(/\]\[/,$indexes));

                    push @plines, "    { \"$lname\", { $addr, $lptr, {$ind}, sizeof($ltype) } }";
                }
                elsif ($l->{substruct} == 1) {
                    my $vtype = $l->{vtype};
                    my $vname = $l->{vname};
                    my $sname = $l->{sname};

                    my $indexes = $l->{indexes};
                    $indexes =~ s/^\[(.*)\]$/$1/;
                    $ind = join(",",split(/\]\[/,$indexes));

                    my $addr = "${type}_base->${sname}";

                    push @plines, "    { \"$vname\", { &${addr}, new ${vtype}_map, {$ind}, sizeof(${addr}[0]) } }";
                }
            }


            print "struct ${type}_map: public RegisterMapper {\n";
            print "  static constexpr $ptr_type ${type}_base=0;\n";
            print "  ${type}_map() : RegisterMapper( {\n";
            print join(",\n",@plines)."\n";
            print "    } )\n";
            print "  {}\n";
            print "};\n\n";
            @lines=();
            $in_struct=0;
        }
        elsif ( m/^\s*struct \{/ ) {
            $_ = <ARGV>;
            my $vtype;
            my $vname;
            my $sname;
            my $indexes;
            #!only works if inner struct has one field then padding
            #first word is lptr
            #second word is lname
            @list = (m/[A-Za-z\_0-9]+/g);
            $vtype  = $list[0];
            $vname = $list[1];
            #skip 2 lines
            $_ = <ARGV>;
            $_ = <ARGV>;

            ($string) = $_ =~ m/[A-Za-z\_0-9\[\]]+/g;
            ($sname) = $string =~ m/[^\[]+/g;
            # works for N-ary
            ($indexes) = $string =~ m/\[.*/g;

            push @lines, { "substruct" => 1, "vtype" => $vtype, "vname" => $vname, "sname" => $sname, "indexes" => $indexes};
        }
    }
}

print "} // namespace MODEL_CHIP_TEST_NAMESPACE\n";
print "#endif\n";

