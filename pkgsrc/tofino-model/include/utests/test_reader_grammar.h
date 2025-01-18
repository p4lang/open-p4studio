/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

#ifndef _UTESTS_TEST_READER_GRAMMAR_
#define _UTESTS_TEST_READER_GRAMMAR_

// You can quickly check changes to this file by compiling test_test_reader :
//    make -C tests utests/test_test_reader
// When adding/changing the actions for rules you also need to update:
//   reader_actions.h 
//   reader_actions.cpp
//   test_reader_reader.cpp  - add to the dummy actions class

#include <string>
#include <iostream>
#include <cinttypes>
#include <iomanip>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_repeat.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/phoenix/fusion/at.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <fstream>


/* use this to get a print out of the data type a rule is generating, by replacing the
    action with  [ Sniffer() ]
   Then use c++filt on the output string to print it nicely
*/
struct Sniffer
{
  typedef void result_type;
  
  template <typename T>
  void operator()(T const&) const { std::cout << typeid(T).name() << "\n"; }
};

// type for parsing register names like:
//   mau_reg_map.rams.map_alu.row[1].adrmux.unitram_config[1][1]
// The string is the name eg "row" and the optional vector is the array index eg: 1,1
//   optional means it is either null or a pointer to the vector.
typedef boost::fusion::vector2< std::string,boost::optional<std::vector<int> > > PathElement;

// lazy function for error reporting - needed so we can only print the one line with the error on!
struct ReportError {
  // the result type must be explicit for Phoenix
  template<typename, typename, typename, typename>
  struct result { typedef void type; };

  // contract the string to the surrounding new-line characters
  template<typename Iter>
  void operator()(Iter first_iter, Iter last_iter,
                  Iter error_iter, const boost::spirit::qi::info& what) const {
    std::string first(first_iter, error_iter);
    std::string last(error_iter, last_iter);
    auto first_pos = first.rfind('\n');
    auto last_pos = last.find('\n');
    auto error_line = ((first_pos == std::string::npos) ? first
                        : std::string(first, first_pos + 1))
                      + std::string(last, 0, last_pos);
    // I couldn't get this to compile
    //auto error_pos = (error_iter - first_iter) + 1;
    //if (first_pos != std::string::npos) {
    //  error_pos -= (first_pos + 1);
    //}
    std::cout << "Parsing error in " << what << std::endl
              << error_line << std::endl
        //              << std::setw(error_pos) << '^'
              << std::endl;
  }
};
const boost::phoenix::function<ReportError> report_error = ReportError();

// The comment skipper - this class defines how to skip comments and whitespace
template<typename ITERATOR> struct CommentSkipper : public boost::spirit::qi::grammar<ITERATOR> {

  CommentSkipper() : CommentSkipper::base_type(skip, "comment") {
    using boost::spirit::repository::qi::confix;
    using boost::spirit::ascii::char_;
    using boost::spirit::qi::eol;
    using boost::spirit::qi::space;
    skip = 
        confix("/*", "*/")[*(char_ - "*/")]  | // C style comment
        confix("//", eol)[*(char_ - eol)] |   // C++ style comment
        space;
  }
  boost::spirit::qi::rule<ITERATOR> skip;
};

// don't need any locals at the moment, but this is where to add them
//   Then you can refer to they as qi::labels::_a  _b etc
//typedef boost::spirit::qi::locals< char,std::string,int,int > my_locals;
typedef boost::spirit::qi::locals< > my_locals;

template<typename ITERATOR, typename ACTION_TYPE>

struct TestReaderGrammar :
    boost::spirit::qi::grammar<ITERATOR, int(), CommentSkipper<ITERATOR>,  my_locals>
{

  TestReaderGrammar() : TestReaderGrammar::base_type(start_rule)
  {
    namespace qi = boost::spirit::qi;
    namespace phoenix = boost::phoenix;
    using qi::lit;
    using qi::string;
    using qi::_val;
    using qi::int_;
    using qi::bool_;
    using qi::hex;
    using qi::char_;
    using phoenix::at_c;
    using phoenix::ref;
    using boost::spirit::attr;


    // Note phoenix::bind can only handle 10 parameters (including class and var, so eight useful ones
    //  the compile will fail if you use more and it will not be obvious from the error messages.
    
    phv_rule = char_("oi")                  // 'o' or 'i'
        >> lit("phv_")
        >> ( string("out") | string("in") ) // string is like lit, but it returns a value
        >> lit("_") >> int_                 
        >> ( -(lit('e') >> int_  )) ;       // the "-" makes this optional returns boost::optional<int>

    // A hex or decimal number. Even though this there is an alternative this just returns an int
    //  because the type of all alternatives is the same
    hex_or_dec = ( ( lit("0x") >> hex ) | int_ );
    // Similar for numbers that need 64 bit - also accept UINT64_C() 
    hex_or_dec_64 = ( ( lit("0x") >> qi::int_parser<uint64_t, 16>{} ) |
                      ( qi::int_parser<uint64_t, 10>{} ) |
                      ( lit("UINT64_C(") >> lit("0x") >> qi::int_parser<uint64_t, 16>{} >> lit(")")) |
                      ( lit("UINT64_C(") >> qi::int_parser<uint64_t, 10>{} >> lit(")")) );

    // rule for recognizing remapping macros (RM_B4, RM_B4_8 etc) that may appear in outwords, uses
    //  _val to set the return value of the rule to the remapped value if one of the macros apprears
    //   or just the value present if there is no macro (once you've used _val one place you have
    //   to use it everywhere)
    hex_or_dec_or_rm_b4_x_rule = ((( lit("RM_B4") >> -(lit('_')>>int_) >> lit('(') >>
                                     hex_or_dec >> lit(')') )
                                   [ _val = phoenix::bind(&ACTION_TYPE::rm_b4_x, &action, qi::_1, qi::_2) ])
                                  | hex_or_dec [ _val = qi::_1 ]);
    
    //  rules to read the register part of this sort of thing
    // tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[0][2], 0x4006);
    array_index_rule = lit('[') >> ( hex_or_dec  % lit("][") ) >> lit(']') ;
    // the -array_index_rule makes the array index optional and means there's a boost optional wrapper around the
    //  vector from the array_index_rule
    path_element_rule = (+( char_("a-zA-Z0-9_-")) >> -array_index_rule);
    register_rule = path_element_rule % '.';

    // this is generated by multi_phv2 in a standard way, so don't parse flexibly
    selector_rule = ( lit("{")
        >> ( lit("MauLogicalRow *logrow = mau->logical_row_lookup(") >> hex_or_dec >> ");" )
        >> ( lit("BitVector<RmtDefs::kDataBusWidth> data_in(UINT64_C(0));") )
        >> ( lit("logrow->get_selector_alu_input_data(&data_in);") )
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> lit(",") >> lit("data_in.get_word(0,  32));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> lit(",") >> lit("data_in.get_word(32, 32));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> lit(",") >> lit("data_in.get_word(64, 32));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> lit(",") >> lit("data_in.get_word(96, 32));"))
                      >> lit("}") )
        [ phoenix::bind(&ACTION_TYPE::selector_test, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5) ];

    // these are only needed to keep the number of parameters below 10 (there is no qi::_10)
    //  don't know if there's a better way. Also the phoenix::bind call can have at most 10
    //  parameters, which leaves only eight after the function and the object.
    data_in_bv_rule = 
           ( lit("data_in_bv.set32(0,") >> hex_or_dec >> ");" )
        >> ( lit("data_in_bv.set32(1,") >> hex_or_dec >> ");" )
        >> ( lit("data_in_bv.set32(2,") >> hex_or_dec >> ");" )
        >> ( lit("data_in_bv.set32(3,") >> hex_or_dec >> ");" );
    // note optional "u" as some have this following hex number!
    data_out_bv_rule = 
           ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("data_out_bv.get_word(") >> lit("0") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("data_out_bv.get_word(") >> lit("32") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("data_out_bv.get_word(") >> lit("64") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("data_out_bv.get_word(") >> lit("96") >> ","  >> lit("32")>> lit("));"));

    // this is generated by stateful_extract in a standard way, so don't parse flexibly
    stateful_rule = (
        ( lit("phv_data_word =") >> hex_or_dec_64 >> lit(";") ) // q1
        >>   data_in_bv_rule // q2
        >> ( lit("addr =") >> hex_or_dec >> lit(';') ) // q3
        >> (( lit("forwarding =") >> bool_ >> lit(';') ) | qi::attr(false)) // q4: optional with default value of false
        // some old tests don't mention forwarding here, so make it optional
        >> ( lit("salu->calculate_output(addr,phv_data_word,&data_in_bv,&data_out_bv,&action_out") >> -lit(",forwarding") >> lit(");") )
        >>   data_out_bv_rule // q5
        >> -( lit("EXPECT_EQ(") >> hex_or_dec >> lit(",action_out);")) // q6: optional action out without default
                     )
    [ phoenix::bind(&ACTION_TYPE::stateful_test, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6 ) ];

    phv_data_word_rule = 
           ( lit("phv_data_word.set32(0,") >> hex_or_dec >> ");" )
        >> ( lit("phv_data_word.set32(1,") >> hex_or_dec >> ");" )
        >> ( lit("phv_data_word.set32(2,") >> hex_or_dec >> ");" )
        >> ( lit("phv_data_word.set32(3,") >> hex_or_dec >> ");" );


    action_out_bv_rule = 
           ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("action_out_bv.get_word(") >> lit("0") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("action_out_bv.get_word(") >> lit("32") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("action_out_bv.get_word(") >> lit("64") >> ","  >> lit("32")>> lit("));"))
        >> ( lit("EXPECT_EQ(") >> hex_or_dec >> -lit("u") >> lit(",") >> lit("action_out_bv.get_word(") >> lit("96") >> ","  >> lit("32")>> lit("));"));

    bool_vector_rule = lit('{') >> ( bool_  % lit(',') ) >> lit('}') ;

    // Separate this out to keep the total number of args in the phoenix::bind for stateful_test_jbay below 10
    stateful_params_rule_jbay = 
           ( lit("addr =") >> hex_or_dec >> lit(';') ) 
        >> ( lit("present_time =") >> hex_or_dec_64 >> lit(";") ) 
        >> ( lit("match_bus") >> lit('=') >> bool_vector_rule >> lit(';') ) 
        >> ( lit("learn_or_match_bus") >> lit('=') >> bool_vector_rule >> lit(';') ) ;
    stateful_rule_jbay = (
              phv_data_word_rule // q1
           >> data_in_bv_rule // q2
           >> stateful_params_rule_jbay // q3
           >> lit("salu->reset_resources();")
           >> -( lit("salu->set_random_number_value(") >> hex_or_dec_64 >> lit(");")) // q4 optional
           >> lit("salu->calculate_output(addr,phv_data_word,&data_in_bv,&data_out_bv,&action_out_bv,present_time,true,match_bus,learn_or_match_bus);")
           >> data_out_bv_rule // q5
           >> action_out_bv_rule // q6
                          )
        [ phoenix::bind(&ACTION_TYPE::stateful_test_jbay, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5, qi::_6 ) ];
    
    start_rule = qi::eps > + (
        // iphv_in_1e0->set(  0, 0x54c00ea6);
        (( phv_rule >> lit("->") >> lit("set") >> lit('(') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(')')>>lit(';')
           // want to get the v=0 or v=1 out of the trailing comment, so have to turn the skipper off and consume the rest of the line:
           //   /* [0,16] v=0  bytes: 67- 64  #274e1# Rtl-Output oPhv  */
           >> qi::no_skip[ *(char_ - 'v') >> lit("v=") >> int_ >> *(char_ - qi::eol) >> qi::eol ] 
           )
         // at_c are from the phv_rule, _2 and _3 are from the hex_or_dec's and _4 from the int_ after v=
         [phoenix::bind(&ACTION_TYPE::set, &action, at_c<0>(qi::_1), at_c<1>(qi::_1), at_c<2>(qi::_1), at_c<3>(qi::_1), qi::_2,qi::_3,at_c<1>(qi::_4))] )

        //Phv *iphv_in_2e0 =  tu.phv_alloc();
        | (( lit("Phv") >> lit("*") >> phv_rule >> lit("=") >> lit("tu.phv_alloc();") )
           // when there is phv_rule and nothing else then the phv_rule's values magically get moved up to _1..._4 somehow
           [phoenix::bind(&ACTION_TYPE::phv_alloc, &action, qi::_1, qi::_2, qi::_3, qi::_4)] )
                        
        | (( phv_rule >> lit("->") >> lit("set_ingress") >> lit('(') >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_ingress, &action, qi::_1, qi::_2, qi::_3, qi::_4)] )
        | (( phv_rule >> lit("->") >> lit("set_egress") >> lit('(') >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_egress, &action, qi::_1, qi::_2, qi::_3, qi::_4)] )
        | (( phv_rule >> lit("->") >> lit("set_ghost") >> lit('(') >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_ghost, &action, qi::_1, qi::_2, qi::_3, qi::_4)] )

        | (( phv_rule >> lit("->") >> lit("set_version") >> lit('(') >> hex_or_dec >> lit(',') >> bool_ >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_version, &action, at_c<0>(qi::_1), at_c<1>(qi::_1), at_c<2>(qi::_1), at_c<3>(qi::_1), qi::_2,qi::_3)] )

        // note: the optional "e[01]" was inserted by a bug in the test generating script long ago
        | (( phv_rule >> lit("->") >> lit("set_relative_time") >> lit('(') >> hex_or_dec_64 >> -(char_('e')>>char_("01"))>> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_relative_time, &action, at_c<0>(qi::_1), at_c<1>(qi::_1), at_c<2>(qi::_1), at_c<3>(qi::_1), qi::_2)] )

        | (( phv_rule >> lit("->") >> lit("set_meter_tick_time") >> lit('(') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec_64 >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_meter_tick_time, &action, at_c<0>(qi::_1), at_c<1>(qi::_1), at_c<2>(qi::_1), at_c<3>(qi::_1), qi::_2, qi::_3, qi::_4 )] )

        | (( phv_rule >> lit("->") >> lit("set_meter_random_value") >> lit('(') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec_64 >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_meter_random_value, &action, at_c<0>(qi::_1), at_c<1>(qi::_1), at_c<2>(qi::_1), at_c<3>(qi::_1), qi::_2, qi::_3, qi::_4 )] )

        // mau->set_pred(0xAAA,0xBBB,0xCCC,0xDDDD,0xEE) OR mau->set_mpr(0xAAA,0xBBB,0xCCC,0xDDDD,0xEE)
        | (( lit("mau->set_pred") >> lit('(') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_pred, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )

        | (( lit("mau->set_mpr") >> lit('(') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(',') >> hex_or_dec >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_mpr, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )

        // mau->set_ingress|egress_snapshot_triggered(true);
        | (( lit("mau->set_ingress_snapshot_triggered") >> lit('(') >> bool_ >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_ingress_snapshot_triggered, &action, qi::_1  )] )
        | (( lit("mau->set_egress_snapshot_triggered") >> lit('(') >> bool_ >> lit(')')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::set_egress_snapshot_triggered, &action, qi::_1  )] )

        //  Note: attr() is used to make the _e[01] part optional by providing a default if it is not present
        //   resulting values are stored in member variables pred_ingress_start_table, pred_egress_start_table and pred_ghost_start_table for use in process_match call later
        | (( lit("int") >> lit("ingress_next_table_") >> int_ >> ((lit('e') >> int_)|attr(0)) >> lit('=') >> hex_or_dec >> lit(';') ) [ ref(pred_ingress_start_table) = qi::_3  ] ) 
        | (( lit("int") >> lit("egress_next_table_")  >> int_ >> ((lit('e') >> int_)|attr(0)) >> lit('=') >> hex_or_dec >> lit(';') ) [ ref(pred_egress_start_table) =  qi::_3  ] )
        | (( lit("int") >> lit("pred_ingress_next_table_") >> int_ >> ((lit('e') >> int_)|attr(0)) >> lit('=') >> hex_or_dec >> lit(';') ) [ ref(pred_ingress_start_table) = qi::_3  ] ) 
        | (( lit("int") >> lit("pred_egress_next_table_")  >> int_ >> ((lit('e') >> int_)|attr(0)) >> lit('=') >> hex_or_dec >> lit(';') ) [ ref(pred_egress_start_table) =  qi::_3  ] )
        | (( lit("int") >> lit("pred_ghost_next_table_")  >> int_ >> ((lit('e') >> int_)|attr(0)) >> lit('=') >> hex_or_dec >> lit(';') ) [ ref(pred_ghost_start_table) =  qi::_3  ] )

        | (( lit("Phv") >> lit('*') >> lit("iphv_out_got_") >> int_ >> ((lit('e') >> int_) | attr(0)) >> lit('=') >> lit("mau->process_match2") >> +(char_ - ';')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::process_match2, &action, qi::_1, qi::_2, &pred_ingress_start_table, &pred_egress_start_table, &pred_ghost_start_table)] )

        | (( lit("Phv") >> lit('*') >> lit("iphv_out_got_") >> int_ >> ((lit('e') >> int_) | attr(0)) >> lit('=') >> lit("mau->process_match") >> +(char_ - ';')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::process_match, &action, qi::_1, qi::_2, &pred_ingress_start_table, &pred_egress_start_table)] )

        | (( lit("Phv") >> lit('*') >> lit("ophv_out_got_") >> int_ >> ((lit('e') >> int_) | attr(0)) >> lit('=') >> lit("mau->process_action") >> +(char_ - ';')>>lit(';') )
           [phoenix::bind(&ACTION_TYPE::process_action, &action, qi::_1, qi::_2 )] )
                     
        //{ uint8_t  eopnum  = Eop::make_eopnum(0x8, 1); iphv_in_2e0->set_eopnum(eopnum, true); } 
        | (( lit("{") >> lit("uint8_t") >> lit("eopnum") >> lit("=") >> lit("Eop::make_eopnum") >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(")") >> lit(";")
             >> phv_rule >> lit("->") >> lit("set_eopnum") >> lit("(") >> lit("eopnum") >> lit(",") >> bool_ >> lit(")") >> lit(";") >> lit("}") )
           [phoenix::bind(&ACTION_TYPE::set_eopnum, &action, at_c<0>(qi::_3), at_c<1>(qi::_3), at_c<2>(qi::_3), at_c<3>(qi::_3), qi::_1,qi::_2,qi::_4)] )

        // Eop eop_1_0{};
        //  Note: attr() is used to make the _int part optional by providing a default if it is not present - I could
        //     probably have done the phv_rule that way as well.
        | (( lit("Eop") >> lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit("{") >> lit("}") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::new_eop, &action, qi::_1, qi::_2 )] )

        //eop_1_0.set_ingress_eopinfo(0x18b3, 0x41); /* RefModel Meter Eop#1e0# */  - ootional error parameter, defaults to 0
        | (( lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_ingress_eopinfo")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec  >> ((lit(",") >> hex_or_dec)|attr(0)) >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::set_ingress_eopinfo, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5  )] )
        | (( lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_egress_eopinfo")   >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec  >> ((lit(",") >> hex_or_dec)|attr(0)) >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::set_egress_eopinfo, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5  )] )

        // eop_1_1.set_meter_tick_time(0,  1, 0x1F);
        | (( lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_meter_tick_time")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::eop_set_meter_tick_time, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )
        // eop_1_1.set_meter_random_value(0,  1, 0x429F495F);
        | (( lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_meter_random_value")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::eop_set_meter_random_value, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )
        
        // eop_1_1.set_relative_time(67);
        | (( lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_relative_time")  >> lit("(") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::eop_set_relative_time, &action, qi::_1, qi::_2, qi::_3 )] )
        
        // mau->handle_eop(eop_1_0);
        | (( lit("mau") >> lit("->")  >> lit("handle_eop") >> lit("(") >> lit("eop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::handle_eop, &action, qi::_1, qi::_2 )] )

        // Teop dteop_1_0{};
        //  Note: attr() is used to make the _int part optional by providing a default if it is not present - I could
        //     probably have done the phv_rule that way as well.
        | (( lit("Teop") >> lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit("{") >> lit("}") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::new_teop, &action, qi::_1, qi::_2 )] )

        // dteop_1_1.set_addr(0, 0x4321);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_raw_addr")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_raw_addr, &action, qi::_1, qi::_2, qi::_3, qi::_4 )] )

        // dteop_1_1.set_byte_len(987);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_byte_len")  >> lit("(") >> hex_or_dec >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_byte_len, &action, qi::_1, qi::_2, qi::_3 )] )

        // dteop_1_1.set_error(0);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_error")  >> lit("(") >> hex_or_dec >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_error, &action, qi::_1, qi::_2, qi::_3 )] )

        // dteop_1_1.set_meter_tick_time(0,  1, 0x1F);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_meter_tick_time")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_meter_tick_time, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )

        // dteop_1_1.set_meter_random_value(0,  1, 0x429F495F);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_meter_random_value")  >> lit("(") >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_meter_random_value, &action, qi::_1, qi::_2, qi::_3, qi::_4, qi::_5 )] )
        
        // dteop_1_1.set_relative_time(67);
        | (( lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(".set_relative_time")  >> lit("(") >> hex_or_dec_64  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::teop_set_relative_time, &action, qi::_1, qi::_2, qi::_3 )] )
        
        // mau->handle_dp_teop(dteop_1_0);
        | (( lit("mau") >> lit("->")  >> lit("handle_dp_teop") >> lit("(") >> lit("dteop_") >> int_ >> ((lit("_") >> int_)|attr(0)) >> lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::handle_dp_teop, &action, qi::_1, qi::_2 )] )

        // tu.Reset();
        | (( lit("tu.Reset") >> lit("(") >> lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::reset, &action )] )
        
        // tu.IndirectWrite(0x020080100000, 0x0000000000000001, 0x0000000000000000);
        | (( lit("tu.IndirectWrite") >> lit("(")
             >> hex_or_dec_64
             >> lit(",") >> hex_or_dec_64
             >> lit(",") >> hex_or_dec_64
             >> -( lit(",") >> hex_or_dec_64 ) // optional time
             >> lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::indirect_write, &action, qi::_1, qi::_2, qi::_3, qi::_4 )] )
        
        | (( lit("tu.RemapIndirectWrite") >> lit("(") >>
             hex_or_dec >> lit(",") >>
             hex_or_dec >> lit(",") >>
             hex_or_dec >> lit(",") >>
             hex_or_dec >> lit(",") >>
             hex_or_dec_64 >> lit(",") >>
             hex_or_dec_64 >> lit(",") >>
             hex_or_dec_64 >> lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::remap_indirect_write, &action, qi::_1, qi::_2, qi::_3,
                          qi::_4, qi::_5, qi::_6, qi::_7 )] )

        | ( selector_rule )

        | ( stateful_rule )
        | ( stateful_rule_jbay )

        // tu.OutWord(&mau_reg_map.rams.map_alu.row[6].adrmux.unitram_config[0][2], 0x4006);
        | (( lit("tu.OutWord") >> lit('(') >> lit('&') >> register_rule >> lit(",") >>
             hex_or_dec_or_rm_b4_x_rule  >> lit(")")>> lit(";") ) 
           [phoenix::bind(&ACTION_TYPE::out_word, &action, qi::_1, qi::_2 )] )

        // tu.OutWordPiT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xffffffff);
        | (( lit("tu.OutWordPiT") >> lit('(') >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >>
             lit('&') >> register_rule >> lit(",") >> hex_or_dec  >> lit(")")>> lit(";") ) 
           [phoenix::bind(&ACTION_TYPE::out_word_pit, &action, qi::_1, qi::_2, qi::_3, qi::_4 )] )

        // tu.OutWordPeT(0, 0, &mau_reg_map.dp.phv_ingress_thread[0][0], 0xffffffff);
        | (( lit("tu.OutWordPeT") >> lit('(') >> hex_or_dec >> lit(",") >> hex_or_dec >> lit(",") >>
             lit('&') >> register_rule >> lit(",") >> hex_or_dec  >> lit(")")>> lit(";") ) 
           [phoenix::bind(&ACTION_TYPE::out_word_pet, &action, qi::_1, qi::_2, qi::_3, qi::_4 )] )

        // tu.OutWord(0x1234000, 0x4006);
        | (( lit("tu.OutWord") >> lit('(') >> hex_or_dec >> lit(",") >>
             hex_or_dec_or_rm_b4_x_rule  >> lit(")")>> lit(";") )
           [phoenix::bind(&ACTION_TYPE::out_word_addr, &action, qi::_1, qi::_2 )] )


        //ActionHvTranslator act_hv_translator;
        | (( lit("ActionHvTranslator") >> lit("act_hv_translator")  >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::aht_new, &action )] )

        // act_hv_translator.ctl_word(7,1,0,0x1);
        | (( lit("act_hv_translator.ctl_word(") >> hex_or_dec >> lit(",") >>
             hex_or_dec >> lit(",") >> hex_or_dec >> lit(",")>> hex_or_dec >>
             lit(")") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::aht_ctl_word, &action, qi::_1, qi::_2 , qi::_3 , qi::_4 )] )

        // act_hv_translator.do_writes(&tu);
        | (( lit("act_hv_translator.do_writes(&tu)") >> lit(";") )
           [phoenix::bind(&ACTION_TYPE::aht_do_writes, &action )] )
        
        // stuff to ignore
        | ( ( lit("EXPECT_TRUE")
            | lit("RMT_UT_LOG_INFO")
            | lit("RmtObjectManager *om =")
            | lit("Mau *mau =")
            | lit("Port *port =")
            | lit("flags =")
            | lit("tu.update_log_flags(")
            | lit("printf(")
              )
            >> +(char_ - ';') >> lit(';') )
        // stuff to ignore until the eol (note: have to turn the skipper off to get the eol to the parser
        | ( (lit("#pragma")|lit("#if")|lit("#define")|lit("#endif")) >> qi::no_skip[ *(char_ - qi::eol) >> qi::eol ] )
        
      ) > qi::eoi; // expect last rule to be followed by end of input

    start_rule.name("start_rule");
    
    qi::on_error<qi::fail>
        (
            start_rule,
            report_error(qi::labels::_1, qi::labels::_2, qi::labels::_3, qi::labels::_4)
         );

  }

  ACTION_TYPE action;
 private:
  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR>, my_locals> start_rule;
  boost::spirit::qi::rule<ITERATOR, boost::fusion::vector4<char,std::string,int, boost::optional<int> >() > phv_rule;
  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR> > selector_rule;
  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR> > stateful_rule;
  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR> > stateful_rule_jbay;
  boost::spirit::qi::rule<ITERATOR, boost::fusion::vector4<int, uint64_t ,std::vector<bool>,std::vector<bool>>(), CommentSkipper<ITERATOR> > stateful_params_rule_jbay;


  
    
  boost::spirit::qi::rule<ITERATOR, std::vector<uint32_t>(), CommentSkipper<ITERATOR> > data_out_bv_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<uint32_t>(), CommentSkipper<ITERATOR> > data_in_bv_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<uint32_t>(), CommentSkipper<ITERATOR> > phv_data_word_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<uint32_t>(), CommentSkipper<ITERATOR> > action_out_bv_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<bool>(), CommentSkipper<ITERATOR> > bool_vector_rule;

  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR>> hex_or_dec;
  boost::spirit::qi::rule<ITERATOR, uint64_t(), CommentSkipper<ITERATOR>> hex_or_dec_64;
  boost::spirit::qi::rule<ITERATOR, int(), CommentSkipper<ITERATOR>> hex_or_dec_or_rm_b4_x_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<int>(), CommentSkipper<ITERATOR> > array_index_rule;
  boost::spirit::qi::rule<ITERATOR, PathElement(), CommentSkipper<ITERATOR> > path_element_rule;
  boost::spirit::qi::rule<ITERATOR, std::vector<PathElement>(), CommentSkipper<ITERATOR> > register_rule;

  int pred_ingress_start_table=-1;
  int pred_egress_start_table=-1;
  int pred_ghost_start_table=-1;
  
};

#endif // _UTESTS_TEST_READER_GRAMMAR_
