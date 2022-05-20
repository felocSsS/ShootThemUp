// Shoot Them Up Game. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUCoreTypes.h"
#include "STUGameModeBase.generated.h"

class AAIController;

UCLASS()
class SHOOTTHEMUP_API ASTUGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    ASTUGameModeBase();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    TSubclassOf<AAIController> AIControllerClass;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    TSubclassOf<APawn> AIPawnClass;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FGameData GameData;

public:
    virtual void StartPlay() override;
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
    virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
    virtual bool ClearPause() override;

    void Killed(AController* KillerController, AController* VictimController);
    void RespawnRequest(AController* Controller);

    FGameData GetGameData() const { return GameData; }
    int32 GetCurrentRoundNum() const { return CurrentRound; }
    int32 GetRoundSecondsRemaining() const { return RoundCountDown; }

    FOnMatchStateChangedSignature OnMatchStateChanged;

private:
    int32 CurrentRound = 1;
    int32 RoundCountDown = 0;

    ESTUMatchState MatchState = ESTUMatchState::WaitingToStart;

    FTimerHandle GameRoundTimerHandle;

    void SpawnBots();
    void StartRound();
    void GameTimerUpdate();
    void ResetPlayers();
    void ResetOnePlayer(AController* Controller);
    void CreateTeamsInfo();
    void SetPlayerColor(AController* Controller);
    void StartRespawn(AController* Controller);
    void GameOver();
    void SetMatchState(ESTUMatchState State); 

    FLinearColor DetermineColorByTeamID(int32 TeamID) const;
};
