#pragma once

class AnimUtils
{
public:

    template<typename T>
    static T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (!Animation) return nullptr;

        const auto NotifysEvents = Animation->Notifies;
        for (auto NotifyEvent : NotifysEvents)
        {
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);
            if (AnimNotify)
            {
                return AnimNotify;
            }
        }
        return nullptr;
    }
};