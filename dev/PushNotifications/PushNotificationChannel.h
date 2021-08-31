﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once
#include "Microsoft.Windows.PushNotifications.PushNotificationChannel.g.h"
#include <NotificationsLongRunningProcess_h.h>

namespace winrt::Microsoft::Windows::PushNotifications::implementation
{
    typedef winrt::Windows::Foundation::TypedEventHandler<
        winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel,
        winrt::Microsoft::Windows::PushNotifications::PushNotificationReceivedEventArgs> PushNotificationEventHandler;

    struct PushNotificationChannel : PushNotificationChannelT<PushNotificationChannel, IWpnForegroundSink>
    {
        PushNotificationChannel(winrt::Windows::Networking::PushNotifications::PushNotificationChannel const& channel);

        PushNotificationChannel(hstring const& channelUri, hstring const& channelId, hstring const& appUserModelId, winrt::Windows::Foundation::DateTime const& extime);

        winrt::Windows::Foundation::Uri Uri();
        winrt::Windows::Foundation::DateTime ExpirationTime();
        void Close();

        winrt::event_token PushReceived(winrt::Windows::Foundation::TypedEventHandler<Microsoft::Windows::PushNotifications::PushNotificationChannel, Microsoft::Windows::PushNotifications::PushNotificationReceivedEventArgs> handler);
        void PushReceived(winrt::event_token const& token) noexcept;

        // IWpnForegroundSink
        HRESULT __stdcall InvokeAll(ULONG length, byte* payload) noexcept;

    private:
        bool IsPackagedAppScenario();
        bool IsBackgroundTaskBuilderAvailable();

        const winrt::Windows::Networking::PushNotifications::PushNotificationChannel m_channel{ nullptr };
        winrt::Windows::Foundation::DateTime m_channelExpirationTime{};
        winrt::Windows::Foundation::Uri m_channelUri{ nullptr };
        winrt::hstring m_channelId;
        winrt::hstring m_appUserModelId;

        bool m_isRegisteredWithLRP = false;
        winrt::event<PushNotificationEventHandler> m_foregroundHandlers;
    };
}

namespace winrt::Microsoft::Windows::PushNotifications::factory_implementation
{
    struct PushNotificationChannel : PushNotificationChannelT<PushNotificationChannel, implementation::PushNotificationChannel>
    {
    };
}
