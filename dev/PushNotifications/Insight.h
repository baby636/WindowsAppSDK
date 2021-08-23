// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "WindowsAppSDKInsights.h"

class InsightsSample : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS(InsightsSample, "Microsoft.WindowsAppSDK.Insights.Sample",
        // {c782669f-f4db-53f2-9674-13b8897164cc}
        (0xc782669f, 0xf4db, 0x53f2, 0x96, 0x74, 0x13, 0xb8, 0x89, 0x71, 0x64, 0xcc));

    // {1870FBB0-2247-44D8-BF46-B02130A8A477}
    //IMPLEMENT_TRACELOGGING_CLASS(InsightsSample, "Microsoft.Windows.Notifications.WpnApis", (0x1870fbb0, 0x2247, 0x44d8, 0xbf, 0x46, 0xb0, 0x21, 0x30, 0xa8, 0xa4, 0x77));

    // Event that contains a single bool value as payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_BOOL(BooleanTelemetryEvent, // Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        value/* value to be logged*/);


    // Event that contains a string as payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_STRING(TextPayloadEvent, // Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        value/* value to be logged*/);

    // Event that contains a single user define data type payload
    DEFINE_COMPLIANT_TELEMETRY_EVENT_PARAM1(EventWithUserDefinedType, //Event name
        PDT_ProductAndServiceUsage, // Privacy data data for events. Check wil/traceloggingconfig.h for a full list
        PCWSTR, // Payload data type
        value);
};
