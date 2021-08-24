﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include <winrt/Windows.Foundation.Metadata.h>
#include "PushNotificationChannel.h"
#include "Microsoft.Windows.PushNotifications.PushNotificationChannel.g.cpp"
#include <winrt\Windows.Networking.PushNotifications.h>
#include <winrt\Windows.Foundation.h>
#include "PushNotificationReceivedEventArgs.h"

namespace winrt::Windows
{
    using namespace winrt::Windows::Networking::PushNotifications;
    using namespace winrt::Windows::Foundation;
}
namespace winrt::Microsoft
{
    using namespace winrt::Microsoft::Windows::PushNotifications;
}

namespace winrt::Microsoft::Windows::PushNotifications::implementation
{
    PushNotificationChannel::PushNotificationChannel(winrt::Windows::PushNotificationChannel const& channel): m_channel(channel) {}

    PushNotificationChannel::~PushNotificationChannel() noexcept
    {
        try
        {
            if (!IsPackagedApp())
            {
                char processName[1024];
                GetModuleFileNameExA(GetCurrentProcess(), NULL, processName, sizeof(processName) / sizeof(processName[0]));

                wil::com_ptr<INotificationsLongRunningPlatform> notificationsLongRunningPlatform{
                    wil::CoCreateInstance<NotificationsLongRunningPlatform, INotificationsLongRunningPlatform>(CLSCTX_LOCAL_SERVER) };

                notificationsLongRunningPlatform->UnregisterForegroundActivator(this, processName);
            }
        }
        CATCH_LOG()
    }

    winrt::Windows::Uri PushNotificationChannel::Uri()
    {
        return winrt::Windows::Uri{ m_channel.Uri() };
    }

    winrt::Windows::DateTime PushNotificationChannel::ExpirationTime()
    {
        return m_channel.ExpirationTime();
    }

    void PushNotificationChannel::Close()
    {
        try
        {
            m_channel.Close();
        }
        catch (...)
        {
            auto channelCloseException = hresult_error(to_hresult());
            if (channelCloseException.code() != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            {
                throw hresult_error(to_hresult());
            }
        }
    }

    winrt::event_token PushNotificationChannel::PushReceived(winrt::Windows::TypedEventHandler<winrt::Microsoft::Windows::PushNotifications::PushNotificationChannel, winrt::Microsoft::Windows::PushNotifications::PushNotificationReceivedEventArgs> handler)
    {
        if (IsPackagedApp()) // Should be !m_isBIAvailable <- just for testing
        {
            return m_channel.PushNotificationReceived([weak_self = get_weak(), handler](auto&&, auto&& args)
            {
                auto strong = weak_self.get();
                if (strong)
                {
                    handler(*strong, winrt::make<winrt::Microsoft::Windows::PushNotifications::implementation::PushNotificationReceivedEventArgs>(args));
                };
            });
        }
        else
        {
            char processName[1024];
            GetModuleFileNameExA(GetCurrentProcess(), NULL, processName, sizeof(processName) / sizeof(processName[0]));

            wil::com_ptr<INotificationsLongRunningPlatform> notificationsLongRunningPlatform{
                wil::CoCreateInstance<NotificationsLongRunningPlatform, INotificationsLongRunningPlatform>(CLSCTX_LOCAL_SERVER) };

            THROW_IF_FAILED(notificationsLongRunningPlatform->RegisterForegroundActivator(this, processName));

            return m_foregroundHandlers.add(handler);
        }
    }

    void PushNotificationChannel::PushReceived(winrt::event_token const& token) noexcept
    {
        if (IsPackagedApp())
        {
            m_channel.PushNotificationReceived(token);
        }
        else
        {
            m_foregroundHandlers.remove(token);
        }
    }

    HRESULT __stdcall PushNotificationChannel::InvokeAll(byte* start, ULONG length) noexcept try
    {
        std::vector<uint8_t> vec;

        for (int i = 0; (ULONG)i < length; i++)
        {
            vec.push_back(start[i]);
        }

        com_array<uint8_t> arr{ start, start + (length * sizeof(uint8_t)) };
        m_foregroundHandlers(*this, winrt::make<winrt::Microsoft::Windows::PushNotifications::implementation::PushNotificationReceivedEventArgs>(start, length));

        return S_OK;
    }
    CATCH_RETURN()

    bool PushNotificationChannel::IsBackgroundTaskBuilderAvailable()
    {
        return winrt::Windows::Metadata::ApiInformation::IsMethodPresent(L"Windows.ApplicationModel.Background.BackgroundTaskBuilder", L"SetTaskEntryPointClsid");
    }

    // Determines if the caller should be treated as packaged app or not.
    bool PushNotificationChannel::IsPackagedApp()
    {
        if (AppModel::Identity::IsPackagedProcess() && IsBackgroundTaskBuilderAvailable())
        {
            return true;
        }

        return false;
    }
}
