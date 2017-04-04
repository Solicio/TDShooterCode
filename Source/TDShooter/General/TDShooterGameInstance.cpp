// Fill out your copyright notice in the Description page of Project Settings.

#include "TDShooter.h"
#include "TDShooterGameInstance.h"

UTDShooterGameInstance::UTDShooterGameInstance()
{

	// Bind functions delegates for creating and starting the session
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnStartOnlineGameComplete);

	// Bind function for finding a session
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnFindSessionsComplete);

	// Bind function for joining a session
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnJoinSessionComplete);

	// Bind function for ending a session
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnEndSessionComplete);

	// Bind function for destroying a session
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UTDShooterGameInstance::OnDestroySessionComplete);

	PrintDuration = 10.0f;
}


//--- Creating Sessions ---//

void UTDShooterGameInstance::LaunchCentralHub(const FName& InSessionName, int32 MaxNumPlayers, bool IsLan, bool IsPresence)
{
	if(ShouldDebugPrint)
		PrintText("Launch Hub!", -1, PrintDuration);

	// Get the user id of the player
	//TSharedPtr<const FUniqueNetId> ID = IOnlineSubsystem::Get()->GetIdentityInterface()->GetUniquePlayerId(0);

	// Create a local player that we can get the ID from.
	ULocalPlayer* const Player = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> ID = Player->GetPreferredUniqueNetId();
	
	// Attempt to create and start a session.
	HostSession(ID, InSessionName, IsLan, IsPresence, MaxNumPlayers);
}

bool UTDShooterGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool IsLan, bool IsPresence, int32 MaxNumPlayers)
{
	// Get the online subsystem
	const IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		if (ShouldDebugPrint)
			PrintText("OnlineSubSystem found!", -1, PrintDuration);

		// Get the session interface, so we can call the "CreateSession" function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Fill in the settings
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = IsLan;
			SessionSettings->bUsesPresence = IsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_MAPNAME, FString(/*"CentralHub"*/"ShooterDevMap"), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_SESSIONNAME, FString(SessionName.ToString()), EOnlineDataAdvertisementType::ViaOnlineService);

			// Set the delegate to the handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete. Request the sessions to be completed.
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	else
	{
		if (ShouldDebugPrint)
			PrintText("No OnlineSubSystem found!", -1, PrintDuration, FColor::Red);
	}

	return false;
}

void UTDShooterGameInstance::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	if (ShouldDebugPrint)
		PrintText("OnCreateSessionComplete, " + SessionName.ToString() + ((WasSuccessful) ? ", Success!" : ", Failure!"), -1, PrintDuration, (WasSuccessful ? FColor::Green : FColor::Red));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (WasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}
	}
}

void UTDShooterGameInstance::OnStartOnlineGameComplete(FName SessionName, bool WasSuccessful)
{
	if (ShouldDebugPrint)
		PrintText("OnStartSessionComplete, " + SessionName.ToString() + ((WasSuccessful) ? ", Success!" : ", Failure!"), -1, PrintDuration, (WasSuccessful ? FColor::Green : FColor::Red));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (WasSuccessful)
	{
		// Set the name of the current session
		CurrentSessionName = SessionName;

		UGameplayStatics::OpenLevel(GetWorld(), /*"CentralHub"*/"ShooterDevMap", true, "listen");
	}
}






//--- Finding Sessions ---//


void UTDShooterGameInstance::FindAllSessions(bool IsLan, bool IsPresence)
{
	// Get the user id of the player
	//TSharedPtr<const FUniqueNetId> ID = IOnlineSubsystem::Get()->GetIdentityInterface()->GetUniquePlayerId(0);

	// Create a local player that we can get the ID from.
	ULocalPlayer* const Player = GetFirstGamePlayer();
	TSharedPtr<const FUniqueNetId> ID = Player->GetPreferredUniqueNetId();

	// Find the sessions
	FindSessions(ID, IsLan, IsPresence);
}

void UTDShooterGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool IsLan, bool IsPresence)
{
	if (ShouldDebugPrint)
		PrintText("Start Search!", -1, PrintDuration);

	// Get the subsystem we want to work with.
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Fill in all the settings in the SearchSettings variable
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = IsLan;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query setting if IsPresence is true
			if (IsPresence)
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, IsPresence, EOnlineComparisonOp::Equals);

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the delegate handle to the find sessions function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			IsSearching = true;

			// Finally Call the SessionsInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);

		}
	}
	else
	{
		if (ShouldDebugPrint)
			PrintText("No OnlineSubSystem found!", -1, PrintDuration, FColor::Red);

		IsSearching = false;

		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}

void UTDShooterGameInstance::OnFindSessionsComplete(bool WasSuccessful)
{
	if (ShouldDebugPrint)
		PrintText("OnStartSessionComplete" + FString(((WasSuccessful) ? ", Success!" : ", Failure!")), -1, PrintDuration, (WasSuccessful ? FColor::Green : FColor::Red));

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			// Make sure it's empty before we start adding elements
			SessionArray.Empty();

			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Print how many search results was found
			if (ShouldDebugPrint)
				PrintText("Num Search Results: " + FString::FromInt(SessionSearch->SearchResults.Num()), -1, PrintDuration);

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets
			if (SessionSearch->SearchResults.Num() > 0)
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information.
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// Get the name of the session
					FString SessionNameSetting;
					SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get<FString>(SETTING_SESSIONNAME, SessionNameSetting);

					// Add this session into the array
					FSessionElement NewSessionElement;
					NewSessionElement.SessionName = *SessionNameSetting;
					SessionArray.Add(NewSessionElement);

					// Print out the sessions
					//PrintText("Session Number: " + FString::FromInt(SearchIdx + 1) + " , SessionName: " + SessionSearch->SearchResults[SearchIdx].Session.OwningUserName, -1, PrintDuration);
					if (ShouldDebugPrint)
						PrintText("Session Number: " + FString::FromInt(SearchIdx + 1) + " , SessionName: " + SessionNameSetting, -1, PrintDuration);
				}
			}
			else
			{
				if (ShouldDebugPrint)
					PrintText("No Sessions Found!", -1, PrintDuration, FColor::Red);
			}
		}
	}

	// Let the menu know that it should update the server list
	UpdateSeverList = true;
	IsSearching = false;
}




//--- Joining Sessions ---//

void UTDShooterGameInstance::JoinSpecifiedSession(FSessionElement SessionElement)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can store the one we want to use
	FOnlineSessionSearchResult SearchResult;

	// Temporary variable used when looking at all the session names
	FString TempSessionName;

	// If the Array is not empty, we can go through it
	if (SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// To avoid the player being somehow able to join itself, we filter against that
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				// Get the name of the search result 
				SessionSearch->SearchResults[i].Session.SessionSettings.Get<FString>(SETTING_SESSIONNAME, TempSessionName);

				// If the session we are looping over matches name with the one we want to join
				if (*TempSessionName == SessionElement.SessionName)
				{
					// Set the search result
					SearchResult = SessionSearch->SearchResults[i];

					// Attempt to join the session
					JoinGameSession(Player->GetPreferredUniqueNetId(), SessionElement.SessionName, SearchResult);
					
					break;
				}
			}
		}
	}
	else
	{
		if (ShouldDebugPrint)
			PrintText("No session to join!", -1, PrintDuration, FColor::Red);
	}
}

bool UTDShooterGameInstance::JoinGameSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	if (ShouldDebugPrint)
		PrintText("Attempting to join!", -1, PrintDuration);

	bool Successful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the delegate handle to the join session function
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			Successful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return Successful;
}

void UTDShooterGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	
	if (ShouldDebugPrint)
		PrintText("OnJoinSessionComplete: " + SessionName.ToString() + ", EOnJoinSessionCompleteResult: " + GetEnumValueToString<EOnJoinSessionCompleteResult::Type>("EOnJoinSessionCompleteResult", Result), -1, PrintDuration, ((Result == EOnJoinSessionCompleteResult::Success) ? FColor::Green : FColor::Red));
		//PrintText("OnJoinSessionComplete: " + SessionName.ToString() + FString(((Result == EOnJoinSessionCompleteResult::Success) ? ", Success!" : ", Failure!")), -1, PrintDuration, ((Result == EOnJoinSessionCompleteResult::Success) ? FColor::Green : FColor::Red));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			APlayerController* const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Set the current session
				CurrentSessionName = SessionName;
				
				// And empty the session array
				SessionArray.Empty();

				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
			else
			{
				if (ShouldDebugPrint)
					PrintText("Error when attempting to join!", -1, PrintDuration, FColor::Red);
			}
		}
	}
}

//--- Ending Sessions ---//

void UTDShooterGameInstance::EndCurrentSession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
			
			// End the session
			Sessions->EndSession(CurrentSessionName);
		}
	}
}

void UTDShooterGameInstance::OnEndSessionComplete(FName SessionName, bool WasSuccessful)
{
	if (ShouldDebugPrint)
		PrintText("OnEndSessionComplete," + SessionName.ToString() + FString(((WasSuccessful) ? ", Success!" : ", Failure!")), -1, PrintDuration, (WasSuccessful ? FColor::Green : FColor::Red));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);

			// If the session was ended, destroy it and leave.
			if (WasSuccessful)
				DestroySessionAndLeaveGame();
		}
	}
}


//--- Destroying Sessions ---//

void UTDShooterGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			// Destroy the session
			Sessions->DestroySession(CurrentSessionName);
		}
	}
}

void UTDShooterGameInstance::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	if (ShouldDebugPrint)
		PrintText("OnDestroySessionComplete," + SessionName.ToString() + FString(((WasSuccessful) ? ", Success!" : ", Failure!")), -1, PrintDuration, (WasSuccessful ? FColor::Green : FColor::Red));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (could be a MainMenu!)
			if (WasSuccessful)
			{
				CurrentSessionName = "";
				UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true);
			}
		}
	}
}