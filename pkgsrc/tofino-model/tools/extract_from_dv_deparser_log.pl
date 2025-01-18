#!/usr/bin/perl -w

# only works for JBay. Tofino wrapper would need updating to get this to work for Tofino.
#  ie deparser_model_process_packet()  would need similar code to deparser_model_process_packet_clot()

use Getopt::Long;

#my $max_phv = 10;
#GetOptions ('phvs=i' => \$max_phv);

print "// ********** begin extracted from log\n";


print <<EOF;
    Packet* output_pkt;
    PortConfig port_config;

    Packet* mirror_pkt = nullptr;
    Packet* resubmit_pkt = nullptr;
    LearnQuantumType lq;

    RmtObjectManager* m_rmt_obj_mgr_p = tu.get_objmgr();
    DeparserBlock* m_deparser_p = m_rmt_obj_mgr_p->deparser_get(0);
    auto& jbay_reg = RegisterUtils::ref_jbay();

EOF

my $last_was_phv = 0;

while (<>) {
    if ( /OutWord/ || /IndirectWrite/ ) {
        print "    ";
        print;
    }
    elsif ( /^M / ) {
        s/^M //;
        print "    ";
        print;
    }

}
print "// ********** end extracted from log\n";
