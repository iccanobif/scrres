#include "windows.h"
#include "winuser.h"
#include "stdio.h"
#include "tchar.h"

#define IS_NUMBER(x) x >= 48 && x <= 57

typedef struct
{
	LPCTSTR screenName;
	LONG x;
	LONG y;
	DWORD width;
	DWORD height;
} DisplayInfo;

void initDisplayDevice(DISPLAY_DEVICE *DisplayDevice)
{
	ZeroMemory(DisplayDevice, sizeof(DISPLAY_DEVICE));
	DisplayDevice->cb = sizeof(DISPLAY_DEVICE);
}

void initDevMode(DEVMODE *DevMode)
{
	ZeroMemory(DevMode, sizeof(DEVMODE));
	DevMode->dmSize = sizeof(DEVMODE);
}

void printScreenList()
{
	int i = 0;
	DISPLAY_DEVICE DisplayDevice;

	initDisplayDevice(&DisplayDevice);

	while (EnumDisplayDevices(NULL, i, &DisplayDevice, 1))
	{
		if (DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{
			_tprintf("%d: %s (%s)\n", 
					 i, DisplayDevice.DeviceName, DisplayDevice.DeviceString);
		}

		i++;
	}
}

void stampaRisultato(int result)
{
	printf("An error occurred: ");

	switch (result)
	{
	case DISP_CHANGE_SUCCESSFUL: printf("DISP_CHANGE_SUCCESSFUL"); break ;
	case DISP_CHANGE_BADDUALVIEW: printf("DISP_CHANGE_BADDUALVIEW"); break;
	case DISP_CHANGE_BADFLAGS: printf("DISP_CHANGE_BADFLAGS"); break;
	case DISP_CHANGE_BADMODE: printf("DISP_CHANGE_BADMODE"); break;
	case DISP_CHANGE_BADPARAM: printf("DISP_CHANGE_BADPARAM"); break;
	case DISP_CHANGE_FAILED: printf("DISP_CHANGE_FAILED"); break;
	case DISP_CHANGE_NOTUPDATED: printf("DISP_CHANGE_NOTUPDATED"); break;
	case DISP_CHANGE_RESTART: printf("DISP_CHANGE_RESTART"); break;
	}

	printf("\n");
}

int changeResolution(int displayNumber, int width, int height)
{
	DEVMODE DevMode;
	DISPLAY_DEVICE DisplayDevice;
	LPCTSTR displayName;
	int result;
	
	if (width == 0 || height == 0)
	{
		printf("Not gonna do this...\n");
		return 1;
	}

	printf("Changing resolution of display %d to %dx%d...\n", displayNumber, width, height);

	//
	// Get display name
	//

	initDisplayDevice(&DisplayDevice);

	if (!EnumDisplayDevices(NULL, displayNumber, &DisplayDevice, 1))
	{
		printf("ERROR: Display %d not found!\n");
		return 1;
	}

	displayName = DisplayDevice.DeviceName;

	//
	// Get current display settings
	//

	if ((DisplayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
	{
		printf("ERROR: The selected display isn't attached to the desktop (Maybe it's not a monitor?\n");
		return 1;
	}

	initDevMode(&DevMode);
	if (!EnumDisplaySettings(displayName, ENUM_CURRENT_SETTINGS, &DevMode) ) 
	{ 
		printf("ERROR: Couldn't retrieve the selected display's current settings");
	}

	printf("Current resolution: %dx%d\n", DevMode.dmPelsWidth, DevMode.dmPelsHeight);

	DevMode.dmPelsWidth = width;
	DevMode.dmPelsHeight = height;
	DevMode.dmFields = DM_POSITION | DM_PELSHEIGHT | DM_PELSWIDTH;

	result = ChangeDisplaySettingsEx(displayName,
						   &DevMode,
						   NULL,
						   0,
						   NULL);

	if (result != DISP_CHANGE_SUCCESSFUL)
	{
		stampaRisultato(result);
		return 1;
	}

	printf("Resolution changed succesfully to %dx%d.\n", DevMode.dmPelsWidth, DevMode.dmPelsHeight);

	return 0; // Everything okay.
}

// Returns 1 if okay, else 0
int validateArguments(int argc, char** argv)
{
	int i;
	char* c;

	if (argc != 4)
		return 0;
	
	for (i = 1; i < 4; i++) // for every parameter
		for (c = argv[i]; *c != 0; c++) // for every character
			if (IS_NUMBER(*c) == 0)
				return 0;

	return 1;
}

int main(int argc, char** argv)
{
	//printf(validateArguments(argc, argv) ? "si\n" : "no\n");
	//return;

	if (!validateArguments(argc, argv))
	{
		printf("scrres\n");
		printf("Usage: scrres displayNumber width height\n");
		printf("Example: scrres 1 640 480\n\n");
		printf("The available displays and their display numbers are the following:\n\n");
		printScreenList();
		return 0;
	}

	return changeResolution(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
}
