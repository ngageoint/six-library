#
# =========================================================================
# This file is part of cphd-python
# =========================================================================
#
# (C) Copyright 2004 - 2020, MDA Information Systems LLC
#
# cphd-python is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; If not,
# see <http://www.gnu.org/licenses/>.
#

# In general, if functionality in CPHD is borrowed from six.sicd,
# refer to six.sicd's test script for more verification

import itertools

import numpy as np

from pysix import cphd
import pysix.six_base as six
from pysix import scene
from coda.math_poly import Poly1D, Poly2D, PolyVector3
from coda.math_linear import Vector2, Vector3

def get_poly_2d(coeffs):
    num_x = len(coeffs)
    num_y = len(coeffs[0])
    poly = Poly2D(num_x - 1, num_y - 1)
    for ii, jj in itertools.product(range(num_x), range(num_y)):
        poly[ii, jj] = coeffs[ii][jj]
    return poly


def get_poly_vector(coeffs):
    num_x = len(coeffs)
    num_y = len(coeffs[0])
    order = num_x - 1
    assert num_y == 3
    poly = PolyVector3(order)
    for ii in range(num_x):
        poly[ii] = Vector3(coeffs[ii])
    return poly


def get_boolean(b):
    return six.BooleanType.IS_TRUE if b else six.BooleanType.IS_FALSE


def get_parameter(name, value):
    p = six.Parameter()
    p.setName(name)
    p.setValue(value)
    return p


IMAGE_AREA = cphd.AreaType()
IMAGE_AREA.x1y1 = Vector2([3.5, 5.3])
IMAGE_AREA.x2y2 = Vector2([5.3, 3.5])
IMAGE_AREA.polygon.append(Vector2([0.1, 0.3]))
IMAGE_AREA.polygon.append(Vector2([0.4, 0.2]))
IMAGE_AREA.polygon.append(Vector2([0.5, 0.9]))


def get_test_metadata(has_support_array, is_compressed):
    # Returns a cphd.Metadata object populated with dummy values

    metadata = cphd.Metadata()

    # CollectionID block

    cid = six.CollectionInformation()
    cid.collectorName = 'Collector'
    cid.corename = 'Core'
    cid.collectType.value = six.CollectType.MONOSTATIC
    cid.radarMode = six.RadarModeType('STRIPMAP')
    cid.setClassificationLevel('UNCLASSIFIED')
    cid.releaseInfo = 'UNSPECIFIED'
    cid.countryCodes.push_back('US')
    cid.countryCodes.push_back('GB')
    cid.countryCodes.push_back('AZ')
    cid.parameters.push_back(get_parameter('param1', 'val'))

    metadata.collectionID = cid

    # Global Block

    glob = cphd.Global()
    glob.domainType.value = cphd.DomainType.FX
    glob.sgn.value = cphd.PhaseSGN.MINUS_1
    glob.refFrequencyIndex = 1

    timeline = cphd.Timeline()
    timeline.collectStart = six.DateTime()
    timeline.collectionDuration = 200
    timeline.txTime1 = 4
    timeline.txTime2 = 204
    glob.timeline = timeline

    fx_band = cphd.FxBand()
    fx_band.fxMin = 9e-1
    fx_band.fxMax = 1.7e0
    glob.fxBand = fx_band

    toa_swath = cphd.TOASwath()
    toa_swath.toaMin = 3.4e0
    toa_swath.toaMax = 6.1e0
    glob.toaSwath = toa_swath

    glob.tropoParameters = cphd.makeScopedCopyableTropoParameters()
    glob.tropoParameters.n0 = 6.52e1
    glob.tropoParameters.refHeight.value = cphd.RefHeight.IARP

    glob.ionoParameters = cphd.makeScopedCopyableIonoParameters()
    glob.ionoParameters.tecv = 5.8e0
    glob.ionoParameters.f2Height = 3.0e0

    metadata._global = glob

    # SceneCoordinates block

    sc = cphd.SceneCoordinates()
    sc.earthModel.value = cphd.EarthModelType.WGS_84

    sc.iarp = cphd.IARP()
    sc.iarp.ecf = Vector3([1.2, 2.3, 3.4])
    sc.iarp.llh = scene.LatLonAlt(45, -102, 34)

    sc.referenceSurface.hae = cphd.makeScopedCopyableHAE()
    sc.referenceSurface.hae.uIax = scene.LatLon(12, 24)
    sc.referenceSurface.hae.uIay = scene.LatLon(36, 48)

    sc.imageArea = IMAGE_AREA

    sc.imageAreaCorners = six.LatLonCorners()
    sc.imageAreaCorners.upperLeft = scene.LatLon(10, 11)
    sc.imageAreaCorners.upperRight = scene.LatLon(20, 21)
    sc.imageAreaCorners.lowerRight = scene.LatLon(30, 31)
    sc.imageAreaCorners.lowerLeft = scene.LatLon(40, 41)

    sc.imageGrid = cphd.makeScopedCopyableImageGrid()
    sc.imageGrid.identifier = 'Grid'
    sc.imageGrid.iarpLocation.line = 1.23
    sc.imageGrid.iarpLocation.sample = 3.45
    sc.imageGrid.xExtent.lineSpacing = 0.123
    sc.imageGrid.xExtent.firstLine = 0
    sc.imageGrid.xExtent.numLines = 2
    sc.imageGrid.yExtent.sampleSpacing = 3.456
    sc.imageGrid.yExtent.firstSample = 0
    sc.imageGrid.yExtent.numSamples = 3

    segments = {
        'Segment1': {
            'start': (0, 1),
            'end': (2, 3),
            'poly': [
                [0.4, 0.6],
                [0.8, 1.2],
                [1.2, 1.8],
            ],
        },
        'Segment2': {
            'start': (1, 2),
            'end': (3, 4),
            'poly': [
                [0.4, 0.6],
                [0.8, 1.2],
                [1.2, 1.8],
            ],
        },
    }

    for name, seg_data in segments.items():
        seg = cphd.Segment()
        seg.identifier = name
        seg.startLine = seg_data['start'][0]
        seg.startSample = seg_data['start'][1]
        seg.endLine = seg_data['end'][0]
        seg.endSample = seg_data['end'][1]
        for p in seg_data['poly']:
            ls = cphd.LineSample()
            ls.line = p[0]
            ls.sample = p[1]
            seg.polygon.append(ls)
        sc.imageGrid.segments.append(seg)

    metadata.sceneCoordinates = sc

    # Data block

    _channels = [
        {
            'id': 'Channel',
            'shape': (2, 2),
            'byte_offset': 0,
            'pvp_byte_offset': 0,
            'comp_sig_size': 8,
        },
        {
            'id': 'Channel',
            'shape': (3, 4),
            'byte_offset': 0,
            'pvp_byte_offset': 0,
            'comp_sig_size': 8,
        },
    ]

    if has_support_array:
        # Note that ordering of support arrays is not neccessarily defined
        # when reading in metadata from an XML string. If there are multiple
        # support arrays, they may not be in the same order, which will make
        # the XML strings not compare equally
        shape = (3, 4)
        _support_arrays = [
            {
                'id': 'AddedSupportArray',
                'shape': shape,
                'bytes_per_element': 8,
                'offset': 144,
            }
        ]
        _support_arrays_data = [np.random.randint(256, size=shape, dtype=np.uint8)]
    else:  # In case we don't want to provide test support data
        _support_arrays, _support_arrays_data = [], []

    data = cphd.Data()
    data.signalArrayFormat.value = cphd.SignalArrayFormat.CF8
    data.numCPHDChannels = len(_channels)
    data.numBytesPVP = 376

    # If signalCompressionID is set, then it thinks the data is compressed
    if is_compressed:
        data.signalCompressionID = 'Compress'

    for vals in _channels:
        _chan = cphd.DataChannel(*vals['shape'])
        _chan.identifier = vals['id']
        _chan.signalArrayByteOffset = vals['byte_offset']
        _chan.pvpArrayByteOffset = vals['pvp_byte_offset']
        _chan.compressedSignalSize = vals['comp_sig_size']
        data.channels.append(_chan)

    for vals in _support_arrays:
        data.setSupportArray(
            vals['id'],
            *vals['shape'],
            vals['bytes_per_element'],
            vals['offset'],
        )

    metadata.data = data

    # Channel block

    channel = cphd.Channel()
    channel.refChId = 'ChId'
    channel.fxFixedCphd.value = six.BooleanType.IS_TRUE
    channel.toaFixedCphd.value = six.BooleanType.IS_FALSE
    channel.srpFixedCphd.value = six.BooleanType.IS_TRUE

    params = cphd.ChannelParameter()
    params.identifier = 'CPI'
    params.refVectorIndex = 1
    params.fxFixed.value = six.BooleanType.IS_FALSE
    params.toaFixed.value = six.BooleanType.IS_TRUE
    params.srpFixed.value = six.BooleanType.IS_TRUE
    params.signalNormal.value = six.BooleanType.IS_FALSE
    params.polarization.txPol.value = cphd.PolarizationType.X
    params.polarization.rcvPol.value = cphd.PolarizationType.RHC
    params.fxC = 1.3
    params.fxBW = 0.8
    params.fxBWNoise = 0.5
    params.toaSaved = 2.7

    params.dwellTimes.codId = 'CODPolynomial'
    params.dwellTimes.dwellId = 'DwellPolynomial'

    params.imageArea = IMAGE_AREA

    params.toaExtended = cphd.makeScopedCopyableTOAExtended()
    params.toaExtended.toaExtSaved = 1.0
    params.toaExtended.lfmEclipse = cphd.makeScopedCopyableTOALFMEclipse()
    params.toaExtended.lfmEclipse.fxEarlyLow = 1.0
    params.toaExtended.lfmEclipse.fxEarlyHigh = 2.0
    params.toaExtended.lfmEclipse.fxLateLow = 1.0
    params.toaExtended.lfmEclipse.fxLateHigh = 2.0

    params.antenna = cphd.makeScopedCopyableChannelAntenna()
    params.antenna.txAPCId = 'TxAPCId'
    params.antenna.txAPATId = 'TxAPATId'
    params.antenna.rcvAPCId = 'RcvAPCId'
    params.antenna.rcvAPATId = 'RcvAPATId'

    params.txRcv = cphd.makeScopedCopyableChannelTxRcv()
    params.txRcv.txWFId.clear()
    params.txRcv.txWFId.push_back('TxWFId')
    params.txRcv.rcvId.clear()
    params.txRcv.rcvId.push_back('RcvId')

    params.tgtRefLevel = cphd.makeScopedCopyableTgtRefLevel()
    params.tgtRefLevel.ptRef = 12.0

    noise_points = [
        (0.3, 2.7),
        (0.5, 2.7),
    ]

    params.noiseLevel = cphd.makeScopedCopyableNoiseLevel()
    params.noiseLevel.pnRef = 0.5
    params.noiseLevel.bnRef = 0.8
    params.noiseLevel.fxNoiseProfile = cphd.makeScopedCopyableFxNoiseProfile()
    for p in noise_points:
        pt = cphd.Point()
        pt.fx = p[0]
        pt.pn = p[1]
        params.noiseLevel.fxNoiseProfile.point.append(pt)

    channel.parameters.append(params)

    added_params = [
        ('AddedParameter1', 'Param'),
        ('AddedParameter2', 'Param'),
    ]

    for args in added_params:
        channel.addedParameters.push_back(get_parameter(*args))

    metadata.channel = channel

    # PVP block
    # Note that the params used here correspond to the list-of-dicts PVP
    # data in get_test_pvp_data() below. Any changes that affect the total PVP
    # size will require updating data.numBytesPVP (above)

    pvp_data = {
        # Name: (offset, size, format)
        'txTime': (0, 1, 'F8'),
        'txPos': (1, 3, 'F8'),
        'txVel': (4, 3, 'F8'),
        'rcvTime': (7, 1, 'F8'),
        'rcvPos': (8, 3, 'F8'),
        'rcvVel': (11, 3, 'F8'),
        'srpPos': (14, 3, 'F8'),
        'aFDOP': (17, 1, 'F8'),
        'aFRR1': (18, 1, 'F8'),
        'aFRR2': (19, 1, 'F8'),
        'fx1': (20, 1, 'F8'),
        'fx2': (21, 1, 'F8'),
        'toa1': (22, 1, 'F8'),
        'toa2': (23, 1, 'F8'),
        'tdTropoSRP': (24, 1, 'F8'),
        'sc0': (25, 1, 'F8'),
        'scss': (26, 1, 'F8'),
        # Optional params
        'ampSF': (27, 1, 'F8'),
        'fxN1': (28, 1, 'F8'),
        'fxN2': (29, 1, 'F8'),
        'signal': (30, 1, 'I8'),
        'tdIonoSRP': (31, 1, 'F8'),
        'toaE1': (32, 1, 'F8'),
        'toaE2': (33, 1, 'F8'),
    }

    added_pvp_data = [
        # size, offset, fmt, name
        (1, 34, 'F4', 'customF4Param'),
        (1, 35, 'F8', 'customF8Param'),
        (1, 36, 'U1', 'customU1Param'),
        (1, 37, 'U2', 'customU2Param'),
        (1, 38, 'U4', 'customU4Param'),
        (1, 39, 'U8', 'customU8Param'),
        (1, 40, 'I1', 'customI1Param'),
        (1, 41, 'I2', 'customI2Param'),
        (1, 42, 'I4', 'customI4Param'),
        (1, 43, 'I8', 'customI8Param'),
        # (1, 44, 'CI2', 'customCI2Param'),
        # (1, 45, 'CI4', 'customCI4Param'),
        # (1, 46, 'CI8', 'customCI8Param'),
        # (1, 47, 'CI16', 'customCI16Param'),
        (1, 44, 'CF8', 'customCF8Param'),
        (2, 45, 'CF16', 'customCF16Param')
    ]

    pvp = cphd.Pvp()
    for name, (offset, size, fmt) in pvp_data.items():
        attr = pvp.__getattr__(name)
        attr.setOffset(offset)
        attr.setSize(size)
        if size == 3:
            fmt = 'X={};Y={};Z={};'.format(fmt, fmt, fmt)
        attr.setFormat(fmt)

    for args in added_pvp_data:
        # Note that this is (size, offset, format, name)
        pvp.setCustomParameter(*args)

    metadata.pvp = pvp

    # SupportArray block

    sa = cphd.makeScopedCopyableSupportArray()

    iaz = cphd.SupportArrayParameter()
    iaz.elementFormat = 'IAZ=F4;'
    iaz.x0 = 0.0
    iaz.y0 = 0.0
    iaz.xSS = 5.0
    iaz.ySS = 5.0
    sa.iazArray.append(iaz)

    gainPhase = cphd.SupportArrayParameter()
    gainPhase.elementFormat = 'Gain=F4;Phase=F4;'
    gainPhase.x0 = 0.0
    gainPhase.y0 = 0.0
    gainPhase.xSS = 5.0
    gainPhase.ySS = 5.0
    sa.antGainPhase.append(gainPhase)

    added_sa = cphd.AdditionalSupportArray()
    added_sa.identifier = 'AddedSupportArray'
    added_sa.elementFormat = 'F4'
    added_sa.x0 = 0.0
    added_sa.y0 = 0.0
    added_sa.xSS = 5.0
    added_sa.ySS = 5.0
    added_sa.xUnits = 'XUnits'
    added_sa.yUnits = 'YUnits'
    added_sa.zUnits = 'ZUnits'
    for args in [('Parameter1', 'Additional parameter'),
                 ('Parameter1', 'Additional parameter')]:
        added_sa.parameter.push_back(get_parameter(*args))

    metadata.supportArray = sa

    # Dwell block

    dwell = cphd.Dwell()

    cod_coeffs = [
        [0.0, 5.0],
        [5.0, 0.0],
    ]

    dwell_time_coeffs = [
        [0.0, 2.0],
        [3.0, 0.0],
    ]

    cod = cphd.COD()
    cod.identifier = 'codPolynomial1'
    cod.codTimePoly = get_poly_2d(cod_coeffs)
    dwell.cod.append(cod)

    dtime = cphd.DwellTime()
    dtime.identifier = 'dwellPolynomial1'
    dtime.dwellTimePoly = get_poly_2d(dwell_time_coeffs)
    dwell.dtime.append(dtime)

    metadata.dwell = dwell

    # ReferenceGeometry block

    geom = cphd.ReferenceGeometry()
    geom.referenceTime = 0.0
    geom.srpCODTime = 23.0
    geom.srpDwellTime = 25.0
    geom.srp.ecf = Vector3([1.0, 2.0, 3.5])
    geom.srp.iac = Vector3([1.5, 2.5, 4.0])

    geom.monostatic = cphd.makeScopedCopyableMonostatic()
    geom.monostatic.arpPos = Vector3([10.0, 10.0, 10.0])
    geom.monostatic.arpVel = Vector3([10.0, 10.0, 10.0])
    geom.monostatic.sideOfTrack.value = six.SideOfTrackType.LEFT
    geom.monostatic.slantRange = 20.0
    geom.monostatic.groundRange = 20.0
    geom.monostatic.dopplerConeAngle = 30.0
    geom.monostatic.grazeAngle = 30.0
    geom.monostatic.incidenceAngle = 30.0
    geom.monostatic.azimuthAngle = 30.0
    geom.monostatic.twistAngle = 30.0
    geom.monostatic.slopeAngle = 30.0
    geom.monostatic.layoverAngle = 30.0

    metadata.referenceGeometry = geom

    # Antenna block

    ant = cphd.makeScopedCopyableCphdAntenna()

    coord_frames = {
        'ACF1': {
            'xaxis': [
                [1.0, 1.0, 1.0],
                [2.0, 2.0, 0.0],
                [3.0, 3.0, 3.0],
                [4.0, 4.0, 0.0],
            ],
            'yaxis': [
                [1.0, 1.0, 1.0],
                [2.0, 2.0, 2.0],
            ],
        },
        'ACF2': {
            'xaxis': [
                [1.0, 1.0, 1.0],
                [2.0, 2.0, 2.0],
            ],
            'yaxis': [
                [1.0, 1.0, 1.0],
                [2.0, 2.0, 2.0],
            ],
        },
    }

    for name, cf_data in coord_frames.items():
        cf = cphd.AntCoordFrame()
        cf.identifier = name
        cf.xAxisPoly = get_poly_vector(cf_data['xaxis'])
        cf.yAxisPoly = get_poly_vector(cf_data['yaxis'])
        ant.antCoordFrame.append(cf)

    phase_centers = [
        {
            'identifier': 'APC',
            'acf_id': 'ACF1',
            'xyz': [5.0, 5.0, 5.0],
        },
    ]

    for pc_data in phase_centers:
        pc = cphd.AntPhaseCenter()
        pc.identifier = pc_data['identifier']
        pc.acfId = pc_data['acf_id']
        pc.apcXYZ = Vector3(pc_data['xyz'])
        ant.antPhaseCenter.append(pc)

    ant_patterns = {
        'APAT': {
            'f0': 2.3,
            'g0': 2.3,
            'eb_freq_shift': True,
            'ml_freq_dilation': False,
            'gain_bs': [1.0, 2.0],
            'dcx': [5.0, 0.0],
            'dcy': [0.0, 5.0],
            'gain_array': [
                [0.0, 1.0],
                [1.0, 5.0],
            ],
            'phase_array': [
                [0.0, 1.0],
                [1.0, 5.0],
            ],
            'gain_element': [
                [0.0, 1.0],
                [1.0, 5.0],
            ],
            'phase_element': [
                [0.0, 1.0],
                [1.0, 5.0],
            ],
            'gain_phase_array': [
                {
                    'freq': 2.3,
                    'array_id': 'Parameter1',
                    'element_id': 'Parameter2',
                },
                {
                    'freq': 2.8,
                    'array_id': 'Parameter1',
                    'element_id': 'Parameter2',
                },
            ],
        },
    }

    for name, ap_data in ant_patterns.items():
        ap = cphd.AntPattern()
        ap.identifier = name
        ap.freqZero = ap_data['f0']
        ap.gainZero = ap_data['g0']
        ap.ebFreqShift.value = get_boolean(ap_data['eb_freq_shift'])
        ap.mlFreqDilation.value = get_boolean(ap_data['ml_freq_dilation'])
        ap.gainBSPoly = Poly1D(ap_data['gain_bs'])
        ap.eb.dcxPoly = Poly1D(ap_data['dcx'])
        ap.eb.dcyPoly = Poly1D(ap_data['dcy'])
        ap.array.gainPoly = get_poly_2d(ap_data['gain_array'])
        ap.array.phasePoly = get_poly_2d(ap_data['phase_array'])
        ap.element.gainPoly = get_poly_2d(ap_data['gain_element'])
        ap.element.phasePoly = get_poly_2d(ap_data['phase_element'])
        for gpa_data in ap_data['gain_phase_array']:
            gpa = cphd.GainPhaseArray()
            gpa.freq = gpa_data['freq']
            gpa.arrayId = gpa_data['array_id']
            gpa.elementId = gpa_data['element_id']
            ap.gainPhaseArray.append(gpa)
        ant.antPattern.append(ap)

    metadata.antenna = ant

    # TxRcv block

    tr = cphd.makeScopedCopyableTxRcv()

    tx_params = {
        'TxWFParams': {
            'pulseLength': 3.0,
            'rfBandwidth': 2.3,
            'freqCenter': 1.8,
            'lfmRate': 1.0,
            'power': 5.0,
            'polarization': cphd.PolarizationType.LHC,
        },
    }

    rcv_params = {
        'RcvParam1': {
            'windowLength': 3.0,
            'sampleRate': 2.3,
            'ifFilterBW': 2.3,
            'freqCenter': 1.8,
            'lfmRate': 1.0,
            'polarization': cphd.PolarizationType.LHC,
            'pathGain': 5.0,
        },
        'RcvParam2': {
            'windowLength': 3.0,
            'sampleRate': 2.3,
            'ifFilterBW': 2.3,
            'freqCenter': 1.8,
            'lfmRate': 1.0,
            'polarization': cphd.PolarizationType.LHC,
            'pathGain': 5.0,
        },
    }

    for name, params in tx_params.items():
        txp = cphd.TxWFParameters()
        txp.identifier = name
        for key, val in params.items():
            try:
                txp.__setattr__(key, val)
            except Exception:
                attr = txp.__getattr__(key)
                attr.__setattr__('value', val)
        tr.txWFParameters.append(txp)

    for name, params in rcv_params.items():
        rcvp = cphd.RcvParameters()
        rcvp.identifier = name
        for key, val in params.items():
            try:
                rcvp.__setattr__(key, val)
            except Exception:
                attr = rcvp.__getattr__(key)
                attr.__setattr__('value', val)
        tr.rcvParameters.append(rcvp)

    metadata.txRcv = tr

    # ErrorParameters block

    ep = cphd.makeScopedCopyableErrorParameters()

    ep.monostatic = cphd.makeScopedCopyableErrorMonostatic()
    ep.monostatic.posVelErr.p1 = 1.0
    ep.monostatic.posVelErr.p2 = 1.0
    ep.monostatic.posVelErr.p3 = 1.0
    ep.monostatic.posVelErr.v1 = 1.0
    ep.monostatic.posVelErr.v2 = 1.0
    ep.monostatic.posVelErr.v3 = 1.0

    ep.monostatic.posVelErr.corrCoefs = six.makeScopedCopyableCorrCoefs()
    for s1, idx1, s2, idx2 in itertools.product(['p', 'v'],
                                                [1, 2, 3],
                                                ['p', 'v'],
                                                [1, 2, 3]):
        name = '{}{}{}{}'.format(s1, idx1, s2, idx2)
        if name in dir(ep.monostatic.posVelErr.corrCoefs):
            ep.monostatic.posVelErr.corrCoefs.__setattr__(name, 0.8)

    ep.monostatic.posVelErr.positionDecorr.corrCoefZero = 0.5
    ep.monostatic.posVelErr.positionDecorr.decorrRate = 1.0

    ep.monostatic.radarSensor.rangeBias = 0.5
    ep.monostatic.radarSensor.clockFreqSF = 1.0
    ep.monostatic.radarSensor.collectionStartTime = 1.0
    ep.monostatic.radarSensor.rangeBiasDecorr = \
        cphd.makeScopedCopyableDecorrType()
    ep.monostatic.radarSensor.rangeBiasDecorr.corrCoefZero = 0.5
    ep.monostatic.radarSensor.rangeBiasDecorr.decorrRate = 1.0

    ep.monostatic.tropoError = six.makeScopedCopyableTropoError()
    ep.monostatic.tropoError.tropoRangeVertical = 5.0
    ep.monostatic.tropoError.tropoRangeSlant = 5.0
    ep.monostatic.tropoError.tropoRangeDecorr.corrCoefZero = 0.5
    ep.monostatic.tropoError.tropoRangeDecorr.decorrRate = 1.0

    ep.monostatic.ionoError = six.makeScopedCopyableIonoError()
    ep.monostatic.ionoError.ionoRangeVertical = 5.0
    ep.monostatic.ionoError.ionoRangeRateVertical = 5.0
    ep.monostatic.ionoError.ionoRgRgRateCC = 0.5
    ep.monostatic.ionoError.ionoRangeVertDecorr.corrCoefZero = 0.5
    ep.monostatic.ionoError.ionoRangeVertDecorr.decorrRate = 1.0

    cphd.errorParameters = ep

    # ProductInfo block

    pi = cphd.makeScopedCopyableProductInfo()
    pi.profile = 'Profile'

    ci = cphd.CreationInfo()
    ci.application = 'Application'
    ci.datetime = six.DateTime()
    ci.site = 'Area51'
    for args in [('Param1', 'Value1'),
                 ('Param1', 'Value2')]:
        ci.parameter.push_back(get_parameter(*args))
    pi.creationInfo.append(ci)

    for args in [('Param1', 'Value1')]:
        pi.parameter.push_back(get_parameter(*args))

    metadata.productInfo = pi

    # GeoInfo block (TODO)

    # MatchInfo block (TODO)

    return (metadata, _support_arrays_data) if has_support_array else (metadata)


def get_test_pvp_data(metadata):

    # Note that the params used here correspond to the PVP in get_test_metadata() above

    # ci2_dtype = np.dtype([('re', np.int8), ('im', np.int8)])
    # ci4_dtype = np.dtype([('re', np.int16), ('im', np.int16)])
    # ci8_dtype = np.dtype([('re', np.int32), ('im', np.int32)])
    # ci16_dtype = np.dtype([('re', np.int64), ('im', np.int64)])

    pvp_data = []
    for channel in range(metadata.getNumChannels()):
        num_pulses = metadata.getNumVectors(channel)
        pvp_data.append({
            'txTime': np.random.rand(num_pulses),  # (1, 'F8')
            'txPos': np.random.rand(num_pulses, 3),  # (3, 'X=F8;Y=F8;Z=F8;')
            'txVel': np.random.rand(num_pulses, 3),  # (3, 'X=F8;Y=F8;Z=F8;')
            'rcvTime': np.random.rand(num_pulses),  # (1, 'F8')
            'rcvPos': np.random.rand(num_pulses, 3),  # (3, 'X=F8;Y=F8;Z=F8;')
            'rcvVel': np.random.rand(num_pulses, 3),  # (3, 'X=F8;Y=F8;Z=F8;')
            'srpPos': np.random.rand(num_pulses, 3),  # (3, 'X=F8;Y=F8;Z=F8;')
            'aFDOP': np.random.rand(num_pulses),  # (1, 'F8')
            'aFRR1': np.random.rand(num_pulses),  # (1, 'F8')
            'aFRR2': np.random.rand(num_pulses),  # (1, 'F8')
            'fx1': np.random.rand(num_pulses),  # (1, 'F8')
            'fx2': np.random.rand(num_pulses),  # (1, 'F8')
            'toa1': np.random.rand(num_pulses),  # (1, 'F8')
            'toa2': np.random.rand(num_pulses),  # (1, 'F8')
            'tdTropoSRP': np.random.rand(num_pulses),  # (1, 'F8')
            'sc0': np.random.rand(num_pulses),  # (1, 'F8')
            'scss': np.random.rand(num_pulses),  # (1, 'F8')
            # Optional params
            'ampSF': np.random.rand(num_pulses),  # (1, 'F8')
            'fxN1': np.random.rand(num_pulses),  # (1, 'F8')
            'fxN2': np.random.rand(num_pulses),  # (1, 'F8')
            'signal': np.ones(num_pulses, dtype=np.int8),  # (1, 'I8')
            'tdIonoSRP': np.random.rand(num_pulses),  # (1, 'F8')
            'toaE1': np.random.rand(num_pulses),  # (1, 'F8')
            'toaE2': np.random.rand(num_pulses),  # (1, 'F8')
            # Custom params
            'customF4Param': np.random.rand(num_pulses).astype(np.float32),  # (1, 'F4')
            'customF8Param': np.random.rand(num_pulses).astype(np.float64),  # (1, 'F8')
            'customU1Param': np.random.randint(127, size=num_pulses, dtype=np.uint8),  # (1, 'U1')
            'customU2Param': np.random.randint(127, size=num_pulses, dtype=np.uint16),  # (1, 'U2')
            'customU4Param': np.random.randint(127, size=num_pulses, dtype=np.uint32),  # (1, 'U4')
            'customU8Param': np.random.randint(127, size=num_pulses, dtype=np.uint64),  # (1, 'U8')
            'customI1Param': np.random.randint(127, size=num_pulses, dtype=np.int8),  # (1, 'I1')
            'customI2Param': np.random.randint(127, size=num_pulses, dtype=np.int16),  # (1, 'I2')
            'customI4Param': np.random.randint(127, size=num_pulses, dtype=np.int32),  # (1, 'I4')
            'customI8Param': np.random.randint(127, size=num_pulses, dtype=np.int64),  # (1, 'I8')
            # TODO: NumPy has no complex int type, use a structured array
            # 'customCI2Param': np.array([*zip(
            #                        np.random.randint(127, size=num_pulses, dtype=np.int8),
            #                        np.random.randint(127, size=num_pulses, dtype=np.int8))
            #                    ], dtype=ci2_dtype),  # (1, 'CI2')
            # 'customCI4Param': np.array([*zip(
            #                        np.random.randint(127, size=num_pulses, dtype=np.int16),
            #                        np.random.randint(127, size=num_pulses, dtype=np.int16))
            #                    ], dtype=ci4_dtype),  # (1, 'CI4')
            # 'customCI8Param': np.array([*zip(
            #                        np.random.randint(127, size=num_pulses, dtype=np.int32),
            #                        np.random.randint(127, size=num_pulses, dtype=np.int32))
            #                    ], dtype=ci8_dtype),  # (1, 'CI8')
            # 'customCI16Param': np.array([*zip(
            #                        np.random.randint(127, size=num_pulses, dtype=np.int64),
            #                        np.random.randint(127, size=num_pulses, dtype=np.int64))
            #                    ], dtype=ci16_dtype),  # (1, 'CI16')
            'customCF8Param': (np.random.rand(num_pulses)
                               + np.random.rand(num_pulses) * 1j)
                               .astype('complex64'),  # (1, 'CF8')
            'customCF16Param': (np.random.rand(num_pulses)
                               + np.random.rand(num_pulses) * 1j)
                               .astype('complex128')  # (1, 'CF16')
            # TODO: Custom string param test
        })

    return pvp_data


def get_test_widebands(metadata):
    widebands = []
    for channel in range(metadata.getNumChannels()):
        shape = (metadata.getNumVectors(channel), metadata.getNumSamples(channel))
        widebands.append((np.random.rand(*shape) + np.random.rand(*shape) * 1j)
                         .astype('complex64'))
    return widebands
