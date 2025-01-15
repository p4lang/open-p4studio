import nose2.tools
import subprocess
import unittest


class BaseTestCase(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def setUp(self):
        super().setUp()

    def run_ptf(self, args=[], input=None):
        # redirect stderr to stdout so we can get error messages
        r = subprocess.run(
            ["./ptf"] + args,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            input=input,
            universal_newlines=True,
        )
        return r.returncode, r.stdout

    def tearDown(self):
        super().tearDown()


class TestParamsTestCase(BaseTestCase):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.testdir = "utests/specs"

    def parse_params(self, out):
        params = {}
        for line in out.splitlines():
            if not line.startswith(">>>"):
                continue
            line = line[3:]
            if line == "None":
                return None
            k, v = line.split("=")
            params[k] = int(v)
        return params

    def do_test_params(self, testspec, test_params_str):
        rc, out = self.run_ptf(
            args=[
                "--test-dir",
                self.testdir,
                "--test-params={}".format(test_params_str),
                "--platform",
                "dummy",
                testspec,
            ]
        )
        self.assertEqual(rc, 0)
        return out

    @nose2.tools.params(
        ("k1=9;k2=18", {"k1": 9, "k2": 18}), ("k1=9;k2=18;", {"k1": 9, "k2": 18})
    )
    def test_test_params_get(self, test_params_str, expected_params):
        testspec = "test.TestParamsGet"
        out = self.do_test_params(testspec, test_params_str)
        params = self.parse_params(out)
        self.assertEqual(params, expected_params)

    @nose2.tools.params(("bad", None))
    def test_test_params_get_error(self, test_params_str, expected_params):
        testspec = "test.TestParamsGet"
        out = self.do_test_params(testspec, test_params_str)
        self.assertIn("Error when parsing test params", out)

    @nose2.tools.params(("k1=9;k2=18", 9), ("k1=9;k2=18;", 9), ("k2=18", -1))
    def test_test_param_get(self, test_params_str, expected_value):
        testspec = "test.TestParamGet"
        out = self.do_test_params(testspec, test_params_str)
        params = self.parse_params(out)
        self.assertEqual(params["k1"], expected_value)

    @nose2.tools.params(("bad", None))
    def test_test_param_get_error(self, test_params_str, expected_params):
        testspec = "test.TestParamGet"
        out = self.do_test_params(testspec, test_params_str)
        self.assertIn("Error when parsing test params", out)
