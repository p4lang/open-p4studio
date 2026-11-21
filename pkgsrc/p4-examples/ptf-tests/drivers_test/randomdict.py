# Source https://github.com/robtandy/randomdict
# Modifications
#  - Modified the self.keys to self._keys and self.values to self._values
#    so that keys(), values() and items() methods can be used
#  - using an OrderedDict() instead of dictionary to store the _keys
#  - Bug fix in __setitem__ for the key already present in map
#  - Added __str__ and __repr__ methods
#  - Added random_key_samples() method

from collections.abc import MutableMapping
from collections import OrderedDict
import random

__version__ = '0.2.0'

class RandomDict(MutableMapping):
    def __init__(self, *args, **kwargs):
        """ Create RandomDict object with contents specified by arguments.
        Any argument
        :param *args:       dictionaries whose contents get added to this dict
        :param **kwargs:    key, value pairs will be added to this dict
        """
        # mapping of _keys to array positions
        self._keys = OrderedDict()
        self._values = []
        self.last_index = -1

        self.update(*args, **kwargs)

    def __setitem__(self, key, val):
        if key in self._keys:
            i = self._keys[key]
            self._values[i] = (key, val)
        else:
            self.last_index += 1
            i = self.last_index
            self._values.append((key, val))

        self._keys[key] = i

    def __delitem__(self, key):
        if not key in self._keys:
            raise KeyError

        # index of item to delete is i
        i = self._keys[key]
        # last item in _values array is
        move_key, move_val = self._values.pop()

        if i != self.last_index:
            # we move the last item into its location
            self._values[i] = (move_key, move_val)
            self._keys[move_key] = i
        # else it was the last item and we just throw
        # it away

        # shorten array of _values
        self.last_index -= 1
        # remove deleted key
        del self._keys[key]

    def __getitem__(self, key):
        if not key in self._keys:
            raise KeyError

        i = self._keys[key]
        return self._values[i][1]

    def __iter__(self):
        return iter(self._keys)

    def __len__(self):
        return self.last_index + 1

    def random_key(self):
        """ Return a random key from this dictionary in O(1) time """
        if len(self) == 0:
            raise KeyError("RandomDict is empty")

        i = random.randint(0, self.last_index)
        return self._values[i][0]

    def random_value(self):
        """ Return a random value from this dictionary in O(1) time """
        return self[self.random_key()]

    def random_item(self):
        """ Return a random key-value pair from this dictionary in O(1) time """
        k = self.random_key()
        return k, self[k]

    def random_key_samples(self, count):
        """ Return a list of random keys from this dictionary """
        if count == 1:
            return [self.random_key()]

        if len(self) == 0:
            raise KeyError("RandomDict is empty")

        if (self.last_index+1) < count:
            raise KeyError("Sample size is greater than contents")

        samples = [self._values[i][0] for i in random.sample(range(self.last_index + 1), count)]
        return samples

    def __repr__(self):
        if not self:
            return '%s()' % (self.__class__.__name__,)
        return '%s(%r)' % (self.__class__.__name__, list(self.items()))

    def __str__(self):
        if not self:
            return '%s()' % (self.__class__.__name__,)
        return '%s(%r)' % (self.__class__.__name__, list(self.items()))
