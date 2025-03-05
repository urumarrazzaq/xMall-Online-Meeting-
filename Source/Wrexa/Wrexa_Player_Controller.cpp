// Developed By Wrexa Technologies 2023


#include "Wrexa_Player_Controller.h"

void AWrexa_Player_Controller::Remove_All_Owned_Share_Component()
{
	for (int i = 0; i < Owned_Share_Components.Num(); i++) 
	{
		Owned_Share_Components[i]->Remove_Owner(this);
	}
}

//bool Share_Component_Ownership_Request_Validate(int32 AddHealth)
//{
//    if (AddHealth > MAX_ADD_HEALTH)
//    {
//        return false;                       // This will disconnect the caller
//    }
 //   return true;                              // This will allow the RPC to be called
//}

void AWrexa_Player_Controller::Share_Component_Ownership_Request_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component)
{
	if (!Can_Execute_Server_Code)
	{
		return;
	}
	bool Is_Successful = Target_Share_Component->Set_New_Owner(Requesting_Controller, false);

	Share_Component_Ownership_Response(Requesting_Controller, Target_Share_Component, Is_Successful);
	if (Is_Successful)
	{
		Share_Component_Ownership_Mulitcast(Target_Share_Component, true);
	}
}

void AWrexa_Player_Controller::Share_Component_Ownership_Response_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful)
{
	if (!Can_Execute_Client_Code)
	{
		return;
	}

	if (IsSuccessful)
	{
		Target_Share_Component->Set_New_Owner(this, true);
		Owned_Share_Components.AddUnique(Target_Share_Component);
	}

	On_Share_Component_Ownership_Response(Target_Share_Component, IsSuccessful);
}

void AWrexa_Player_Controller::Share_Component_Release_Request_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component)
{
	if (!Can_Execute_Server_Code)
	{
		return;
	}
	
	if (Target_Share_Component == nullptr)
	{
		return;
	}
	else
	{
		if (!IsValid(Target_Share_Component))
		{
			return;
		}
	}

	bool Is_Successful = Target_Share_Component->Remove_Owner(Requesting_Controller);
	Share_Component_Release_Response(Requesting_Controller, Target_Share_Component, Is_Successful);
	if (Is_Successful)
	{
		Share_Component_Ownership_Mulitcast(Target_Share_Component, false);
	}
}

void AWrexa_Player_Controller::Share_Component_Release_Response_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful)
{
	if (!Can_Execute_Client_Code)
	{
		return;
	}

	if (IsSuccessful)
	{
		IsSuccessful = Target_Share_Component->Remove_Owner(Requesting_Controller);
		Owned_Share_Components.Remove(Target_Share_Component);
	}

	On_Share_Component_Release_Response(Target_Share_Component, IsSuccessful);
}

void AWrexa_Player_Controller::On_Share_Component_Ownership_Response_Implementation(UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful)
{

}

void AWrexa_Player_Controller::On_Share_Component_Release_Response_Implementation(UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful)
{

}

void AWrexa_Player_Controller::Share_Component_Ownership_Mulitcast_Implementation(UWrexa_Share_Component* Target_Share_Component, bool Owning_True_Remove_False)
{
	//if (Owning_True_Remove_False)
	//{
	//	//Target_Share_Component->Set_New_Owner(this, true);
	//	//Owned_Share_Components.AddUnique(Target_Share_Component);
	//	//Target_Share_Component->Currently_Sharing = true;
	//}
	//else
	//{
	//
	//}
	On_Share_Component_Other_Ownership(Target_Share_Component, Owning_True_Remove_False);
}

void AWrexa_Player_Controller::On_Share_Component_Other_Ownership_Implementation(UWrexa_Share_Component* Target_Share_Component, bool Owning_True_Remove_False)
{


}

void AWrexa_Player_Controller::Share_Component_Ownership_Mirror_Request_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component)
{
	if (!Can_Execute_Server_Code)
	{
		return;
	}
	bool Is_Successful = false;
	if (Requesting_Controller != nullptr)
	{
		if ((IsValid(Requesting_Controller)))
		{
			if (Child_Share_Component->Owning_Controller == Requesting_Controller)
			{
				Is_Successful = Child_Share_Component->Set_New_Mirror_From(Parent_Share_Component, false);
			}
		}
	}

	Share_Component_Ownership_Mirror_Response(Requesting_Controller, Parent_Share_Component, Child_Share_Component, Is_Successful);
	if (Is_Successful)
	{
		Share_Component_Ownership_Mirror_Mulitcast(Parent_Share_Component, Child_Share_Component, true);
	}

}
void AWrexa_Player_Controller::Share_Component_Ownership_Mirror_Response_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool IsSuccessful)
{
	if (!Can_Execute_Client_Code)
	{
		return;
	}

	if (IsSuccessful)
	{
		Child_Share_Component->Set_New_Mirror_From(Parent_Share_Component, true);
	}

	On_Share_Component_Mirror_Response(Parent_Share_Component, Child_Share_Component, IsSuccessful);
}

void AWrexa_Player_Controller::On_Share_Component_Mirror_Response_Implementation(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool IsSuccessful)
{

}

void AWrexa_Player_Controller::Share_Component_Ownership_Mirror_Mulitcast_Implementation(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool Mirrored_True_Remove_False)
{
	if (Mirrored_True_Remove_False)
	{
		Child_Share_Component->Set_New_Mirror_From(Parent_Share_Component, true);
	}
	else
	{
		Child_Share_Component->Remove_Mirror_From(Parent_Share_Component);
	}
	On_Share_Component_Other_Ownership_Mirror(Parent_Share_Component, Child_Share_Component, Mirrored_True_Remove_False);
}

void AWrexa_Player_Controller::On_Share_Component_Other_Ownership_Mirror_Implementation(UWrexa_Share_Component* Parent_Share_Component, UWrexa_Share_Component* Child_Share_Component, bool Mirrored_True_Remove_False)
{


}

void AWrexa_Player_Controller::Share_Component_Mirror_Release_Request_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component)
{
	if (!Can_Execute_Server_Code)
	{
		return;
	}

	bool Is_Successful = Target_Share_Component->Stop_Mirror(Requesting_Controller, false);

	Share_Component_Mirror_Release_Response(Requesting_Controller, Target_Share_Component, Is_Successful);
	if (Is_Successful)
	{
		Share_Component_Ownership_Mirror_Release_Mulitcast(Target_Share_Component);
	}
}

void AWrexa_Player_Controller::Share_Component_Mirror_Release_Response_Implementation(AController* Requesting_Controller, UWrexa_Share_Component* Target_Share_Component, bool IsSuccessful)
{
	if (!Can_Execute_Client_Code)
	{
		return;
	}
}

void AWrexa_Player_Controller::Share_Component_Ownership_Mirror_Release_Mulitcast_Implementation(UWrexa_Share_Component* Target_Share_Component)
{
	Target_Share_Component->Stop_Mirror(nullptr, true);
}

void AWrexa_Player_Controller::On_New_Screen_Frame_Received_Implementation(const TArray <uint8>& RGBA_Buffer, int Width, int Height)
{
	return;
}