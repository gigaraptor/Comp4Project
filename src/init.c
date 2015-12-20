//---------------------------------- C PREPROCESSOR --------------------------


#ifndef INCLUDE_LOCK
#define INCLUDE_LOCK
#include "main.h"
#endif



//------------------------------ FUNCTIONS -----------------------------------
/*
	char *loadTextFile(const char *filename, int *success):
	Loads a file in a safe way and creates a string from it

*/

char *loadTextFile(const char *filename, int *success)
{

	int fileDescriptor;
	
	fileDescriptor = open(filename, O_RDONLY, 0600);//avoids the race condition issues
	if(fileDescriptor == -1)
	{
		fprintf(stderr, "open has failed : %s \n", strerror(errno));
		*success = FAIL;
		return NULL;
	}
	
	FILE *jsonFile = fdopen(fileDescriptor, "rb");
	int fileSize, result, looper;
	char *fileContents, *truncatedContents;
	if(!jsonFile)
	{
		fprintf(stderr, "fopen has failed : %s \n", strerror(errno));
		*success = FAIL;
		return NULL;
	}
	fileSize = getFileSize(jsonFile, success);
	if(!fileSize)
	{
		return NULL;
	
	}
	fileContents = malloc(sizeof(char) * fileSize);
	if(!fileContents)
	{
		fprintf(stderr, "malloc has failed : %s", strerror(errno));
		*success = FAIL;
		return NULL;
	
	}
	fread(fileContents,1,fileSize, jsonFile);
	fileContents[fileSize] = '\0';
	fclose(jsonFile);
	return fileContents;




}
/*
	int getFileSize(FILE *sizeToGet, int *success):
	Gets the size of a file

*/

int getFileSize(FILE *sizeToGet, int *success)
{
	int fileSize = 0;
	fseek(sizeToGet,0,SEEK_END);
	fileSize = ftell(sizeToGet);
	rewind(sizeToGet);
	if(errno > 0)
	{
		fprintf(stderr, "ftell has failed : %s", strerror(errno));
		*success = FAIL;
		return 0;
	}
	return fileSize;

}
/*
	optionsData *initOptions(char *fileContents, int *success):
	loads the options file in JSON to a optionsData structure

*/

optionsData initOptions(char *fileContents, int *success)
{
	optionsData tempOpt;
	json_t *tempJsonHandle, *optionsData;
	json_error_t errorHandle;
	
	tempJsonHandle = json_loads(fileContents,0, &errorHandle);
	if(!tempJsonHandle)
	{
		fprintf(stderr, "json_loads has failed : %s \n", errorHandle.text);
		*success = FAIL;
		return tempOpt;
	
	}
	
	optionsData = json_array_get(tempJsonHandle, 0);
	if(!json_is_object(optionsData))
	{
		fprintf(stderr,"json_object_get failed, didn't get an object\n");
		*success = FAIL;
		json_decref(tempJsonHandle);
		return tempOpt;
	
	}

	tempOpt.SCREEN_WIDTH = json_integer_value(json_object_get(optionsData,"SCREEN_WIDTH"));
	tempOpt.SCREEN_HEIGHT = json_integer_value(json_object_get(optionsData,"SCREEN_HEIGHT"));
	tempOpt.windowTitle = json_string_value(json_object_get(optionsData, "TITLE"));
	tempOpt.SAMPLE_SIZE = json_integer_value(json_object_get(optionsData,"SAMPLE_SIZE"));
	tempOpt.SAMPLE_FREQUENCY = json_integer_value(json_object_get(optionsData,"SAMPLE_FREQUENCY"));
	tempOpt.NO_CHANNELS = json_integer_value(json_object_get(optionsData,"NO_CHANNELS"));

	return tempOpt;
}

/*
	SDL_Window *initSDL(optionsData *opt, int *success):
	initialise SDL2 and associated library

*/

SDL_Window *initSDL(optionsData *opt, int *success)
{
	SDL_Window *temp;
	int SDL_Flags, IMG_Flags;
	SDL_Flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO;
	IMG_Flags = IMG_INIT_JPG | IMG_INIT_PNG;
	//Main SDL2 Library
	if(SDL_Init(SDL_Flags) < 0)
	{
		fprintf(stderr, "SDL_Init has failed : %s \n", SDL_GetError());
		*success = FAIL;
		return NULL;
	}
	//TTF Font Library
	if(TTF_Init() != 0)
	{
		fprintf(stderr, "TFF_Init has failed : %s \n", TTF_GetError());
		*success = FAIL;
		return NULL;
	}
	//Image library
	if(!(IMG_Init(IMG_Flags) & IMG_Flags))
	{
		fprintf(stderr, "IMG_Init has failed, %s \n", IMG_GetError());
		*success = FAIL;
		return NULL;
	
	}
	//Audio library
	if(Mix_OpenAudio(opt->SAMPLE_FREQUENCY, MIX_DEFAULT_FORMAT, opt->NO_CHANNELS, opt->SAMPLE_SIZE) < 0)//first arg : frequency of audio, second arg : format, third arg : Number of audio channels, forth arg : sample size
	{
		fprintf(stderr, "Mix_OpenAudio has failed, %s \n", Mix_GetError());
		*success = FAIL;
		return NULL;
	
	}
	//initialise window
	temp = SDL_CreateWindow(opt->windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, opt->SCREEN_WIDTH, opt->SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if(!temp)
	{
		fprintf(stderr, "SDL_CreateWindow has failed : %s \n");
		*success = FAIL;
		return NULL;
	
	}
	
	return temp;

}
/*
	SDL_Renderer *createRenderer(SDL_Window *screen, int *success):
	initialise hardware renderer

*/

SDL_Renderer *createRenderer(SDL_Window *screen, int *success)
{
	SDL_Renderer *temp;
	int Render_Flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	temp = SDL_CreateRenderer(screen, -1, Render_Flags);
	if(!temp)
	{
		fprintf(stderr, "SDL_CreateRenderer has failed : %s \n", SDL_GetError());
		*success = FAIL;
		return NULL;
	}

	return temp;

}