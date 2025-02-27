#include "tofino/intrinsic_metadata.p4"

/* Sample P4 program */
header_type pkt_t {
    fields {
        field_a_32 : 32;
        field_b_32 : 32;
        field_c_32 : 32;
        field_d_32 : 32;

        field_e_16 : 16;
        field_f_16 : 16;
        field_g_16 : 16;
        field_h_16 : 16;

        field_i_8 : 8;
        field_j_8 : 8;
        field_k_8 : 8;
        field_l_8 : 8;
    }
}

parser start {
    return parse_ethernet;
}

header pkt_t pkt;

parser parse_ethernet {
    extract(pkt);
    return ingress;
}

action do_nothing(){
    no_op();
}

action action_0(param0) {
    modify_field(pkt.field_a_32, param0);
}

action action_1(param1) {
    modify_field(pkt.field_b_32, param1);
}

table table_0 {
    reads {
        pkt.field_a_32 : ternary;
    }
    action_profile: shared_action_profile;
    size : 2048;
}

table table_1 {
    reads {
        pkt.field_b_32 : ternary;
    }
    action_profile: shared_action_profile;
    size : 2048;
}

action_profile shared_action_profile {
    actions {
        do_nothing;
        action_0;
        action_1;
    }
    size : 1024;
}

//@pragma stage 0
table table_2 {
    reads {
        pkt.field_a_32 : exact;
    }
    actions {
        do_nothing;
    }
}

/* Main control flow */

control ingress {

    if (pkt.field_i_8 == 0){
        apply(table_0);
    } else {
        apply(table_1);
    }

    apply(table_2);
}
