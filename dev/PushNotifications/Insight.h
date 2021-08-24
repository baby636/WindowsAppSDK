// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "WindowsAppSDKInsights.h"

class PushTelemetry : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS(PushTelemetry, "Microsoft.WindowsAppSDK.Insights.PushTelemetry",
        // {e6bc6071-c4fd-5ecd-8ec8-f2b2f46cc173}
        (0xe6bc6071, 0xc4fd, 0x5ecd, 0x8e, 0xc8, 0xf2, 0xb2, 0xf4, 0x6c, 0xc1, 0x73));

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

    DEFINE_EVENT_METHOD(ToastActivationStart)(
        _In_ PCWSTR appUserModelId,
        _In_ PCWSTR activationType,
        std::string& correlationVector) noexcept
    {
        TraceLoggingWrite(TraceLoggingType::Provider(), "ToastActivationStart", TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));
            //"ToastActivationStart");
            //TelemetryPrivacyDataTag(PDT_ProductAndServicePerformance),
            //TraceLoggingValue(CensorFilePath(appUserModelId), "AppUserModelId"),
            //TraceLoggingValue(activationType, "ActivationType"),
            //TraceLoggingString(correlationVector.c_str(), "CV"),
            //TraceLoggingCVString(correlationVector.c_str()));
#if 0
        if (c_maxEventLimit >= UpdateLogEventCount())
        {
            TraceLoggingClassWriteMeasure(
                "ToastActivationStart",
                TelemetryPrivacyDataTag(PDT_ProductAndServicePerformance),
                TraceLoggingValue(CensorFilePath(appUserModelId), "AppUserModelId"),
                TraceLoggingValue(activationType, "ActivationType"),
                TraceLoggingString(correlationVector.c_str(), "CV"),
                TraceLoggingCVString(correlationVector.c_str()));
        }
#endif
    }

    DEFINE_EVENT_METHOD(ToastActivationStop)(
        _In_ PCWSTR appUserModelId,
        _In_ PCWSTR activationType,
        _In_ HSTRING argument,
        ULONG inputCount,
        HRESULT hr,
        std::string& correlationVector) noexcept
    {
#if 0
        if (c_maxEventLimit >= UpdateLogEventCount())
        {
            TraceLoggingClassWriteMeasure(
                "ToastActivationStop",
                TelemetryPrivacyDataTag(PDT_ProductAndServicePerformance),
                TraceLoggingValue(CensorFilePath(appUserModelId), "AppUserModelId"),
                TraceLoggingValue(activationType, "ActivationType"),
                TraceLoggingUInt64(argument == nullptr ? 0UL : wcslen(StringReference(argument).GetRawBuffer(nullptr)), "ArgumentLength"),
                TraceLoggingUInt32(inputCount, "InputCount"),
                TraceLoggingString(correlationVector.c_str(), "CV"),
                TraceLoggingCVString(correlationVector.c_str()),
                TraceLoggingHexUInt32(hr, "OperationResult"));
        }
#endif
    }
};
