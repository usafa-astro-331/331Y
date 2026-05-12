
#include "definitions.h"     // Project definitions (this directory)
#include "sd_functions.h"
#include "zmodem.h"
#include <HardwareSerial.h>

extern HardwareSerial Xbee;

// File-scope objects (NOT in the header)
// static SdFat sd;
extern SdFs sd;
extern SdFile fout;

/*---------------------------------------------------------------------------------------------*/
// Function Definitions:
/*---------------------------------------------------------------------------------------------*/

/**
 * @brief Initializes the SD card with the specified chip select pin.
 * 
 * @param csPin The chip select pin for the SD card.
 * @return true if initialization was successful.
 */
bool sd_init(uint8_t csPin)
{
    pinMode(SD_CS_PIN, OUTPUT);
    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(25))) {
        while (1) { Xbee.println("[ERROR] SD card initialization failed. Card present?"); delay(2000); }
    } else {Xbee.println("[INFO] SD Card Initialized.");}
    return true;
}


/**
 * @brief Creates a new data file on the SD card with a custom filename preamble.
 *
 * Example output: "lab99_data001.csv"
 *
 * @param dataFile     Pointer to a File object that will be opened.
 * @param preamble     String to replace the "lab" portion of the filename.
 * @return true        If the file was successfully created and opened.
 * @return false       If file creation/opening failed.
 */
bool sd_createDataFile(FsFile *dataFile, const char *preamble) {

  char filename[40]; 
  int fileNumber = 1;

  // Safety: handle null pointer
  if (preamble == nullptr || strlen(preamble) == 0) {
    preamble = "lab";
  }

  // Optional: enforce a max preamble length for FAT compatibility
  const size_t MAX_PREAMBLE_LEN = 24;

  char safePreamble[MAX_PREAMBLE_LEN + 1];
  strncpy(safePreamble, preamble, MAX_PREAMBLE_LEN);
  safePreamble[MAX_PREAMBLE_LEN] = '\0';

  // Find next available file number
  do {
    snprintf(filename, sizeof(filename),
             "%s_data%03d.csv",
             safePreamble,
             fileNumber);
    fileNumber++;

  } while (sd.exists(filename) && fileNumber <= 999);

  if (fileNumber > 999) {
    Xbee.println("[ERROR] Maximum file number exceeded (999).");
    return false;
  }

  Xbee.print("[INFO] Creating file: ");
  Xbee.println(filename);

  *dataFile = sd.open(filename, FILE_WRITE);

  if (!*dataFile) {
    Xbee.println("[ERROR] could not create file.");
    return false;
  }

  return true;
}



/**
 * @brief Recursively lists all files and directories on the SD card.
 * 
 * Opens the given directory, prints file and folder names (with indentation
 * for hierarchy), and displays file sizes. Calls itself recursively for
 * subdirectories.
 * 
 * @param dir   The directory to list (use SD.open("/") for root).
 * @param depth Indentation level for nested directories (start with 0).
 * @return void
 */

void sd_listFiles(String dirName, int depth)
{
  Xbee.print("[INFO] Listing contents of: ");
  Xbee.println(dirName);
  FsFile dir = sd.open(dirName.c_str());
  if (!dir) {
    Xbee.println("[ERROR] Could not open directory.");
    return;
  } // end if
  
  while (true)
  {
    FsFile entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      if (depth == 0) {
        Xbee.println("[INFO] Listing SD contents complete.");
      }
      break;
    }

    // indent for clarity
    for (int i = 0; i < depth; i++)
    {
      Xbee.print("  ");
    }

    char tempName[40];
    entry.getName(tempName, sizeof(tempName));
    Xbee.print(tempName);
    
    if (entry.isDirectory())
    {
      Xbee.println("/");
      // Build full path for subdirectory
      String fullPath = dirName;
      if (!dirName.endsWith("/")) {
        fullPath += "/";
      }
      fullPath += tempName;
      sd_listFiles(fullPath, depth + 1); // recurse with full path
    }
    else
    {
      // files have sizes, directories do not
      Xbee.print("\t\t");
      Xbee.print(entry.size(), DEC);
      Xbee.println(" bytes");
    }
    entry.close();
  }
  dir.close();
} // end function sd_listFiles()

/**
 * @brief Lists files on the SD card with numbers and prompts user to choose one to print.
 * 
 * Scans the root directory, prints files with an index number,
 * and waits for user input of the file number.
 * 
 * @return void
 */
void sd_printFileMenu(const String& path) {
  FsFile directory = sd.open(path);
  
  if (!directory) {
    Xbee.println("[ERROR] Could not open directory.");
    return;
  }
  
  const int MAX_FILES = 50;        // Reduced for better memory management
  String fileList[MAX_FILES];      // store filenames
  int fileCount = 0;
  Xbee.println("[INFO] Files on SD card:");
  
  while (true) {
    FsFile entry = directory.openNextFile();
    if (!entry) break;  // no more files

    if (!entry.isDirectory() && fileCount < MAX_FILES) {
      char tempName[64];  // Increased buffer size for longer filenames
      entry.getName(tempName, sizeof(tempName));
      
      // Ensure null termination
      tempName[sizeof(tempName) - 1] = '\0';
      
      fileList[fileCount] = String(tempName);
      Xbee.print("(");
      Xbee.print(fileCount + 1);
      Xbee.print("): ");
      Xbee.print(fileList[fileCount]);
      Xbee.print(" (");
      Xbee.print(entry.size());
      Xbee.println(" bytes)");
      fileCount++;
    } else if (!entry.isDirectory() && fileCount >= MAX_FILES) {
      Xbee.println("[CAUTION] More files exist but only showing first 50.");
      entry.close();
      break;
    }
    entry.close();
  }

  directory.close();

  if (fileCount > 0) {
    Xbee.println("[REQUEST] Enter the file number to print.");
    
    // Clear any existing serial input buffer
    while (Xbee.available()) {
      Xbee.read();
    }
    
    // Wait for user input with timeout
    unsigned long timeout = millis() + 30000; // 30 second timeout
    while (!Xbee.available() && millis() < timeout) {
      delay(10);
    }
    
    if (millis() >= timeout) {
      Xbee.println("[CAUTION] Input timeout, returning to Menu.");
      return;
    }
    
    int choice = Xbee.parseInt();   // read number user typed


    
    // Clear remaining characters in buffer
    while (Xbee.available()) {
      Xbee.read();
    }
    
    if (choice > 0 && choice <= fileCount) {
      Xbee.print("[INFO] You picked file #");
      Xbee.println(choice);
      sd_printFile(fileList[choice - 1].c_str());
    } else {
      Xbee.println("[INFO] Invalid choice, returning to Menu.");
    }
  } else {
    Xbee.println("[CAUTION] No files found on SD card.");
  }
}

/**
 * @brief Prints the contents of a selected file to Xbee.
 * 
 * Opens the file in read mode and sends its contents over Xbee.
 * 
 * @param filename Name of the file to print.
 * @return void
 */
void sd_printFile(const char *filename) {
  FsFile file = sd.open(filename);

  if (!file) {
    Xbee.print("[ERROR] Error opening file: ");
    Xbee.println(filename);
    return;
  }

  // Check file size - warn if very large
  uint32_t fileSize = file.size();
  if (fileSize > 10000) { // 10KB threshold
    Xbee.print("[CAUTION] File is large (");
    Xbee.print(fileSize);
    Xbee.println(" bytes). This may take a while to print.");
  }

  Xbee.print("[INFO] ---- Contents of ");
  Xbee.print(filename);
  Xbee.print(" (");
  Xbee.print(fileSize);
  Xbee.println(" bytes) ----\n");

  // Print file contents with periodic yield for system stability
  uint32_t bytesRead = 0;
  while (file.available()) {
    Xbee.write(file.read());
    bytesRead++;
    
    // Yield to system every 100 bytes to prevent watchdog issues
    if (bytesRead % 100 == 0) {
      yield();
    }
  }

  file.close();
  Xbee.println("\n[INFO] ---- End of file ----");
}

