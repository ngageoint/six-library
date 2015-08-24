#!/usr/bin/env python

#
# =========================================================================
# This file is part of cphd-python
# =========================================================================
#
# (C) Copyright 2004 - 2015, MDA Information Systems LLC
#
# cphd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; If not,
# see <http://www.gnu.org/licenses/>.
#

# In general, if functionality in CPHD is borrowed from six.sicd,
# refer to six.sicd's test script for more verification

# The numbers used here don't correspond to anything at all

from pysix import cphd
import pysix.six_base as six
from pysix import six_sicd
from pysix import scene
from coda import coda_types
from coda import math_poly
from coda import math_linear


def notEqualVector3(lhs, rhs):
    return lhs[0] != rhs[0] or lhs[1] != rhs[1] or lhs[2] != rhs[2]


vecParam = cphd.VectorParameters()

domainType = cphd.DomainType()
domainType.value = cphd.DomainType.TOA

numVec = coda_types.VectorSizeT()
numVec.push_back(3)
numVec.push_back(3)
numVec.push_back(3)
vbm = cphd.VBM(3,
        numVec,
        True,
        False,
        True,
        domainType 
        )

if vbm.haveTropoSRP():
    print 'VBM TropoSRP selection not working'

if not vbm.haveAmpSF():
    print 'VBM AmpSF selection not workig'

if not vbm.haveSRPTime():
    print 'VBM SRPTime selection not working'

assignNum = 99
vbm.setTxTime(assignNum,1,1)
if vbm.getTxTime(1,1) != assignNum:
    print 'VBM txtime assign failed'


assignVec = math_linear.Vector3()
assignVec[0] = 1
assignVec[1] = 2
assignVec[2] = 3
vbm.setTxPos(assignVec,2,2)
if notEqualVector3(assignVec, vbm.getTxPos(2,2)):
    print 'VBM txpos assign failed'

vbm.setRcvTime(assignNum, 0, 1)
if vbm.getRcvTime(0,1) != assignNum:
    print 'VBM rcvtime assign failed'

vbm.setRcvPos(assignVec,0,0)
if notEqualVector3(vbm.getRcvPos(0,0), assignVec):
    print 'VBM rcvpos assign failed'

vbm.setSRPTime(assignNum, 1, 0)
if vbm.getSRPTime(1,0) != assignNum:
    print 'VBM srptime assign failed'

vbm.setSRPPos(assignVec, 2, 0)
if notEqualVector3(vbm.getSRPPos(2,0), assignVec):
    print 'VBM srppos assign failed'

vbm.setDeltaTOA0(assignNum, 1,2)
if vbm.getDeltaTOA0(1,2) != assignNum:
    print 'VBM deltaTOA0 assign failed'

vbm.setTOASS(assignNum, 1,1)
if vbm.getTOASS(1,1) != assignNum:
    print 'VBM TOASS assign failed'

vbm.updateVectorParameters(vecParam)

# If you want to be sure things are getting assigned,
# just uncomment all these

# These show that our vecParam was updated using our optional
# field selection (tropoSRP size will be undefined, tropoSRP offset
# will be negative 1)
#print vecParam.txTime
#print vecParam.txPos
#print vecParam.rcvTime
#print vecParam.rcvPos
#print vecParam.srpTime
#print vecParam.srpPos
#print vecParam.tropoSRP
#print vecParam.ampSF
#print vecParam.toaParameters.deltaTOA0
#print vecParam.toaParameters.toaSS

#print vecParam.txTimeOffset()
#print vecParam.txPosOffset()
#print vecParam.rcvTimeOffset()
#print vecParam.rcvPosOffset()
#print vecParam.srpTimeOffset()
#print vecParam.srpPosOffset()
#print vecParam.tropoSRPOffset()
#print vecParam.ampSFOffset()
#print vecParam.deltaTOA0Offset()
#print vecParam.toaSSOffset()

# These print out the three channel buffers that we assigned to earlier
#print vbm.toBuffer(0)
#print vbm.toBuffer(1)
#print vbm.toBuffer(2)
