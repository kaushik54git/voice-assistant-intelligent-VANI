//										PROJECT VANI
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <unistd.h>
#include <Mmsystem.h>
#include<conio.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
//for linking gcc -o voice_recoder_c voice_recorder_c.c -lcurl -lwinmm
#define ALIAS "random_str"
#define MAX_WORD_LENGTH 100
#define FILENAME "C:\\Users\\kaushik\\OneDrive\\Desktop\\c program project\\out.wav.txt"

//int main(int argc,char *argv[])
void calling_mic()
{
	char mci_command[100];
	char ReturnString[300];
	int mci_error;
	FILE *fp;

	sprintf(mci_command, "open new type waveaudio alias %s", ALIAS);
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	// set the time format
	sprintf(mci_command,"set %s time format ms", ALIAS);    // just set time format
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	// start recording
	sprintf(mci_command, "record %s notify", ALIAS);
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	printf("Now on air, press a key too stop...\n");
    Sleep(5000);

	//stop recording
	sprintf(mci_command,"stop %s", ALIAS);
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	// save the file
	sprintf(mci_command, "save %s %s", ALIAS, "random1.wav");
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	// close the device
	sprintf(mci_command,"close %s", ALIAS);
	mci_error = mciSendString(mci_command, ReturnString, sizeof(ReturnString), NULL);

	freopen("NUL", "w", stdout);  //Redirect stdout to the null device
    freopen("NUL", "w", stderr);  // Redirect stderr to the null device

	system("wsl  ffmpeg -i random1.wav -acodec pcm_s16le -ac 1 -ar 16000 out.wav -y ");
	//system("main.exe -otxt out.wav");
	fp = popen("wsl curl --location --request POST \"https://centralindia.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1?language=en-US&format=detailed\""
           " --header \"Ocp-Apim-Subscription-Key: e650cf7f3f8e4b4f9721a05039ca6091\""
           " --header \"Content-Type: audio/wav\""
           " --data-binary \"@/mnt/c/Users/kaushik/OneDrive/Desktop/c program project/out.wav\""
           " --output response.json"
           " && wsl jq -r '.NBest[0].Display' response.json > output.txt","w");
	if (fp == NULL) {
        // Error occurred while executing the command
    }
	pclose(fp); 


	freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);

}

char *NthWord(int n) {
    FILE *file = fopen("C:\\Users\\kaushik\\OneDrive\\Desktop\\c program project\\output.txt", "r");
    if (file == NULL) {
        printf("Error opening the file.\n");
		calling_mic();
    }
	char* word = (char*)malloc(100 * sizeof(char));
    int count = 0;
    // Loop through each word in the file
    while (fscanf(file, "%s", word) ==1) {
		printf("yeah 12 \n");
        count++;
        if (count == n) {
            fclose(file);
			return word;
        }
    }
    
    fclose(file);
	return word;
}

void removeSpecialChars(char *sentence) {
    int i, j;
    int length = strlen(sentence);

    for (i = 0, j = 0; i < length; i++) {
        // Check if the current character is alphanumeric or space
        if (isalnum(sentence[i]) || isspace(sentence[i])) {
            sentence[j] = sentence[i];
            j++;
        }
    }

    sentence[j] = '\0';
}

void tts(const char* text){
	char command[5000];
    snprintf(command, sizeof(command), "curl --location --request POST \"https://centralindia.tts.speech.microsoft.com/cognitiveservices/v1\" --header \"Ocp-Apim-Subscription-Key: e650cf7f3f8e4b4f9721a05039ca6091\" --header \"Content-Type: application/ssml+xml\" --header \"X-Microsoft-OutputFormat: audio-16khz-128kbitrate-mono-mp3\" --header \"User-Agent: curl\" --data-raw \"<speak version='1.0' xml:lang='en-US'><voice xml:lang='en-US' xml:gender='female' name='en-IN-NeerjaNeural'>%s </voice></speak>\" --output output123.mp3", text);

    system(command);

    // Specify the path to the MP3 file
    const char* filePath = "C:\\Users\\kaushik\\OneDrive\\Desktop\\c program project\\output123.mp3";

    // Open the MP3 file
    MCI_OPEN_PARMS openParams = { 0 };
    openParams.lpstrDeviceType = "mpegvideo";
    openParams.lpstrElementName = filePath;

    if (mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&openParams) != 0) {
        printf("Failed to open the MP3 file.\n");
        //return 1;
    }

    // Play the MP3 file
    MCI_PLAY_PARMS playParams = { 0 };
    playParams.dwCallback = (DWORD_PTR)GetConsoleWindow();

    if (mciSendCommand(openParams.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&playParams) != 0) {
        printf("Failed to play the MP3 file.\n");
        //return 1;
    }

    // Get the length of the audio file
    MCI_STATUS_PARMS statusParams = { 0 };
    statusParams.dwItem = MCI_STATUS_LENGTH;

    if (mciSendCommand(openParams.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&statusParams) != 0) {
        printf("Failed to get the audio file length.\n");
        //return 1;
    }

    // Convert the length to milliseconds
    DWORD audioLength = statusParams.dwReturn;
    DWORD audioDuration = audioLength / 1000;

    //printf("Audio file duration: %u seconds\n", audioDuration);

	audioDuration*=1000;

    // Wait until the playback is finished
    Sleep(audioDuration);  // Adjust the duration as needed

    // Close the MP3 file
    MCI_GENERIC_PARMS closeParams = { 0 };

    if (mciSendCommand(openParams.wDeviceID, MCI_CLOSE, 0, (DWORD_PTR)&closeParams) != 0) {
        printf("Failed to close the MP3 file.\n");
        //return 1;
    }

}

void mousepointer(int x, int y){
    SetCursorPos(x, y); // Move the mouse cursor to the specified position

    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);

    // Simulate a left mouse button release
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void ts(const char* sentence) {
    // Simulate pressing the Alt key
    keybd_event(VK_MENU, 0, 0, 0); // Press Alt key
    keybd_event(VK_SPACE, 0, 0, 0); // Press space bar
    keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0); // Release space bar
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); // Release Alt key
    //INPUT inputs[strlen(sentence)];
    size_t sentenceLength = strlen(sentence);
    INPUT* inputs = malloc(sentenceLength * sizeof(INPUT));

    // Set up the inputs for each character in the sentence
    for (int i = 0; i<strlen(sentence); i++) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = 0;
        inputs[i].ki.wScan = sentence[i];
        inputs[i].ki.dwFlags = KEYEVENTF_UNICODE;
        inputs[i].ki.time = 0;
        inputs[i].ki.dwExtraInfo = 0;
    }

    // Send the key events to type the sentence
    SendInput(strlen(sentence), inputs, sizeof(INPUT));
    keybd_event(VK_RETURN, 0, 0, 0); // Press Enter key
    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0); // Release Enter key

    free(inputs);

}


char* getTextAfterWord(const char* word) {
    FILE* file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Failed to open the file: %s\n", "output.txt");
        return NULL;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char* wordPosition = strstr(buffer, word);
        if (wordPosition != NULL) {
            // Calculate the length of the text after the word
            size_t textLength = strlen(wordPosition);

            // Allocate memory for the result
            char* result = (char*)malloc((textLength + 1) * sizeof(char));
            if (result == NULL) {
                printf("Memory allocation failed.\n");
                fclose(file);
                return NULL;
            }

            // Copy the text after the word into the result
            strcpy(result, wordPosition);

            fclose(file);
            return result;
        }
    }

    fclose(file);
    return NULL;
}

void run_python(const char* sentence) {
    char command[200];
    sprintf(command, "C:/Users/kaushik/AppData/Local/Microsoft/WindowsApps/python3.10.exe \"c:/Users/kaushik/OneDrive/Desktop/c program project/gptgo.py\" \"%s\" > outputgptgo.txt", sentence);
    system(command);
}

void whatsapp(){
	FILE *file;
    char filename[] = "C:\\Users\\kaushik\\OneDrive\\Desktop\\c program project\\output.txt";  // Replace with your input file name
    char line[100];
    char *word;
    char output1[100] = "";
    char lastWord[100] = "";

    // Open the file
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open the file.\n");
    }

    // Read the first (and only) line
    if (fgets(line, sizeof(line), file) != NULL) {
        printf("l25");
        // Find the position of "sent" and "to"
        char *sentPos = strstr(line, "sent");
        char *sendPos = strstr(line, "send");
        char *toPos = strstr(line, "to");

        // Extract the desired portion between "sent" and "to"
        if ((sentPos != NULL && toPos != NULL && sentPos < toPos) || (sendPos != NULL && toPos != NULL && sendPos < toPos)) {
            char *start;
            if (sentPos != NULL && sentPos < toPos)
                start = sentPos + strlen("sent");
            else if (sendPos != NULL && sendPos < toPos)
                start = sendPos + strlen("send");
            else {
                printf("Error: Invalid input format.\n");
            }
            
            strncpy(output1, start, toPos - start);
            output1[toPos - start] = '\0';
        }
        else {
            printf("Error: Keywords not found in input.\n");
        }


        // Split the line into words to get the last word
        word = strtok(line, " ");
        while (word != NULL) {
            strcpy(lastWord, word);
            word = strtok(NULL, " ");
        }
    }

    // Print the desired portion
    printf("Output 1: %s\n", output1);

    // Print the last word
    printf("Last word: %s\n", lastWord);

    // Close the file
    fclose(file);

	char number[20]="+916383646159";
	/*if(strcmp("Avinash",lastWord)==0 || strcmp("avinash",lastWord)==0 ){
		strcpy(number, "+916383646159");
	}
	else if (strcmp("khadar",lastWord)==0 || strcmp("Khadar",lastWord)==0){
		strcpy(number, "+918688854584");
	}
	else if (strcmp("theva",lastWord)==0 || strcmp("Theva",lastWord)==0){
		strcpy(number, "+917397682607");
	}*/
	printf("%s \n",number);
	const char talk[] = "sending message to avinash ";
	//strcat(talk, lastWord);
	printf("talk  %s  \n ",talk);
	tts(talk);
	printf("316 \n");

    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://api.ultramsg.com/instance50192/messages/chat");

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    char requestParams[200];
    snprintf(requestParams, sizeof(requestParams), "token=6qlr4eoslfkmik57&to=%s&body=%s", number, output1);

    // Set the updated request parameters
    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, requestParams);

    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);

    CURLcode ret = curl_easy_perform(hnd);

    if (ret == CURLE_OK) {
        printf("Request sent successfully.\n");
    } else {
        printf("Failed to send request. Error: %s\n", curl_easy_strerror(ret));
    }

    curl_easy_cleanup(hnd);
    curl_slist_free_all(headers);
	tts("message send successfully");
}


void step2(){
	int r;
	char word[100];
	char *s21=NthWord(2);
	char *s3=NthWord(3);
	char *s4=NthWord(4);
	char start[100]="start ";
	removeSpecialChars(s4);
	if (strcmp("exit",s3)==0 || strcmp("Exit",s3)==0 ){
		int z=0;
		printf("hi");
	}
	else if (strcmp("open",s3)==0 || strcmp("Open",s3)==0 || strcmp("opens",s3)==0 || strcmp("Opens",s3)==0 || strcmp("open",s21)==0 || strcmp("Open",s21)==0 || strcmp("opens",s21)==0 || strcmp("Opens",s21)==0 ){
		if (strcmp("camera",s4)==0 || strcmp("camera",s3)==0){
			const char talk[] = "opening camera";
			tts(talk);
			system("start microsoft.windows.camera:");
		}else if (strcmp("store",s4)==0 || strcmp("store",s3)==0){
			const char talk[] = "opening microsoft store";
			tts(talk);
			system("start ms-windows-store://home");
		}else if (strcmp("whatsapp",s4)==0 || strcmp("WhatsApp",s4)==0 || strcmp("whatsapp",s3)==0 || strcmp("WhatsApp",s3)==0){
			const char talk[] = "opening whatsapp";
			tts(talk);
			system("\"start\" https://web.whatsapp.com");
			//const char talk[] = "entering password ";
			Sleep(15000);
			tts("entering password ");
			//Sleep(15000);
			int numberKeys[] = {0x32, 0x32, 0x30, 0x32, 0x39}; // ASCII codes for '2', '2', '0', '2', '9'
			int numKeys = sizeof(numberKeys) / sizeof(numberKeys[0]);

			for (int i = 0; i < numKeys; i++) {
				keybd_event(numberKeys[i], 0, 0, 0);
				keybd_event(numberKeys[i], 0, KEYEVENTF_KEYUP, 0);
			}
			keybd_event(VK_RETURN, 0, 0, 0);  // Press Enter key
			keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);  // Release Enter key
			// const char talk[] = "password entered successfully... welcome MASTER";
			tts("password entered successfully... welcome MASTER");
		}else if (strcmp("calendar",s4)==0 || strcmp("calendar",s3)==0){
			const char talk[] = "opening calender";
			tts(talk);
			system("start outlookcal:");
		}else if (strcmp("mail",s4)==0 || strcmp("mail",s3)==0){
			const char talk[] = "opening mail";
			tts(talk);
			system("start mailto:");
		}else if (strcmp("vs code",s4)==0 || strcmp("vs code",s3)==0){
			const char talk[] = "opening vs code";
			tts(talk);
			system("start code");
		}else if (strcmp("powerpoint",s4)==0 || strcmp("powerpoint",s3)==0){
			const char talk[] = "opening powerpoint";
			tts(talk);
			system("powerpnt");
		}else if (strcmp("words",s4)==0|| strcmp("words",s3)==0){
			const char talk[] = "opening microsoft word";
			tts(talk);
			system("start winword");
		}else if (strcmp("recycle bin",s4)==0 || strcmp("recycle bin",s3)==0){
			const char talk[] = "opening recycling bin";
			tts(talk);
			system("start explorer.exe shell:RecycleBinFolder");
		}else if (strcmp("shut down",s4)==0 || strcmp("shut down",s3)==0){
			const char talk[] = "shutting down... bye";
			tts(talk);
			system("shutdown -s");
		}else if (strcmp("restart",s4)==0 ||strcmp("restart",s3)==0){
			const char talk[] = "restarting";
			tts(talk);
			system("shutdown /r /t 0");
		}else if (strcmp("Calculator",s4)==0 || strcmp("calculator",s4)==0){
			const char talk[] = "opening calculator";
			tts(talk);
			system("calc.exe ");
		}else if (strcmp("gallary",s4)==0 || strcmp("photos",s4)==0){
			const char talk[] = "opening photos or gallary";
			tts(talk);
			system("start ms-photos:");
		}else if (strcmp("clock",s4)==0 || strcmp("clock",s3)==0){
			const char talk[] = "opening clock";
			tts(talk);
			system("start ms-clock:");
		}else if (strcmp("lock",s4)==0 || strcmp("lock",s3)==0){
			const char talk[] = "locking the screen";
			tts(talk);
			system("rundll32.exe user32.dll,LockWorkStation");
		}else if (strcmp("wheather",s4)==0 || strcmp("wheather",s3)==0){
			const char talk[] = "opening wheather";
			tts(talk);
			system("ms-weather:");
		}else if (strcmp("notepad",s4)==0 || strcmp("notepad",s3)==0){
			const char talk[] = "opening notepad";
			tts(talk);
			system("start notepad");
			// const char talk[] = "start talking ";
			tts("start talking");
			keybd_event(VK_LWIN, 0, 0, 0);
			keybd_event('H', 0, 0, 0);

			// Release the keys
			keybd_event('H', 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
		}else{
			//strcat(start,s4);
			//removeSpecialChars(start);
			char talk[] = "opening";
			strcat(talk, s4);
			tts(talk);
			ts(s4);
		}
	}
	else if (strcmp("close",s3)==0 || strcmp("Close",s3)==0){
		mousepointer(1155,20);

	}else if (strcmp("end",s3)==0 || strcmp("End",s3)==0 || strcmp("and",s3)==0){
		mousepointer(1255,20);

	}else if (strcmp("sent",s3)==0 || strcmp("Sent",s3)==0 || strcmp("send",s3)==0){
		whatsapp();
	}else if (strcmp("increase",s3)==0 || strcmp("increase",s21)==0){
		//if (strcmp("volume",s4)==0 || strcmp("volume",s3)==0){
		const char talk[] = " increasing volume ";
		tts(talk);
		keybd_event(VK_VOLUME_UP, 0, 0, 0); // Press volume up key
		keybd_event(VK_VOLUME_UP, 0, KEYEVENTF_KEYUP, 0); // Release volume up key
		//}
		//else{
	}else if (strcmp("decrease",s3)==0 || strcmp("decrease",s21)==0){
		const char talk[] = " decreasing volume ";
		tts(talk);
		keybd_event(VK_VOLUME_DOWN, 0, 0, 0); // Press volume down key
		keybd_event(VK_VOLUME_DOWN, 0, KEYEVENTF_KEYUP, 0); // Release volume down key


	}else if (strcmp("select",s3)==0 || strcmp("Select",s3)==0){
		const char talk[] = "selecting all ";
		tts(talk);
		keybd_event(VK_CONTROL, 0, 0, 0); // Press Ctrl key
		keybd_event('A', 0, 0, 0); // Press "A" key
		keybd_event('A', 0, KEYEVENTF_KEYUP, 0); // Release "A" key
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Release Ctrl key
	}else if (strcmp("copy",s3)==0 || strcmp("Copy",s3)==0){
		const char talk[] = "copying all ";
		tts(talk);
		keybd_event(VK_CONTROL, 0, 0, 0); // Press Ctrl key
		keybd_event('c', 0, 0, 0); // Press "c" key
		keybd_event('c', 0, KEYEVENTF_KEYUP, 0); // Release "c" key
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Release Ctrl key
	}else if (strcmp("paste",s3)==0 || strcmp("Paste",s3)==0){
		const char talk[] = "pasting all ";
		tts(talk);
		keybd_event(VK_CONTROL, 0, 0, 0); // Press Ctrl key
		keybd_event('v', 0, 0, 0); // Press "v" key
		keybd_event('v', 0, KEYEVENTF_KEYUP, 0); // Release "v" key
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Release Ctrl key
	}else if (strcmp("extract",s3)==0 || strcmp("Extract",s3)==0){
		const char talk[] = "start extracting  ";
		tts(talk);
		keybd_event(VK_LWIN, 0, 0, 0); // Press left Windows key
		keybd_event(VK_SHIFT, 0, 0, 0); // Press Shift key
		keybd_event('t', 0, 0, 0); // Press "t" key
		keybd_event('t', 0, KEYEVENTF_KEYUP, 0); // Release "t" key
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0); // Release Shift key
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0); // Release left Windows key

	}else if (strcmp("cursor",s3)==0 || strcmp("Cursor",s3)==0){
		const char talk[] = "cursor is here ";
		tts(talk);
		for(int j=0;j<=1;j++){
			keybd_event(VK_CONTROL, 0, 0, 0); // Press Ctrl key
			keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Release Ctrl key

		}
		Sleep(3);
		keybd_event(VK_CONTROL, 0, 0, 0); // Press Ctrl key
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Release Ctrl key

	}else{
		const char* word = s3;
		char* result = getTextAfterWord(word);

		//const char* input_sentence = "Hello, world!";
		run_python(result);

		if (result != NULL) {
			free(result);
		}

		// Read the result from the output file
		FILE *file;
		char *content;
		long file_size;
		
		// Open the file
		file = fopen("outputgptgo.txt", "r");
		if (file == NULL) {
			printf("Failed to open the file.\n");
			//return 1;
		}
		
		// Determine the file size
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);
		
		// Allocate memory to store the content
		content = (char *) malloc(file_size + 1);
		if (content == NULL) {
			printf("Failed to allocate memory.\n");
			fclose(file);
			//return 1;
		}
		
		// Read the content of the file
		fread(content, file_size, 1, file);
		content[file_size] = '\0'; // Add a null terminator at the end
		
		// Close the file
		fclose(file);

		// remove all new line characters
		char cleanedText[5000];
		int j = 0;
		for (int i = 0; content[i] != '\0'; i++) {
			if (content[i] != '\n' && content[i] != '\r') {
				cleanedText[j++] = content[i];
			}
    }
		cleanedText[j] = '\0';

		free(content);

		tts(cleanedText);
		
	}
	free(s3);
	free(s4);

}

int main(int argc,char *argv[]){
	int i=0,result;
    char word[100],s1[100],s2[100][100]= {"Hey Vani","hevani","herevani","heyvani","evani","A vani","Hevani"," Himani","Hey Bonnie","Here Wani"," Here Vani","vani","Vani","Hey Wani"};
	char *end_w=NthWord(3);
	char *word2=NthWord(1);
	char *word31=NthWord(2);

	while(strcmp("exit.",end_w)!=0){
		printf("recognising");
		calling_mic();
		printf("484 \n");

		word2=NthWord(1);
		word31=NthWord(2);

		strcpy(s1, word2);
		strcat(s1," ");
		strcat(s1, word31);
		removeSpecialChars(s1);
		printf("%s \n \n",s1); //output as "HeyVani"

		for (i=0;i<=13;i++){
			result=strcmp(s1,s2[i]);
			if (result==0 || strcmp(word2,s2[i])==0){
				printf("101 /t %d",i); // true and move forward
				step2();
			}
		}

	}
	free(word31);
	free(word2);
	//calling_mic();
	return 0;
}
