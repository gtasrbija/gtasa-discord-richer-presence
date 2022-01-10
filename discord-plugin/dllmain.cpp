#include "dllmain.h"
#include <string> 
#include <stdlib.h>


void MainThread()
{
	while (*(DWORD*)0xC8D4C0 != 9)
		Sleep(350);

	pGame = new Game();
	std::string details, state, smallImageText, largeImageText, largeImageKey;

	DiscordRichPresence drp;
	
	drp = { 0 };
	drp.startTimestamp = time(0);
	
	largeImageKey = "icon";
	drp.largeImageKey = largeImageKey.c_str();

	Discord_Initialize(APPLICATION_ID, 0, 0, 0);
	
	if (GetModuleHandleA("samp.dll"))
	{
		pSamp = new Samp();
				
		char buffer[500];
		wcstombs(buffer, GetCommandLine(), 500);
		while (!pSamp->readServerData(buffer));
		
		Query query(pSamp->srvData.address, std::stoi(pSamp->srvData.port));

		details = "SAMP nick: " + pSamp->srvData.username;
		smallImageText = "Igra SAMP";
		std::string fullAddress = "samp://" + pSamp->srvData.address + ':' + pSamp->srvData.port;
		drp.smallImageKey = "samp_icon";

		while (1 < 2) {
			if (query.info(pSamp->srvData.info)) {
				std::string players = std::to_string(pSamp->srvData.info.basic.players) + "/" + std::to_string(pSamp->srvData.info.basic.maxPlayers);
				state = "Server ime: " + pSamp->srvData.info.hostname;
				largeImageText = "Gamemode: " + pSamp->srvData.info.gamemode + "\nBroj igraca: " + players;

			}
			else {
				largeImageText = "Neuspesno uzimanje podataka sa servera!";
			}

			


			if (pGame->IsPedExists()) {
				drp.largeImageText = largeImageText.c_str();
				drp.smallImageText = smallImageText.c_str();
				drp.details = details.c_str();
				drp.state = state.c_str();

				Discord_UpdatePresence(&drp);
				Sleep(15000);

			}
		}
		
		
	}
	else
	{
		// Single Player
		
		drp.smallImageKey = "game_icon";

		// Loop
		while (1 < 2)
		{
			if (pGame->IsPedExists())
			{
				if (pGame->IsInCutscene()) {
					details = "Misija: " + pGame->GetCurrentMission();
					state = "Gleda cutscenu";
				}

				else if (pGame->IsInVehicle()) {
					if (pGame->GetVehicleID() >= 400 && pGame->GetVehicleID() <= 611) 
						 details = "Vozilo: " + vehNames[pGame->GetVehicleID() - 400];
					else details = "Hm, nisam siguran koje je to vozilo!";
					state = "Radio: " + radioNames[pGame->GetCurrentRadio()];
				}

				else if (pGame->IsAnyMissionActive()) {
					details = "Misija: " + pGame->GetCurrentMission();
					state = "Oruzje: " + weaponNames[pGame->GetCurrentWeapon()];
				}

				else if (pGame->GetPlayerWantedLevel()) {
					details = "Trazeni nivo: " + std::to_string(pGame->GetPlayerWantedLevel());
					state = "Oruzje: " + weaponNames[pGame->GetCurrentWeapon()];
				}

				else {
					details = "Novac: $" + std::to_string(pGame->GetPlayerMoney());
					char helt[64];
					sprintf_s(helt, "Zdravlje: %.2f%%", pGame->GetPlayerHealth());
					state = helt;
				}
												
				largeImageText = "Lokacija: " + pGame->GetCurrentZone();
				smallImageText = "Vreme u igri: " + pGame->GetTime();

				// Sending data
				drp.largeImageText = largeImageText.c_str();
				drp.smallImageText = smallImageText.c_str();
				drp.details = details.c_str();
				drp.state = state.c_str();

				Discord_UpdatePresence(&drp);

				Sleep(15000);
			}
		}

	}
}

std::string cp1251_to_utf8(const char* str)
{
	std::string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
	if (!result_u) { return 0; }
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return 0;
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), 0, 0, 0);
	}
	else if (reason == DLL_PROCESS_DETACH)
        Discord_Shutdown();

	return TRUE;
}