import ptf
from ptf.base_tests import BaseTest
from ptf import testutils


class TestParamsGet(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)

    def runTest(self):
        params = testutils.test_params_get(default=None)
        if params is None:
            print(">>>None")
        else:
            for k, v in params.items():
                print(">>>{}={}".format(k, v))


class TestParamGet(BaseTest):
    def setUp(self):
        BaseTest.setUp(self)

    def runTest(self):
        v = testutils.test_param_get("k1", default=-1)
        if v is None:
            print(">>>None")
        else:
            print(">>>k1={}".format(v))
