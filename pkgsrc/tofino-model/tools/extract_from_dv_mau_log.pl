#!/usr/bin/perl -w
# this version also extracts egress eops and teops

use Getopt::Long;

my $max_phv = 10;

GetOptions ('phvs=i' => \$max_phv);

print "// ********** begin extracted from log\n";
my $phv_num = "";
my $out_phv_num = "";
my %eops;
my %teops;
my $outputted_pre_phv_code=0;
my $last_selector_time =-1;
my $post_processing_checks="";
my $stored_ops="";
my $pred_ing_st=0;
my $pred_egr_st=0;
my $pred_ght_st=0;
my $pred_global_exec=0;
my $pred_long_branch=0;
my $prev_pred_ing_st=0;
my $prev_pred_egr_st=0;
my $prev_pred_ght_st=0;
my $prev_pred_global_exec=0;
my $prev_pred_long_branch=0;
my $mpr_ing_st=0;
my $mpr_egr_st=0;
my $mpr_ght_st=0;
my $mpr_global_exec=0;
my $mpr_long_branch=0;
my $prev_mpr_ing_st=0;
my $prev_mpr_egr_st=0;
my $prev_mpr_ght_st=0;
my $prev_mpr_global_exec=0;
my $prev_mpr_long_branch=0;
while (<>) {
    # some lines have this junk at start
    s/In getInstance m_wrapper_created=1//;
    s/^[a-z_:]+:\s*//; # some have blah::blah: on front now
    # extract mau config
    if ( m/\stu\./ ) {
        die "Unrecognised:\n $_" unless m/OutWord/ or m/IndirectWrite/;
        s/^#//;
        print_or_store( $_ );    
    }
    elsif ( m/Creating REF MODEL WRAPPER/ ) {
	print " tu.Reset();\n";
    }
    elsif ( m/PRED Start Table RefModel\s+i=(0x[0-9A-Fa-f]+) e=(0x[0-9A-Fa-f]+) g=(0x[0-9A-Fa-f]+) glob=(0x[0-9A-Fa-f]+) lbr=(0x[0-9A-Fa-f]+)/ ) {
        print STDERR $_;
        $prev_pred_ing_st=$pred_ing_st;
        $prev_pred_egr_st=$pred_egr_st;
        $prev_pred_ght_st=$pred_ght_st;
	$prev_pred_global_exec=$pred_global_exec;
	$prev_pred_long_branch=$pred_long_branch;
        $pred_ing_st = $1;
        $pred_egr_st = $2;
        $pred_ght_st = $3;
	$pred_global_exec = $4;
	$pred_long_branch = $5;
    }
    elsif ( m/MPR Start Table RefModel\s+i=(0x[0-9A-Fa-f]+) e=(0x[0-9A-Fa-f]+) g=(0x[0-9A-Fa-f]+) glob=(0x[0-9A-Fa-f]+) lbr=(0x[0-9A-Fa-f]+)/ ) {
        print STDERR $_;
        $prev_mpr_ing_st=$mpr_ing_st;
        $prev_mpr_egr_st=$mpr_egr_st;
        $prev_mpr_ght_st=$mpr_ght_st;
	$prev_mpr_global_exec=$mpr_global_exec;
	$prev_mpr_long_branch=$mpr_long_branch;
        $mpr_ing_st = $1;
        $mpr_egr_st = $2;
        $mpr_ght_st = $3;
	$mpr_global_exec = $4;
	$mpr_long_branch = $5;
    }
    elsif ( m/Start Table RefModel\s+i=(0x[0-9A-Fa-f]+) e=(0x[0-9A-Fa-f]+)/ ) {
        print STDERR $_;
        $prev_pred_ing_st=$pred_ing_st;
        $prev_pred_egr_st=$pred_egr_st;
        $prev_pred_ght_st=$pred_ght_st;
	$prev_pred_global_exec=$pred_global_exec;
	$prev_pred_long_branch=$pred_long_branch;
        $prev_mpr_ing_st=$mpr_ing_st;
        $prev_mpr_egr_st=$mpr_egr_st;
        $prev_mpr_ght_st=$mpr_ght_st;
	$prev_mpr_global_exec=$mpr_global_exec;
	$prev_mpr_long_branch=$mpr_long_branch;	
        $pred_ing_st = $1;
        $pred_egr_st = $2;
        $pred_ght_st = 0;
	$pred_global_exec = 0;
	$pred_long_branch = 0;
        $mpr_ing_st = $1;
        $mpr_egr_st = $2;
        $mpr_ght_st = 0;
	$mpr_global_exec = 0;
	$mpr_long_branch = 0;
    }
    elsif ( m/\/\/\#(\d+)e([01])# RefModel ([io])Phv/ ) {
        maybe_output_pre_phv_code();
        my $ing_egr = $2;
        my $dir = $3;
        if ($phv_num && ($dir eq "i")) {
            #print STDERR "print_processing called from: $_\n";
            print_processing($phv_num,
			     $prev_pred_ing_st,$prev_pred_egr_st,$prev_pred_ght_st,
			     $prev_pred_global_exec,$prev_pred_long_branch,
			     $prev_mpr_ing_st,$prev_mpr_egr_st,$prev_mpr_ght_st,
			     $prev_mpr_global_exec,$prev_mpr_long_branch);
            $prev_pred_ing_st=0;
            $prev_pred_egr_st=0;
            $prev_pred_ght_st=0;
	    $prev_pred_global_exec=0;
	    $prev_pred_long_branch=0;
            $prev_mpr_ing_st=0;
            $prev_mpr_egr_st=0;
            $prev_mpr_ght_st=0;
	    $prev_mpr_global_exec=0;
	    $prev_mpr_long_branch=0;
        }
        $phv_num = $1;
        if ($phv_num == $max_phv) { 
            $phv_num="";
            print STDERR "Exeeded max_phv setting ($max_phv). Use -phvs=N to increase. Finishing.\n";
            last; 
        }
        $phv_num .= "e" . $ing_egr;
        # 
        print " Phv *${dir}phv_in_${phv_num} =  tu.phv_alloc();\n";
    }
    # WAS elsif ( $phv_num && m/phv_in->/ && ! m/\# Mon-/ ) {
    elsif ( $phv_num && m/phv_in->/ && m/\#\d+e[0-1]\#/ && ! m/\# Mon-/ ) {

        maybe_output_pre_phv_code();
        s/phv_in/phv_in_${phv_num}/g;
        s/^\#\s*//;
        s/\) \/\*/\); \/*/; # some don't have semicolons!
        s|\*/.*$|\*/|; # some have junk on the end of the line
        print;
    }
    # grab output phvs from rtl
    elsif ( m/\/\/\#(\d+e[0-1])# Rtl-Output oPhv  (MAU_O)?PHV/ ) {
        maybe_output_pre_phv_code();
        $out_phv_num = $1;
        print " Phv *ophv_out_${phv_num} =  tu.phv_alloc();\n";
    }
    elsif ( $phv_num && m/phv_out->/ ) {
        maybe_output_pre_phv_code();
        s/ophv_out/ophv_out_${phv_num}/g;
        s/^\# //;
        print;
    }
    elsif ( m/del Meter Eop#(\d+)e([01])#/ ) {
	print STDERR $_;	
        maybe_output_pre_phv_code();
        my $eop_num = "${1}_${2}";
        if (m/set_.*gress_eopinfo/) { #  first line mentioning this eop
            print " Eop eop_${eop_num}{};\n";
            $eops{ $eop_num } = 1;
        }
        if ( m/handle_eop/ ) {
            print_or_store( "mau->handle_eop(eop_${eop_num});\n" );
        }
        else {
            s/eop->/eop_${eop_num}./g;
            s/^\#/ /;
            print;
        }
    }
    elsif ( m/del Meter dTeop#(\d+)e([01])#/ ) {
        maybe_output_pre_phv_code();
        my $teop_num = "${1}_${2}";
        if (m/set_raw_addr/) { #  first line mentioning this teop
            print " Teop dteop_${teop_num}{};\n";
            $teops{ $teop_num } = 1;
        }
        if ( m/handle_dp_teop/ ) {
            print_or_store( "mau->handle_dp_teop(dteop_${teop_num});\n" );
        }
        else {
            s/.*dteop->/dteop_${teop_num}./g;
            s/^\#/ /;
            print;
        }
        
    }
    
   # other non-phv checks
    elsif ( m/\@ (\d+)ps .*\#(\d+)e0\# Sel alu (mismatch)?: word\[\s*(\d+)\] rtl = 0x([^,]+),/ ) {
        my $time = $1;
        my $selector_row= ($4*4)+3;
        my $expected=$5;
        my @words = unpack("(A8)*", $expected);
        if ( $time != $last_selector_time ) {  # protect against duplicates
            #print "Selector: $time $selector_row $expected\n";
            $post_processing_checks .= <<EOF;
            {
                // check the input to the selector alu
                MauLogicalRow *logrow = mau->logical_row_lookup($selector_row);

                BitVector<RmtDefs::kDataBusWidth> data_in(UINT64_C(0));
      
                //logrow->stats_alu_rd_data(&data_in); 
                logrow->get_selector_alu_input_data(&data_in); 
                EXPECT_EQ( 0x$words[3], data_in.get_word(0,  32));
                EXPECT_EQ( 0x$words[2], data_in.get_word(32, 32));
                EXPECT_EQ( 0x$words[1], data_in.get_word(64, 32));
                EXPECT_EQ( 0x$words[0], data_in.get_word(96, 32));

            }       
EOF

        }
        $last_selector_time = $time;
    }
    elsif ( /runSim/ ) {
        print "//$_";
    }
    
}

if ($phv_num) {
    print_processing($phv_num,
		     $pred_ing_st,$pred_egr_st,$pred_ght_st,$pred_global_exec,$pred_long_branch,
		     $mpr_ing_st,$mpr_egr_st,$mpr_ght_st,$mpr_global_exec,$mpr_long_branch);
}
print "// ********** end extracted from log\n";

sub print_processing {
    my ($i,
	$pred_ing_st,$pred_egr_st,$pred_ght_st,$pred_global_exec,$pred_long_branch,
	$mpr_ing_st,$mpr_egr_st,$mpr_ght_st,$mpr_global_exec,$mpr_long_branch) = @_;

    print STDERR "print_processing($i,$pred_ing_st,$pred_egr_st,$pred_ght_st,$pred_global_exec,$pred_long_branch)\n";
    print STDERR "print_processing($i,$mpr_ing_st,$mpr_egr_st,$mpr_ght_st,$mpr_global_exec,$mpr_long_branch)\n";

 print <<EOF;
    int pred_ingress_next_table_${i} = ${pred_ing_st};
    int pred_egress_next_table_${i} = ${pred_egr_st};
    int pred_ghost_next_table_${i} = ${pred_ght_st};
    // Next 2 calls setup PRED/MPR ingress_start_tab, egress_start_tab, ghost_start_tab, global_exec and long_branch.
    mau->set_pred(${pred_ing_st}, ${pred_egr_st}, ${pred_ght_st}, ${pred_global_exec}, ${pred_long_branch}); // i_nxt,e_nxt,g_nxt,gex,lbr
    mau->set_mpr(${mpr_ing_st}, ${mpr_egr_st}, ${mpr_ght_st}, ${mpr_global_exec}, ${mpr_long_branch}); // i_nxt,e_nxt,g_nxt,gex,lbr
    Phv *iphv_out_got_${i} = mau->process_match2(iphv_in_${i}, ophv_in_${i},
						 &pred_ingress_next_table_${i}, &pred_egress_next_table_${i}, &pred_ghost_next_table_${i});
    Phv *ophv_out_got_${i} = mau->process_action(iphv_in_${i}, ophv_in_${i});
    
    EXPECT_TRUE( TestUtil::compare_phvs(ophv_out_got_${i},ophv_out_${i},false) );
    // this works for some tests, but not for others:
    //EXPECT_TRUE( TestUtil::compare_phvs(iphv_out_got_${i},iphv_in_${i},false) );
    RMT_UT_LOG_INFO("iphv_out_got %s iphv_in\\n",
                    TestUtil::compare_phvs(iphv_out_got_${i},iphv_in_${i},false) ?
                    "==":"!=");
EOF

    print $post_processing_checks;
    $post_processing_checks="";

    print $stored_ops;
    $stored_ops="";
}


sub print_or_store {
    my ($t) = @_;
	# initially we can just print the writes / handle eops, but once we're into the phvs we have to 
    #  store them so that they don't get ahead of the phvs
	if ( $phv_num ) {
	    $stored_ops .= $t;
	}
	else {
	    print $t;
	}
}

sub maybe_output_pre_phv_code {
    return if $outputted_pre_phv_code;

    $outputted_pre_phv_code=1;
    print <<EOF;

#pragma GCC diagnostic ignored "-Wunused-variable"
    // Get handles for ObjectManager, MAU, port
    // Setup a port on pipe 0
    // This also sets up basic config ingress parser and deparser
    RmtObjectManager *om = tu.get_objmgr();
    Mau *mau = om->mau_lookup(pipe, stage);
    Port *port = tu.port_get(16);
    // Uncomment below to up the debug output
    flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

EOF
}
