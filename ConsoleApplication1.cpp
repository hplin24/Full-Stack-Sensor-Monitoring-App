#include <iostream>
#include <Windows.h>

const std::wstring STLINK_COM_PORT = L"\\\\.\\COM3";

int main()
{
	HANDLE hCom;
	hCom = CreateFile(STLINK_COM_PORT.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		std::cout << "hCom init failed";
		return 0;
	}

	DCB dcb;
	//  Initialize the DCB structure.
	SecureZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(hCom, &dcb))
	{
		//  Handle the error.
		std::cout << "GetCommState failed with error: " << GetLastError() << std::endl;
		return 0;
	}

	dcb.BaudRate = CBR_9600;     //  baud rate
	dcb.ByteSize = 8;             //  data size, xmit and rcv
	dcb.Parity = NOPARITY;      //  parity bit
	dcb.StopBits = ONESTOPBIT;    //  stop bit

	if (!SetCommState(hCom, &dcb))
	{
		//  Handle the error.
		std::cout << "SetCommState failed with error: " << GetLastError() << std::endl;
		return 0;
	}

	while (1)
	{
		std::string str(10, 0);
		std::cout << "readfile: " << ReadFile(hCom, &str[0], 2, NULL, NULL) << std::endl;
		std::cout << str << std::endl;
	}

	return 0;
}