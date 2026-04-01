#pragma once

/*---------------------------------------------------------------------------------------------*/
// Function Prototypes (see defintiions below):
/*---------------------------------------------------------------------------------------------*/
bool sd_createDataFile(File *dataFile);
void sd_listFiles(File dir, int depth);
void sd_printFileMenu();
void sd_printFile(const char *filename);

/*---------------------------------------------------------------------------------------------*/
// Function Definitions:
/*---------------------------------------------------------------------------------------------*/
/**
 * @brief Creates a new data file on the SD card using timestamp from RTC.
 * 
 * Generates a filename of the form "data_YYYYMMDD_HHMMSS.csv" and opens it
 * for writing. If successful, the file pointer is updated and the function
 * returns true. Otherwise, returns false.
 * 
 * @param rtc       Reference to RTC_PCF8523 object (must be initialized).
 * @param dataFile  Pointer to a File object that will be opened.
 * @return true     If the file was successfully created and opened.
 * @return false    If file creation/opening failed.
 */
bool sd_createDataFile(File *dataFile) {

  // Build filename, e.g., "DATA_2025-09-01_093015.CSV"
  char filename[32];
  snprintf(filename, sizeof(filename),
           "data_Lab7.csv");

  Serial.print("[INFO] Creating file: ");
  Serial.print(filename);

  *dataFile = SD.open(filename, FILE_WRITE);
  if (!*dataFile) {
    Serial.println("[ERROR] could not create file.");
    return false;
  } else {
    Serial.println(" --> Created file.");
  }

  // Optional: write header row
  dataFile->println("mcu time(ms),gyro_Z(deg/s),mag_X(uT),mag_Y(uT),sun_direction(deg),sun_plusX(count),sun_plusY(count),sun_minusX(count),sun_minusY(count),w_RW_cmd(RPM),w_RW_meas(RPM)");
  dataFile->flush();
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
void sd_listFiles(File dir, int depth) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    // indent for clarity
    for (int i = 0; i < depth; i++) {
      Serial.print("  ");
    }

    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      sd_listFiles(entry, depth + 1);  // recurse into subdirectory
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      Serial.println(" bytes");
    }
    entry.close();
  }
}

/**
 * @brief Lists files on the SD card with numbers and prompts user to choose one to print.
 * 
 * Scans the root directory, prints files with an index number,
 * and waits for user input of the file number.
 * 
 * @return void
 */
void sd_printFileMenu() {
  File root = SD.open("/");
  int MAX_FILES = 1000;        // max number of files to handle
  String fileList[MAX_FILES];      // store filenames
  int fileCount = 0;
  Serial.println("[INFO] Files on SD card:");
  
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;  // no more files

    if (!entry.isDirectory() && fileCount < MAX_FILES) {
      fileList[fileCount] = entry.name();
      Serial.print("(");
      Serial.print(fileCount + 1);
      Serial.print("): ");
      Serial.println(fileList[fileCount]);
      fileCount++;
    }
    entry.close();
  }

  root.close();

  if (fileCount > 0) {
    Serial.println("[REQUEST] Enter the file number to print.");
  } else {
    Serial.println("[CAUTION] No files found on SD card.");
  }

  while (!Serial.available()){delay(10);}; // Wait for user input

  int choice = Serial.parseInt();   // read number user typed
  if (choice > 0 && choice <= fileCount) {
    Serial.print("[INFO] You picked file #");
    Serial.println(choice);
    sd_printFile(fileList[choice - 1].c_str());
  } else {
    Serial.println("[INFO] Invalid choice, returning to Menu.");
  }
}

/**
 * @brief Prints the contents of a selected file to Serial.
 * 
 * Opens the file in read mode and sends its contents over Serial.
 * 
 * @param filename Name of the file to print.
 * @return void
 */
void sd_printFile(const char *filename) {
  File file = SD.open(filename);

  if (!file) {
    Serial.print("[ERROR] Error opening file: ");
    Serial.println(filename);
    return;
  }

  Serial.print("[INFO] ---- Contents of ");
  Serial.print(filename);
  Serial.println(" ----\n");

  while (file.available()) {
    Serial.write(file.read());
  }

  file.close();
  Serial.println("\n[INFO] ---- End of file ----");
}
