#include "zmodem.h"

void help(void)
{
  ASERIAL.print(Progname);
  ASERIAL.print(F(" - Transfer rate: ")); ASERIAL.flush();
  ASERIAL.println(ZMODEM_SPEED); ASERIAL.flush();
  ASERIAL.println(F("Available Commands:")); ASERIAL.flush();
  ASERIAL.println(F("HELP     - Print this list of commands")); ASERIAL.flush();
  ASERIAL.println(F("DIR      - List files in current working directory - alternate LS")); ASERIAL.flush();
  ASERIAL.println(F("PWD      - Print current working directory")); ASERIAL.flush();
  ASERIAL.println(F("CD       - Change current working directory")); ASERIAL.flush();
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_FILE_MGR
  ASERIAL.println(F("DEL file - Delete file - alternate RM")); ASERIAL.flush();
  ASERIAL.println(F("MD  dir  - Create dir - alternate MKDIR")); ASERIAL.flush();
  ASERIAL.println(F("RD  dir  - Delete dir - alternate RMDIR")); ASERIAL.flush();
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_SZ
  ASERIAL.println(F("SZ  file - Send file from Arduino to terminal (* = all files)")); ASERIAL.flush();
#endif
#ifdef ARDUINO_SMALL_MEMORY_INCLUDE_RZ
  ASERIAL.println(F("RZ       - Receive a file from terminal to Arduino (Hyperterminal sends this")); ASERIAL.flush();
  ASERIAL.println(F("              automatically when you select Transfer->Send File...)")); ASERIAL.flush();
#endif
} // end function help()

int count_files(int *file_count, long *byte_count)
{
  *file_count = 0;
  *byte_count = 0;

  zdir->openCwd();
  zdir->rewindDirectory();

  while (zfile_pointer->openNext(zdir)) {
    // read next directory entry in current working directory

    if (!zfile_pointer->isDir()) {
      *file_count = *file_count + 1;
      *byte_count = *byte_count + zfile_pointer->fileSize();
    }

    zfile_pointer->close();
  }
  zdir->close();
  return 0;
}

/// @brief lists the contents of the current directory
void directory_listing() {
  ASERIAL.println("\nfilename / filesize / (number)");

  zdir->openCwd();
  zdir->rewindDirectory();

  int filenum = 1;
  while (zfile_pointer->openNext(zdir)) {
    // read next directory entry in current working directory

    // format file name
    zfile_pointer->getName(zfile_name, 64);

    ASERIAL.print(zfile_name); ASERIAL.flush();
    for (uint8_t i = 0; i < 50 - strlen(zfile_name); ++i) ASERIAL.print(F(" "));
    if (!(zfile_pointer->isDir())) {
      // ugh, this section uses 'zfile_name' to store the file size
      ultoa(zfile_pointer->fileSize(), zfile_name, 10);
      ASERIAL.print(zfile_name);

      ASERIAL.print("    ("); ASERIAL.print(filenum); ASERIAL.print(") \n");
      ASERIAL.flush();
    } else {
      ASERIAL.println(F("DIR\n"));
    }
    filenum++;
    ASERIAL.flush();
    zfile_pointer->close();
  }
  ASERIAL.println(F("End of Directory"));
}

/// @brief prints location of current directory
void print_working_directory() {
  zdir->openCwd();
  zdir->getName(zfile_name, 256);
  zdir->close();
  ASERIAL.print(F("Current working directory is "));
  ASERIAL.flush(); ASERIAL.println(zfile_name); ASERIAL.flush();
} // end function print_working_directory()

/// @brief change directory
/// @param param name of directory to change to
void change_directory(const String& param) {
  if(!sd.chdir(param)) {
    ASERIAL.print(F("Directory "));
    ASERIAL.flush(); ASERIAL.print(param); ASERIAL.flush();
    ASERIAL.println(F(" not found"));
  } else {
    ASERIAL.print(F("Current directory changed to "));
    ASERIAL.flush(); ASERIAL.println(param); ASERIAL.flush();
  }
} // end function change_directory()

/// @brief remove single file
/// @param param name of file to be deleted
void remove_file(String param) {
  if (!sd.remove(param)) {
    ASERIAL.print(F("Failed to delete file "));
    ASERIAL.flush(); ASERIAL.println(param); ASERIAL.flush();
  } else {
    ASERIAL.print(F("File "));
    ASERIAL.flush(); ASERIAL.print(param); ASERIAL.flush();
    ASERIAL.println(F(" deleted"));
  }
} // end function remove_file()

/// @brief make directory
/// @param param name of new directory
void mkdir(String param) {
  if (!sd.mkdir(param, true)) {
    ASERIAL.print(F("Failed to create directory "));
    ASERIAL.flush(); ASERIAL.println(param); ASERIAL.flush();
  } else {
    ASERIAL.print(F("Directory "));
    ASERIAL.flush(); ASERIAL.print(param); ASERIAL.flush();
    ASERIAL.println(F(" created"));
  }
} // end function mkdir()

/// @brief remove directory
/// @param param name of directory to be removed
void remove_directory(String param) {
  if (!sd.rmdir(param)) {
    ASERIAL.print(F("Failed to remove directory "));
    ASERIAL.flush(); ASERIAL.println(param); ASERIAL.flush();
  } else {
    ASERIAL.print(F("Directory "));
    ASERIAL.flush(); ASERIAL.print(param); ASERIAL.flush();
    ASERIAL.println(F(" removed"));
  }
} // end function remove_directory()

void zmodem_send_file(char * param) {
  if (!strcmp_P(param, PSTR("*"))) {
    count_files(&Filesleft, &Totalleft);
    strcmp(param, PSTR("*"));
    if (Filesleft > 0) {
      ZSERIAL.print(F("rz\n"));
      ZSERIAL.flush();

      sendzrqinit();
      delay(200);

      // Cannot use the "shared 1K memory" block with the latest SDFat because the file transfer will corrupt the directory object.
      FsFile dirsz;

      dirsz.openCwd();
      dirsz.rewindDirectory();

      while (fout.openNext(&dirsz)) {
        // read next directory entry in current working directory

        // open file
        fout.getName(zfile_name, 256);
        //if (!fout.open(zfile_name, O_READ)) error(F("file.open failed"));

        //else
        if (!fout.isDir()) {

          if (wcs(zfile_name) == ERROR) {
            delay(500);
            fout.close();
            break;
          }
          else delay(500);
        }
        fout.close();

      }

      dirsz.close();

      saybibi();
    } else {
      ASERIAL.println(F("No files found to send"));
    }
  } else if (!fout.open(param, O_READ)) {
    ASERIAL.println(F("file.open failed"));
  } else {
    // Start the ZMODEM transfer
    Filesleft = 1;
    Totalleft = fout.fileSize();
    ZSERIAL.print(F("rz\n"));
    ZSERIAL.flush();
    sendzrqinit();
    delay(200);
    wcs(param);
    saybibi();
    fout.close();
  }
} // end zmodem_send_file()

void zmodem_receive_file() {
  ASERIAL.println(F("Receiving file..."));
  if (wcreceive(0, 0)) {
    ASERIAL.println(F("zmodem transfer failed"));
  } else {
    ASERIAL.println(F("zmodem transfer successful"));
  }
  //fout.flush();
  fout.sync();
  fout.close();
} // end function zmodem_receive_file()

