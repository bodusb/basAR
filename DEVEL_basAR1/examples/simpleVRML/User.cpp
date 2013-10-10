#include "User.h"
#include "Arpe.h"
#include "uIdent.h"


#include <list>
using namespace std;

User::User(void)
{
}


User::~User(void)
{
}

int User::userReadFile(){
		
	FILE          *fp;
	char           buf[256],buf1[256],buf2[256];
	int			   aux, retScan;

	//--------------------------------------------------------------------------
	// Open the User Configuration file
	//--------------------------------------------------------------------------
	printf("\n --------------------------------------------------------------------------");
	printf("\n Setting up User file %s", this->filename);
	printf("\n --------------------------------------------------------------------------");

	if( strcmp(this->filename,"") == 0) {
		printf("\n Any User will be assigned."); 
		return  0;
	}

	if( (fp=fopen(this->filename,"r")) == NULL) { printf("\n Error on opening %s user file!! ",this->filename);	return -1;}

	//--------------------------------------------------------------------------
	// Read the user identification
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }
	this->ident = new uIdent();
	if ( strcmp(buf1,"ANONYMOUS") == 0) {
		printf("\n No ID is used to this user, assuming generic user Anonymous");
		strcpy(this->ident->name,"Anonymous");
		strcpy(this->ident->basARID,"0000");
	} else {
		// WRITE READING FILE PROCESS
	}

	//--------------------------------------------------------------------------
	// Read the Log File
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
	if (sscanf(buf, "%s", &this->uLogFilename) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }

	if( strcmp(this->uLogFilename,"NO_LOG")) {
		// NO LOG IS DEFINED
		this->uLogList.clear();
		strcpy(this->uLogFilename,"");
		this->uLogIncremental = false;
		this->uLogRefreshSize = 0;
		printf("\n No LOG is defined");
	} else {
		// READ A LOG INFORMATION
		retScan = sscanf(buf, "%s %d %s", &this->uLogFilename, &aux, &buf1);

		switch(retScan){
		case 1:{ // <FILEADDRESS>
				this->uLogIncremental = false;
				this->uLogRefreshSize = 1;
				break;}
		case 2:{ // <FILEADDRESS> <SIZE>
				this->uLogIncremental = false;
				this->uLogRefreshSize = aux;
				break;}
		case 3:{ // <FILEADDRESS> <SIZE> <INCREMENTAL>
				this->uLogRefreshSize = aux;
				if (strcmp(buf1,"INCREMENTAL") == 0) {
					this->uLogIncremental = true;
				}
				break;}
		default: break;
		}
	}

	//--------------------------------------------------------------------------
	// Read the Static parameters
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }

	if (strcmp(buf1,"NO_STATIC") == 0) {
		strcpy(this->uStaticFilename,"");
		printf("\n No Static parameters");
		this->uStaticList.clear();
	} else {
		strcpy(this->uStaticFilename,buf1);
		printf("\n Static parameters file: %s",this->uStaticFilename);
		// LOAD FILE AND FILL 
	}

	//--------------------------------------------------------------------------
	// Read the Profile parameters
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }

	if (strcmp(buf1,"NO_PROFILE") == 0) {
		strcpy(this->uProfileFilename,"");
		printf("\n No Profile parameters");
		this->uProfileList.clear();
	} else {
		strcpy(this->uProfileFilename,buf1);
		printf("\n Static parameters file: %s",this->uProfileFilename);
		// LOAD FILE AND FILL 

	}

	//--------------------------------------------------------------------------
	// Read Adaptation
	//--------------------------------------------------------------------------
	getBuff(buf,256,fp);
	if (sscanf(buf, "%s", &buf1) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }

	if( strcmp(buf1,"NO_ADAPTATION") == 0) {
		this->myAdapt = 0;
		printf("\n Any adaptation will be loaded");
	} else {

		// READ SAD - Structure Adaptation Definitions

		if (sscanf(buf, "&s &s", &buf1, &this->adaptFilename) != 2) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }
		
		if ( strcmp(buf1,"ONLY_LOAD") == 0 ) {
			printf(" Will only load the file: %s",this->adaptFilename);
			// LOAD ADAPTATION MODULE
			// BUT DOESN'T APPLY
			printf(" SAD loaded");
		}
		if( strcmp(buf1,"ON_BEGINNING") == 0) {
			printf(" Will load and apply the file: %s",this->adaptFilename);
			// LOAD ADAPTATION MODULE
			// APPLY  -- SAD Adaptation will 
		}

		// READ number of ATs

		getBuff(buf,256,fp);
		if (sscanf(buf, "%d", &aux) != 1) { printf("\n Check %s file format", this->filename); fclose(fp); return -1; }



		// READ ATS
		for(int i = 0; i < aux ; i++){


		}





	}



	fclose(fp);
	return 1;
}



char User::getBuff(char *buf, int n, FILE *fp)
{
    char *ret;
	
    for(;;) {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return(NULL);
        if (buf[0] != '\n' && buf[0] != '#') return(1); // Skip blank lines and comments.
    }
}










