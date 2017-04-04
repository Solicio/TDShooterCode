// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "TDShooterGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FSessionElement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	FName SessionName;

};

#define SETTING_SESSIONNAME FName(TEXT("SESSIONNAME"))

/**
 * 
 */
UCLASS()
class TDSHOOTER_API UTDShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()

	public:

		UTDShooterGameInstance();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		bool ShouldDebugPrint;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
		float PrintDuration;


		/** The name of the current active session */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		FName CurrentSessionName;

		/** The current session list, filled with found sessions when FindAllSessions is called */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		TArray<FSessionElement> SessionArray;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		bool UpdateSeverList;

		/** If the system is currently searching for sessions */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer")
		bool IsSearching;


		/** Launches the lobby map. When a player chooses to host a server. */
		UFUNCTION(BlueprintCallable, Category = "Multiplayer")
		void LaunchCentralHub(const FName& InSessionName, int32 MaxNumPlayers, bool IsLan, bool IsPresence);

		/** Will find all public sessions and return them, used for server list */
		UFUNCTION(BlueprintCallable, Category = "Multiplayer")
		void FindAllSessions(bool IsLan, bool IsPresence);

		/** Attempts to join the specified session */
		UFUNCTION(BlueprintCallable, Category = "Multiplayer")
		void JoinSpecifiedSession(FSessionElement SessionElement);

		/** Ends the currently active session */
		UFUNCTION(BlueprintCallable, Category = "Multiplayer")
		void EndCurrentSession();

		/** Destroys the session and goes back to the main menu */
		UFUNCTION(BlueprintCallable, Category = "Multiplayer")
		void DestroySessionAndLeaveGame();



	private:


		//--- Hosting Sessions ---//

		/** Setting used by the session, contains thing such as if it's LAN or the max amounts of players */
		TSharedPtr<class FOnlineSessionSettings> SessionSettings;

		/** Starts a new session using the provided parameters
		*	UserId			The user that started the request.
		*	SessionName		Name of the sessions
		*	IsLAN			If the game should be over the local network
		*	IsPresence		"Is the Session to create a presence Session"
		*	MaxNumPlayers	How many players can connect to the server.
		*/
		bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool IsLan, bool IsPresence, int32 MaxNumPlayers);

		/** Delegate called when session created */
		FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
		/** Delegate called when session started */
		FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

		/** Handles to registered delegates for creating/starting a session */
		FDelegateHandle OnCreateSessionCompleteDelegateHandle;
		FDelegateHandle OnStartSessionCompleteDelegateHandle;

		/** Function that is called when the session create request was completed */
		virtual void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);

		/** Function that is called when the session start request was completed */
		void OnStartOnlineGameComplete(FName SessionName, bool WasSuccessful);

		//--- Hosting Sessions END ---//


		//--- Finding Sessions ---//

		/** Variable used for search settings, will also contain search results */
		TSharedPtr<class FOnlineSessionSearch> SessionSearch;

		/** Finds an online session
		*	UserId			The user that started the request.
		*	SessionName		Name of session this search will generate
		*	IsLan			If we are searching for lan sessions
		*	IsPresence		Are we searching for presence sessions
		*/
		void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool IsLan, bool IsPresence);

		/** Delegate for searching for sessions */
		FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

		/** Handle to registered delegate for searching a session */
		FDelegateHandle OnFindSessionsCompleteDelegateHandle;

		/** Delegate fired when a session search query has completed */
		void OnFindSessionsComplete(bool WasSuccessful);

		//--- Finding Sessions END ---//


		//--- Joining Sessions ---//

		/** Joins a session via a search result
		*	SessionName		The name of the session we want to join
		*	SearchResult	Sessions to join
		*/
		bool JoinGameSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

		/** Delegate for joining a session */
		FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

		/** Handle to registered delegate for joining a session */
		FDelegateHandle OnJoinSessionCompleteDelegateHandle;

		/** Delegate fired when a session join request was completed
		*	SessionName		The name of the sessions this callback is for
		*	Result			The result of he player trying to join a session
		*/
		void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

		//--- Joining Sessions END ---//


		//--- Ending Sessions ---//

		/** Delegate for ending a session */
		FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;

		/** Handle to registered delegate for ending a session */
		FDelegateHandle OnEndSessionCompleteDelegateHandle;

		/** Delegate fired when ending an online session has completed
		*	SessionName		The name of the session this callback is for
		*	WasSuccessful	If the sessions was ended successfully
		*/
		virtual void OnEndSessionComplete(FName SessionName, bool WasSuccessful);

		//--- Ending Sessions END ---//


		//--- Destroying Sessions ---//

		/** Delegate for destroying a session */
		FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

		/** Handle to registered delegate for destroying a session */
		FDelegateHandle OnDestroySessionCompleteDelegateHandle;

		/** Delegate fired when destroying an online session has completed
		*	SessionName		The name of the session this callback is for
		*	WasSuccessful	If the sessions was destroyed successfully
		*/
		virtual void OnDestroySessionComplete(FName SessionName, bool WasSuccessful);

		//--- Destroying Sessions END ---//


};
