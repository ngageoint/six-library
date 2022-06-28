/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <cphd/Channel.h>

#include <memory>

#include <xml/lite/MinidomParser.h>
#include <six/Init.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

#include "TestCase.h"

TEST_CASE(EmptyChannel)
{
    cphd::Channel channel1, channel2;
    channel1.parameters.resize(3);
    channel2.parameters.resize(3);
    TEST_ASSERT_TRUE((channel1.parameters == channel2.parameters));
    TEST_ASSERT_TRUE((channel1 == channel2));
}

TEST_CASE(TestPolygonInvalid)
{
    cphd::Channel channel;

    six::Vector2 vertex1, vertex2;
    vertex1[0] = 0;
    vertex1[1] = 0;
    vertex2[0] = 5;
    vertex2[1] = 5;

    channel.parameters.resize(1);
    channel.parameters[0].imageArea.x1y1[0] = 0;
    channel.parameters[0].imageArea.x1y1[1] = 0;
    channel.parameters[0].imageArea.x2y2[0] = 10;
    channel.parameters[0].imageArea.x2y2[1] = 10;
    channel.parameters[0].imageArea.polygon.push_back(vertex1);
    channel.parameters[0].imageArea.polygon.push_back(vertex2);

    TEST_ASSERT_FALSE(channel.parameters[0].imageArea.polygon.size() >= 3);
}


TEST_CASE(TxRcvMultIds)
{
    cphd::Channel channel;
    channel.parameters.resize(2);
    channel.parameters[0].txRcv.reset(new cphd::ChannelParameter::TxRcv());
    channel.parameters[0].txRcv->txWFId.push_back("TransmitWaveformParam1");
    channel.parameters[0].txRcv->txWFId.push_back("TransmitWaveformParam2");
    channel.parameters[0].txRcv->txWFId.push_back("TransmitWaveformParam3");

    channel.parameters[0].txRcv->rcvId.push_back("ReceiverWaveformParam1");
    channel.parameters[0].txRcv->rcvId.push_back("ReceiverWaveformParam2");

    TEST_ASSERT_EQ(channel.parameters[0].txRcv->txWFId.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(channel.parameters[0].txRcv->rcvId.size(), static_cast<size_t>(2));
}

TEST_MAIN(
        TEST_CHECK(EmptyChannel);
        TEST_CHECK(TestPolygonInvalid);
        TEST_CHECK(TxRcvMultIds);
        )

