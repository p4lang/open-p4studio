################################################################################
 #  Copyright (C) 2024 Intel Corporation
 #
 #  Licensed under the Apache License, Version 2.0 (the "License");
 #  you may not use this file except in compliance with the License.
 #  You may obtain a copy of the License at
 #
 #  http://www.apache.org/licenses/LICENSE-2.0
 #
 #  Unless required by applicable law or agreed to in writing,
 #  software distributed under the License is distributed on an "AS IS" BASIS,
 #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 #  See the License for the specific language governing permissions
 #  and limitations under the License.
 #
 #
 #  SPDX-License-Identifier: Apache-2.0
################################################################################


"""
Thrift SAI interface Scheduler tests
"""

from sai_base_test import *


class SchedulerParamsTest(SaiHelper):
    '''
    Basic scheduler parameters tests
    '''

    def setUp(self):
        super(SchedulerParamsTest, self).setUp()

        attr = sai_thrift_get_port_attribute(self.client,
                                             self.port1,
                                             qos_number_of_queues=True)
        num_queues = attr["qos_number_of_queues"]
        queue_list = sai_thrift_object_list_t(count=num_queues)
        port_attr = sai_thrift_get_port_attribute(
            self.client,
            self.port1,
            qos_queue_list=queue_list)

        self.test_queue = port_attr['qos_queue_list'].idlist[0]

        cpu_queue_list = sai_thrift_object_list_t(count=40)

        cpu_port_attr = sai_thrift_get_port_attribute(
            self.client,
            self.cpu_port_hdl,
            qos_queue_list=cpu_queue_list)

        self.cpu_test_queue = cpu_port_attr['qos_queue_list'].idlist[0]

    def runTest(self):
        try:
            self.schedulerWeightTest(self.test_queue)
            self.schedulerWeightTest(self.cpu_test_queue)
            self.schedulerStictPriorityTest(self.test_queue)
            self.schedulerStictPriorityTest(self.cpu_test_queue)
            self.schedulerMinBwidthRateTest(self.test_queue)
            self.schedulerMinBwidthRateTest(self.cpu_test_queue)
            self.schedulerMaxBwidthRateTest(self.test_queue)
            self.schedulerMaxBwidthRateTest(self.cpu_test_queue)
            self.schedulerMinBwidthBurstRateTest(self.test_queue)
            self.schedulerMinBwidthBurstRateTest(self.cpu_test_queue)
            self.schedulerMaxBwidthBurstRateTest(self.test_queue)
            self.schedulerMaxBwidthBurstRateTest(self.cpu_test_queue)
        finally:
            pass

    def schedulerWeightTest(self, queue):
        '''
        This verifies creation of scheduler with scheduling type DWRR
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerWeightTest() for cpu queue")
        else:
            print("\nschedulerWeightTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                scheduling_type=SAI_SCHEDULING_TYPE_DWRR,
                scheduling_weight=2)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                scheduling_type=True,
                scheduling_weight=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_DWRR)
            self.assertEqual(sched_attr['scheduling_weight'], 2)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(self.client,
                                                        sched,
                                                        scheduling_weight=4)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, scheduling_weight=True)
            self.assertEqual(sched_attr['scheduling_weight'], 4)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerStictPriorityTest(self, queue):
        '''
        This verifies creation of scheduler with priority set
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerStrictPriorityTest() for cpu queue")
        else:
            print("\nschedulerStrictPriorityTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, meter_type=True, scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMinBwidthRateTest(self, queue):
        '''
        This verifies creation of scheduler with min bandwidth rate set
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerMinBwidthRateTest() for cpu queue")
        else:
            print("\nschedulerMinBwidthRateTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                min_bandwidth_rate=100,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                min_bandwidth_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['min_bandwidth_rate'], 100)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(self.client,
                                                        sched,
                                                        min_bandwidth_rate=200)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, min_bandwidth_rate=True)
            self.assertEqual(sched_attr['min_bandwidth_rate'], 200)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMaxBwidthRateTest(self, queue):
        '''
        This verifies creation of scheduler with max bandwidth rate set
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerMaxBwidthRateTest() for cpu queue")
        else:
            print("\nschedulerMaxBwidthRateTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, max_bandwidth_rate=2000)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, max_bandwidth_rate=True)
            self.assertEqual(sched_attr['max_bandwidth_rate'], 2000)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMinBwidthBurstRateTest(self, queue):
        '''
        This verifies creation of scheduler with min bandwidth burst rate set
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerMinBwidthBurstRateTest() for cpu queue")
        else:
            print("\nschedulerMinBwidthBurstRateTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                min_bandwidth_burst_rate=100,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                min_bandwidth_burst_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['min_bandwidth_burst_rate'], 100)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, min_bandwidth_burst_rate=200)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, min_bandwidth_burst_rate=True)
            self.assertEqual(sched_attr['min_bandwidth_burst_rate'], 200)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMaxBwidthBurstRateTest(self, queue):
        '''
        This verifies creation of scheduler with max bandwidth burst rate set
        for non-cpu and cpu queues

        :param uint64 queue: queue to test, non-cpu or cpu
        '''
        if queue == self.cpu_test_queue:
            print("\nschedulerMaxBwidthBurstRateTest() for cpu queue")
        else:
            print("\nschedulerMaxBwidthBurstRateTest() for non-cpu queue")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_burst_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_burst_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_burst_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_queue_attribute(self.client,
                                                    queue,
                                                    scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            queue_attr = sai_thrift_get_queue_attribute(
                self.client, queue, scheduler_profile_id=True)
            self.assertEqual(queue_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, max_bandwidth_burst_rate=2000)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, max_bandwidth_burst_rate=True)
            self.assertEqual(sched_attr['max_bandwidth_burst_rate'], 2000)

        finally:
            sai_thrift_set_queue_attribute(self.client,
                                           queue,
                                           scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)


class SchedulerGroupAttachTest(SaiHelper):
    '''
    These verify possibilities of attaching scheduler with different params set
    to a scheduler group
    '''

    def setUp(self):
        super(SchedulerGroupAttachTest, self).setUp()

        try:
            self.schgroup = sai_thrift_create_scheduler_group(
                self.client, level=0, port_id=self.port1)
            self.assertTrue(self.schgroup != 0)

        except BaseException:
            print("Failed to create scheduler group")

    def runTest(self):
        try:
            self.schedulerWeightGroupAttachTest()
            self.schedulerStrictPriorityGroupAttachTest()
            self.schedulerMinBwidthRateGroupAttachTest()
            self.schedulerMaxBwidthRateGroupAttachTest()
            self.schedulerMinBwidthBurstRateGroupAttachTest()
            self.schedulerMaxBwidthBurstRateGroupAttachTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_scheduler_group(self.client, self.schgroup)

        super(SchedulerGroupAttachTest, self).tearDown()

    def schedulerWeightGroupAttachTest(self):
        '''
        This verifies creation of scheduler with scheduling type DWRR
        and attaching it to a scheduler group
        '''
        print("\nschedulerWeightGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                scheduling_type=SAI_SCHEDULING_TYPE_DWRR,
                scheduling_weight=2)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                scheduling_type=True,
                scheduling_weight=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_DWRR)
            self.assertEqual(sched_attr['scheduling_weight'], 2)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(self.client,
                                                        sched,
                                                        scheduling_weight=4)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, scheduling_weight=True)
            self.assertEqual(sched_attr['scheduling_weight'], 4)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerStrictPriorityGroupAttachTest(self):
        '''
        This verifies creation of scheduler with priority set
        and attaching it to a scheduler group
        '''
        print("\nschedulerStrictPriorityGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, meter_type=True, scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(self.client,
                                                        sched,
                                                        scheduling_weight=4)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, scheduling_weight=True)
            self.assertEqual(sched_attr['scheduling_weight'], 4)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMinBwidthRateGroupAttachTest(self):
        '''
        This verifies creation of scheduler with min bandwidth rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMinBwidthRateGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                min_bandwidth_rate=100,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                min_bandwidth_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['min_bandwidth_rate'], 100)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(self.client,
                                                        sched,
                                                        min_bandwidth_rate=200)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, min_bandwidth_rate=True)
            self.assertEqual(sched_attr['min_bandwidth_rate'], 200)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMaxBwidthRateGroupAttachTest(self):
        '''
        This verifies creation of scheduler with max bandwidth rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMaxBwidthRateGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, max_bandwidth_rate=2000)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, max_bandwidth_rate=True)
            self.assertEqual(sched_attr['max_bandwidth_rate'], 2000)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMinBwidthBurstRateGroupAttachTest(self):
        '''
        This verifies creation of scheduler with min bandwidth burst rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMinBwidthBurstRateGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                min_bandwidth_burst_rate=100,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                min_bandwidth_burst_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['min_bandwidth_burst_rate'], 100)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, min_bandwidth_burst_rate=200)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, min_bandwidth_burst_rate=True)
            self.assertEqual(sched_attr['min_bandwidth_burst_rate'], 200)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMaxBwidthBurstRateGroupAttachTest(self):
        '''
        This verifies creation of scheduler with max bandwidth burst rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMaxBwidthBurstRateGroupAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_burst_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_burst_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_burst_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            schgroup_attr = sai_thrift_get_scheduler_group_attribute(
                self.client, self.schgroup, scheduler_profile_id=True)
            self.assertEqual(schgroup_attr['scheduler_profile_id'], sched)

            status = sai_thrift_set_scheduler_attribute(
                self.client, sched, max_bandwidth_burst_rate=2000)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client, sched, max_bandwidth_burst_rate=True)
            self.assertEqual(sched_attr['max_bandwidth_burst_rate'], 2000)

        finally:
            sai_thrift_set_scheduler_group_attribute(self.client,
                                                     self.schgroup,
                                                     scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)


class SchedulerPortAttachTest(SaiHelper):
    '''
    These verify possibilities of attaching scheduler with different params set
    to a port
    '''

    def runTest(self):
        try:
            self.schedulerMaxBwidthRatePortAttachTest()
            self.schedulerMaxBwidthBurstRatePortAttachTest()
        finally:
            pass

    def schedulerMaxBwidthRatePortAttachTest(self):
        '''
        This verifies creation of scheduler with max bandwidth rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMaxBwidthRatePortAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_port_attribute(
                self.client, self.port1, qos_scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            port_attr = sai_thrift_get_port_attribute(
                self.client, self.port1, qos_scheduler_profile_id=True)
            self.assertEqual(port_attr['qos_scheduler_profile_id'], sched)

        finally:
            sai_thrift_set_port_attribute(self.client,
                                          self.port1,
                                          qos_scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)

    def schedulerMaxBwidthBurstRatePortAttachTest(self):
        '''
        This verifies creation of scheduler with max bandwidth burst rate set
        and attaching it to a scheduler group
        '''
        print("\nschedulerMaxBwidthBurstRatePortAttachTest()")

        try:
            sched = sai_thrift_create_scheduler(
                self.client,
                meter_type=SAI_METER_TYPE_PACKETS,
                max_bandwidth_burst_rate=1000,
                scheduling_type=SAI_SCHEDULING_TYPE_STRICT)
            self.assertTrue(sched != 0)

            sched_attr = sai_thrift_get_scheduler_attribute(
                self.client,
                sched,
                meter_type=True,
                max_bandwidth_burst_rate=True,
                scheduling_type=True)
            self.assertEqual(sched_attr['meter_type'], SAI_METER_TYPE_PACKETS)
            self.assertEqual(sched_attr['max_bandwidth_burst_rate'], 1000)
            self.assertEqual(sched_attr['scheduling_type'],
                             SAI_SCHEDULING_TYPE_STRICT)

            status = sai_thrift_set_port_attribute(
                self.client, self.port1, qos_scheduler_profile_id=sched)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            port_attr = sai_thrift_get_port_attribute(
                self.client, self.port1, qos_scheduler_profile_id=True)
            self.assertEqual(port_attr['qos_scheduler_profile_id'], sched)

        finally:
            sai_thrift_set_port_attribute(self.client,
                                          self.port1,
                                          qos_scheduler_profile_id=0)

            sai_thrift_remove_scheduler(self.client, sched)
