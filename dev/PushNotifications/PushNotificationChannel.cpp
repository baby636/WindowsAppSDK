﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include <winrt/Windows.Foundation.Metadata.h>
#include "PushNotificationChannel.h"
#include "Microsoft.Windows.PushNotifications.PushNotificationChannel.g.cpp"
#include <winrt\Windows.Networking.PushNotifications.h>
#include <winrt\Windows.Foundation.h>
#include "PushNotificationReceivedEventArgs.h"
#include <FrameworkUdk/PushNotifications.h>
#include "externs.h"

namespace winrt::Windows
{
    using namespace winrt::Windows::Networking::PushNotifications;
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Metadata;
}
namespace winrt::Microsoft
{
    using namespace winrt::Microsoft::Windows::PushNotifications;
}

namespace winrt::Microsoft::Windows::PushNotifications::implementation
{
    PushNotificationChannel::PushNotificationChannel(winrt::Windows::PushNotificationChannel const& channel): m_channel(channel) {}

    PushNotificationChannel::PushNotificationChannel(hstring const& channelUri,
        hstring const& channelId,
        hstring const& appUserModelId,
        winrt::Windows::Foundation::DateTime const& channelExpirationTime)
    {
        m_channelUri = winrt::Windows::Uri{ channelUri };
        m_channelExpirationTime = channelExpirationTime;
        m_channelId = channelId;
        m_appUserModelId = appUserModelId;
    }

    winrt::Windows::Uri PushNotificationChannel::Uri()
    {
        if (m_channel)
        {
            return winrt::Windows::Uri{ m_channel.Uri() };
        }
        else
        {
            return m_channelUri;
        }
    }

    winrt::Windows::DateTime PushNotificationChannel::ExpirationTime()
    {
        if (m_channel)
        {
            return m_channel.ExpirationTime();
        }
        else
        {
            return m_channelExpirationTime;
        }
    }

    void PushNotificationChannel::Close()
    {
        try
        {
            if (m_channel)
            {
                m_channel.Close();
            }
            else
            {
               PushNotifications_CloseChannel(m_appUserModelId.c_str(), m_channelId.c_str());
            }
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
        if (IsPackagedAppScenario())
        {
            return m_channel.PushNotificationReceived([weak_self = get_weak(), handler](auto&&, auto&& args)
            {
                if (auto strong = weak_self.get())
                {
                    handler(*strong, winrt::make<winrt::Microsoft::Windows::PushNotifications::implementation::PushNotificationReceivedEventArgs>(args));
                };
            });
        }
        else
        {
            auto lock = m_lock.lock_exclusive();
            if (!m_foregroundHandlerCount++)
            {
                wil::com_ptr<INotificationsLongRunningPlatform> notificationsLongRunningPlatform{
                    wil::CoCreateInstance<NotificationsLongRunningPlatform, INotificationsLongRunningPlatform>(CLSCTX_LOCAL_SERVER) };

                wil::unique_cotaskmem_string processName;
                THROW_IF_FAILED(GetCurrentProcessPath(processName));

                THROW_IF_FAILED(notificationsLongRunningPlatform->RegisterForegroundActivator(this, processName.get()));             
            }
            return m_foregroundHandlers.add(handler);
        }
    }

    void PushNotificationChannel::PushReceived(winrt::event_token const& token) noexcept
    {
        if (IsPackagedAppScenario())
        {
            m_channel.PushNotificationReceived(token);
        }
        else
        {
            auto lock = m_lock.lock_exclusive();
            m_foregroundHandlers.remove(token);
            if (!--m_foregroundHandlerCount)
            {
                wil::com_ptr<INotificationsLongRunningPlatform> notificationsLongRunningPlatform{
                    wil::CoCreateInstance<NotificationsLongRunningPlatform, INotificationsLongRunningPlatform>(CLSCTX_LOCAL_SERVER) };

                wil::unique_cotaskmem_string processName;
                THROW_IF_FAILED(GetCurrentProcessPath(processName));

                THROW_IF_FAILED(notificationsLongRunningPlatform->UnregisterForegroundActivator(processName.get()));
            }
        }
    }

    HRESULT __stdcall PushNotificationChannel::InvokeAll(_In_ ULONG length, _In_ byte* payload, _Out_ BOOL* foregroundHandled) noexcept try
    {
        auto args = winrt::make<winrt::Microsoft::Windows::PushNotifications::implementation::PushNotificationReceivedEventArgs>(payload, length);
        m_foregroundHandlers(*this, args);
        *foregroundHandled = args.Handled();
        return S_OK;
    }
    CATCH_RETURN()

    bool PushNotificationChannel::IsBackgroundTaskBuilderAvailable()
    {
        return winrt::Windows::ApiInformation::IsMethodPresent(L"Windows.ApplicationModel.Background.BackgroundTaskBuilder", L"SetTaskEntryPointClsid");
    }

    // Determines if the caller should be treated as packaged app or not.
    bool PushNotificationChannel::IsPackagedAppScenario()
    {
        return AppModel::Identity::IsPackagedProcess() && IsBackgroundTaskBuilderAvailable();
    }
}
