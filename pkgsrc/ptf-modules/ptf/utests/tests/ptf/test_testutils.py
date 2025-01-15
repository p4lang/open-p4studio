from ptf.testutils import simple_igmp_packet


def test_simple_igmp_packet__proper_setting_mrtime_mrcode():
    simple_packet = simple_igmp_packet(igmp_mrtime=10)
    assert simple_packet["IGMP"].mrcode == 10
