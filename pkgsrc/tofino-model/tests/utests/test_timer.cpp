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

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>
#include <cinttypes>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gtest.h"
#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <model_core/timer-manager.h>
#include <mau.h>
#include <packet.h>



namespace MODEL_CHIP_TEST_NAMESPACE {

  bool test_timer_print = true;
  uint64_t stop_timer_id = 0;
  uint64_t restart_timer_id = 0;

  using namespace std;
  using namespace model_timer;

  class timerTest {
    public:
      timerTest(uint64_t id) : 
        timer1([this](uint64_t tid){this->testCallBack(tid);}) 
      { id_ = id; cb_count_ = 0;}

      void testCallBack(uint64_t tid) 
      { 
          uint64_t current_time;
          model_timer::ModelTimerGetTime(current_time);
          cb_count_++;
          RMT_UT_LOG_INFO("Timer fired %" PRIu64 " at %" PRIu64 "count %" PRIu64 "\n", tid, current_time, cb_count_); 
          if (tid == stop_timer_id) {
            timer1.stop();
          }
          if (tid == restart_timer_id) {
            timer1.stop();  // need to explicitly stop it.. will fix it in future
            timer1.run(10);
          }
      }
      uint64_t id_;
      uint64_t cb_count_;
      
      Timer timer1;

  };
  
  class LinkTimer {
	public:
		LinkTimer(void):
		p_timer([&](uint64_t tid){packet_cb(); }),
		b_timer([&](uint64_t tid){batch_cb(); }),
		p_jitter_delay([&](uint64_t tid){packet_jitter_cb(); }),
		b_jitter_delay([&](uint64_t tid){batch_jitter_cb(); }),
		p_delay(10),
		b_delay(40),
		p_count(0),
		b_count(0),
		p_rand(true),
		b_rand(true),
		n_pkts(3),
		n_batches(4),
		done_flag_(false),
		b_expired(false) {
			srand(time(NULL));
		}
		~LinkTimer(void){}
	
		Timer p_timer;
		Timer b_timer;
		Timer p_jitter_delay;
		Timer b_jitter_delay;

		uint32_t p_delay;
		uint32_t b_delay;
		std::atomic<uint32_t> p_count;
		std::atomic<uint32_t> b_count;
		bool p_rand;
		bool b_rand;
		uint32_t n_pkts;
		uint32_t n_batches; 
		bool done_flag_;
		std::atomic<bool>b_expired;

		uint32_t rand_b_val;
		uint32_t rand_p_val;	

	
		void new_batch(void) {

			uint64_t tm;
			ModelTimerGetTime(tm);
			if (b_count.load() == 0) {
				p_count = 0;
				b_expired = false;
				b_timer.run(b_delay, model_timer::Timer::Once);
				new_packet();

			} else {
				if (b_count.load() < n_batches) {
					if (p_count.load() == n_pkts) {
						p_count = 0;
						b_timer.stop();
						b_expired = false;
						b_timer.run(b_delay, model_timer::Timer::Once);
						new_packet();
					}
				} else {
					finalize();
				}		
			}	
		}			
		void new_packet(void) {
			uint64_t tm;
			ModelTimerGetTime(tm);
			if (p_count.load() == n_pkts) {
				if (b_expired.load()) {
					if (b_count.load() == (n_batches - 1)) {
							finalize();
							return;
					}
					b_count ++;					
					new_batch();
				} 
				return;
			} else {
				//cout << "CLK:" << tm << ":BATCH:" << b_count.load() << ":PKT:" << p_count.load() << endl;
				/*
			 	* Packet processing here
			 	*/
				p_timer.stop();
				p_timer.run(p_delay, model_timer::Timer::Once);
				p_count ++;
			}
		}
		
		void packet_cb(void) {
			uint64_t tm;
			ModelTimerGetTime(tm);
			if (p_rand) {
				rand_p_val = rand()%32;
				p_jitter_delay.run(1, model_timer::Timer::Once);		
			} else {
				new_packet();
			}					

		}
		void batch_cb(void) {
			uint64_t tm;
			ModelTimerGetTime(tm);

			if (b_rand) {
				rand_b_val = rand()%32;
				b_jitter_delay.run(1, model_timer::Timer::Once);
			} else {
				if (p_count.load() < n_pkts) {
					b_expired = true;
				} else {
					b_count ++;					
					new_batch();
				}
			}					
		}
		void packet_jitter_cb(void) {
			uint32_t random_val = rand()%32;
			uint64_t tm;
			ModelTimerGetTime(tm);
			
			if (random_val == rand_p_val) {
				p_jitter_delay.stop();
				new_packet();
			} else {
				p_jitter_delay.stop();
				p_jitter_delay.run(1, model_timer::Timer::Once);
			}
		}
		void batch_jitter_cb(void) {
			uint32_t random_val = rand()%32;
			uint64_t tm;
			ModelTimerGetTime(tm);
			if (random_val == rand_b_val) {
				b_jitter_delay.stop();
				if (p_count.load() < n_pkts) {
					b_expired = true;
				} else {
					b_count ++;
					new_batch();
				}
			} else {
				b_jitter_delay.stop();
				b_jitter_delay.run(1, model_timer::Timer::Once);

			}
		}
		void finalize() {
			done_flag_ = 1;

		}
   }; 


  TEST(BFN_TEST_NAME(ModelTimerTest), ModelTimerTest_1) {
    timerTest *t1[10];
    timerTest *t2[10];

    // set timer scale to 1
    getTimerManager()->set_scale(1);
    getTimerManager()->set_time(0);

    RMT_UT_LOG_INFO("====== Creating Timers"); 
    for (int i=0; i<20; i++) {
      if (i%2) {
        t1[i/2] = new timerTest(i+10);
      } else {
        t2[i/2] = new timerTest(i+10);
      }
    }
    // Add t1 timers w/ monotonically incresing times
    // Add t2 timers w/ decresing times (insertion in the list)
    RMT_UT_LOG_INFO("====== Run Timers"); 
    for (int i=1; i<20; i+=2) {
        t1[i/2]->timer1.run(i*3, model_timer::Timer::Recurring);
        if (test_timer_print)
          RMT_UT_LOG_INFO("timer %" PRIu64 ", t_val %" PRIu64, t1[i/2]->timer1.id(), t1[i/2]->timer1.timeout());
    }
    for (int i=0; i<20; i+=2) {
        t2[i/2]->timer1.run((20-i)*3);
        if (test_timer_print)
          RMT_UT_LOG_INFO("timer %" PRIu64 ", t_val %" PRIu64, t2[i/2]->timer1.id(), t2[i/2]->timer1.timeout());
    }
    // Run few timers, cancel(stop) expired and some peding timers
    RMT_UT_LOG_INFO("====== Run Timers upto timeout = 30"); 
    for (int j=0; j<15; j++) {
        model_timer::ModelTimerIncrement(2);
    }
    for (int i=0; i<10; i++) {
      // check if all the timers w/ timout <30 have fired
      if (t1[i]->timer1.is_running() && t1[i]->timer1.timeout() <= 30) {
        EXPECT_EQ(30/t1[i]->timer1.timeout(), t1[i]->cb_count_);
      }
      if (!t1[i]->timer1.is_running() || t1[i]->timer1.timeout() > 30) {
        EXPECT_EQ(0u, t1[i]->cb_count_);
      }
      if (t2[i]->timer1.is_running() && t2[i]->timer1.timeout() <= 30) {
        EXPECT_EQ(1u, t2[i]->cb_count_);
      }
      if (!t2[i]->timer1.is_running() || t2[i]->timer1.timeout() > 30) {
        EXPECT_EQ(0u, t2[i]->cb_count_);
      }
    }
    uint64_t current_time;
    model_timer::ModelTimerGetTime(current_time);
    EXPECT_EQ(30u, current_time);
    RMT_UT_LOG_INFO("====== Stop some of the  timers"); 
    for (int i=1; i<20; i+=4) {
        t1[i/2]->timer1.stop();
    }
    for (int i=0; i<20; i+=4) {
        t2[i/2]->timer1.stop();
    }
    RMT_UT_LOG_INFO("====== Set time to 60 units"); 
    model_timer::ModelTimerSetTime(60);

    for (int i=0; i<10; i++) {
      // if (test_timer_print) RMT_UT_LOG_INFO("Test timer %d", t1[i]->timer1.id());
      if (t1[i]->timer1.is_running()) {
        if (t1[i]->timer1.timeout() <= 30) {
          // t1 timers are recurring
          EXPECT_EQ(60/t1[i]->timer1.timeout(), t1[i]->cb_count_);
        } else {
          EXPECT_EQ(60/t1[i]->timer1.timeout(), t1[i]->cb_count_);
        }
      }
      if (t2[i]->timer1.is_running())
      {
        if (t2[i]->timer1.timeout() <= 30) {
          // no change to single shot timers
          EXPECT_EQ(1u, t2[i]->cb_count_);
        } else {
          EXPECT_EQ(1u, t2[i]->cb_count_);
        }
      }
    }
    RMT_UT_LOG_INFO("====== Special tests stop/start timers in callback func"); 
    timerTest *timer_s = new timerTest(100);
    timerTest *timer_r = new timerTest(101);
    stop_timer_id = timer_s->timer1.id();
    restart_timer_id = timer_r->timer1.id();

    timer_s->timer1.run(10, model_timer::Timer::Recurring);
    timer_r->timer1.run(10);

    model_timer::ModelTimerIncrement(15);
    EXPECT_EQ(1u, timer_s->cb_count_);
    EXPECT_EQ(1u, timer_r->cb_count_);

    model_timer::ModelTimerIncrement(15);
    EXPECT_EQ(1u, timer_s->cb_count_);   // stopped by WIP
    EXPECT_EQ(3u, timer_r->cb_count_);   // restarted by WIP for 10 ticks


    RMT_UT_LOG_INFO("====== Delete all timers"); 
    for (int i=0; i<10; i++) {
      delete t1[i];
      delete t2[i];
    }
    delete timer_s;
    delete timer_r;
    model_timer::ModelTimerGetTime(current_time);
    EXPECT_EQ(90u, current_time);
  }

  TEST(BFN_TEST_NAME(ModelTimerTest), LinkedPktGenTimers) {
	/*
	 * This test case is for testing linked timers and to see if they 
	 * fire correctly. 
     * For timers, every timer.run must have a corresponding timer.stop
	 * Recurring timers are best avoided for fine grained control using stop.
	 */

	LinkTimer ltt;
	
    getTimerManager()->set_scale(1);
    getTimerManager()->set_time(0);
	uint64_t clock = 0;
	


	ltt.p_delay = 1;
	ltt.b_delay = 3;


	ltt.new_batch();




	while (clock < 10000) {
		model_timer::ModelTimerIncrement(1);
		clock++;


	}	
	EXPECT_EQ(ltt.done_flag_, true);




   }



}
