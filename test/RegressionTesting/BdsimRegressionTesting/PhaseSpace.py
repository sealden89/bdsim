import numpy as _np
from pymadx import Ptc

allowedPhaseSpaceVars = ['x', 'px', 'y', 'py', 't', 'pt']


class PhaseSpace(dict):
    """ Class for representing the phase space co-ordinates of a test beam.
        Co-ordinates can be entered upon instantiation or via setter functions.
        Data types for the co-ordinates can be numerical (float, int, etc.),
        string, arrays, lists, and tuples.

        The writer function will write the beam to a madx file to be called by
        a gmad ptc beam distribution. Upon writing, all permutations of particle
        phase space co-ordinates will be written.
        """
    def __init__(self, x=0, px=0, y=0, py=0, t=0, pt=0):
        dict.__init__(self)
        for param in allowedPhaseSpaceVars:
            self[param] = []
        
        self._checkInput('x', x)
        self._checkInput('px', px)
        self._checkInput('y', y)
        self._checkInput('py', py)
        self._checkInput('t', t)
        self._checkInput('pt', pt)

    def _checkInput(self, variable, value):
        if allowedPhaseSpaceVars.__contains__(variable):
            variableValues = []
            if isinstance(value, _np.ndarray) or isinstance(value, list) or isinstance(value, tuple):
                for val in value:
                    try:
                        floatval = _np.float(val)
                        variableValues.append(floatval)
                    except ValueError:
                        print ("Unknown data type for " + variable + ".")
            else:
                try:
                    val = _np.float(value)
                except ValueError:
                    raise TypeError("Unknown data type for " + variable + ".")
                variableValues.append(val)
            self[variable] = variableValues
        else:
            raise ValueError("Unknown beam phase space parameter.")

    def __repr__(self):
        s = 'pybdsimTest.bdsimTesting.PhaseSpace instance.\r\n'
        s += 'Phase space test range:\r\n'
        for param in self.keys():
            s += '  ' + param + ': ' + self[param].__repr__() + '\r\n'
        return s

    def SetX(self, value):
        self._checkInput('x', value)

    def SetPX(self, value):
        self._checkInput('px', value)

    def SetY(self, value):
        self._checkInput('y', value)

    def SetPY(self, value):
        self._checkInput('py', value)

    def SetT(self, value):
        self._checkInput('t', value)

    def SetPT(self, value):
        self._checkInput('pt', value)

    def _WriteToInrays(self, filename):
        """ Function to write the beam to a madx file to be called by
            a gmad ptc beam distribution. Upon writing, all permutations of particle
            phase space co-ordinates will be written.
            """
        self.inrays = Ptc.Inrays()
        # all dimensions must have at least one value
        for key, values in self.iteritems():
            if len(values) == 0:
                self[key].append(0)

        for x in self['x']:
            for px in self['px']:
                for y in self['y']:
                    for py in self['py']:
                        for t in self['t']:
                            for pt in self['pt']:
                                self.inrays.AddParticle(x, px, y, py, t, pt)

        self.inrays.Write(filename)
