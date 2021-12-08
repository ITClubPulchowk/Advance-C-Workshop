#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#endif

// shouldn't C std have this function??
int file_exist(const char *file) {
	FILE *fp = fopen(file, "rb");
	if (fp) {
		fclose(fp);
		return 1;
	}
	return 0;
}

#ifdef _WIN32
void launch_process(const char *process) {
	STARTUPINFOA startup = { sizeof(startup) };
	PROCESS_INFORMATION  process_info;
	memset(&process_info, 0, sizeof(process_info));

#if defined(__clang__) || defined(__ibmxl__)
	char *process_name = _strdup(process);
#else
	char *process_name = strdup(process);
#endif

	if (CreateProcessA(NULL, process_name, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startup, &process_info)) {
		WaitForSingleObject(process_info.hProcess, INFINITE);

		DWORD code = 0;
		GetExitCodeProcess(process_info.hProcess, &code);
		printf("Process exited with %d exit code\n", (int)code);
	}
	else {
		fprintf(stderr, "ERROR: Failed to launch %s process\n", process);
	}
	free(process_name);
}
#elif defined(__linux__)
void launch_process(const char* process)
{
    if (!system(NULL))
    {
	fprintf(stderr,"\nERROR : No shell to execute the command\n");
	return ;
    }
    int code = system(process);
    
    // negative return codes are not allowed 
    if (WIFEXITED(code))
	fprintf(stdout,"\n\033[31mProcess exited with %d exit code\n\033[0m",WEXITSTATUS(code));
    else
	fprintf(stderr,"\n\033[34mERROR : Failed to launch %s process\n\033[0m",process);	
}
#endif

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Error, USAGE: %s <count> <process>\n", argv[0]);
		return 1;
	}

	int count = atoi(argv[1]);
	if (count == 0) {
		count = 1;
		fprintf(stderr, "Invalid count, running %d times\n\n", count);
	}

	const char *process = argv[2];

	if (!file_exist(process)) {
		fprintf(stderr, "Error %s file does not exist!!\n", process);
		return 1;
	}

	for (int i = 0; i < count; ++i) {
		printf("Launching %d...\n", i);
		launch_process(process);
		printf("\n\n");
	}

	return 0;
}
