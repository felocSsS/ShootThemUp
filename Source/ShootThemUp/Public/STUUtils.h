#pragma once


class STUUtils
{
public:
	template<typename T>
	static T* GetSTUPlayerComponent(AActor* PlayerActor)
	{
        if (!PlayerActor) return nullptr;

        const auto Component = PlayerActor->GetComponentByClass(T::StaticClass());
        return Cast<T>(Component);
	}
};