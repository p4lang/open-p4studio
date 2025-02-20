# vi: ft=python
set print static-members off
set print object
set unwindonsignal on
set unwind-on-terminating-exception on
set python print-stack full

python
import sys
import re

def template_split(s):
    parts = []
    bracket_level = 0
    paren_level = 0
    current = []
    for c in (s):
        if c == "," and bracket_level == 1 and paren_level == 0:
            parts.append("".join(current))
            current = []
        else:
            if c == '>':
                bracket_level -= 1
            if bracket_level > 0:
                current.append(c)
            if c == '<':
                bracket_level += 1
            if c == '(':
                paren_level += 1
            if c == ')':
                paren_level -= 1
    parts.append("".join(current))
    return parts

TYPE_CACHE = {}

def lookup_type(typename):
    """Return a gdb.Type object represents given `typename`.
    For example, x.cast(ty('Buffer'))"""
    if typename in TYPE_CACHE:
        return TYPE_CACHE[typename]

    m0 = re.match(r"^(.*\S)\s*const$", typename)
    m1 = re.match(r"^(.*\S)\s*[*|&]$", typename)
    if m0 is not None:
        tp = lookup_type(m0.group(1))
    elif m1 is None:
        tp = gdb.lookup_type(typename)
    else:
        if m1.group(0).endswith('*'):
            tp = lookup_type(m1.group(1)).pointer()
        else:
            tp = lookup_type(m1.group(1)).reference()
    TYPE_CACHE[typename] = tp
    return tp

class stdarrayPrinter(object):
    "Print a std::array object (missing from the normal std pretty printers)"
    def __init__(self, val):
        self.val = val
        self.args = template_split(val.type.tag)
    def to_string(self):
        return "std::array<%s, %d>" % (self.args[0], int(self.args[1]))
    #def display_hint(self):
    #    return 'array'
    class _iter:
        def __init__(self, data, size):
            self.data = data
            self.size = size
            self.counter = -1
        def __iter__(self):
            return self
        def __next__(self):
            self.counter += 1
            if self.counter >= self.size:
                raise StopIteration
            item = self.data[self.counter]
            return ("[%d]" % self.counter, item)
        def next(self): return self.__next__()
    def children(self):
        return self._iter(self.val["_M_elems"], int(self.args[1]))

class BitFieldPrinter(object):
    "Print a BitField object"
    def __init__(self, val):
        self.val = val
        self.args = template_split(val.type.tag)
    def to_string(self):
        data = self.val['value_']['_M_elems']
        val = 0
        shift = 0
        idx = 0
        bits = int(self.args[0])
        while bits > 0:
            val = val + data[idx] << shift
            shift += 64
            bits -= 64
            idx += 1
        rv = "0x%x" % val
        return rv;

def stf_pp(val):
    if str(val.type.tag).startswith('mimic::utility::BitField<'):
        return BitFieldPrinter(val)
    if str(val.type.tag).startswith('std::array<'):
        return stdarrayPrinter(val)
    # print("no pretty printer for %s\n", str(val.type.tag))
    return None

try:
    found = False
    for i in range(len(gdb.pretty_printers)):
        try:
            if gdb.pretty_printers[i].__name__ == "stf_pp":
                gdb.pretty_printers[i] = stf_pp
                found = True
        except:
            pass
    if not found:
        gdb.pretty_printers.append(stf_pp)
except:
    pass

end
