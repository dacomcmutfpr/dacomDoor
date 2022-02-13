#include "SPIFFS.h"
#include <FS.h>
#include <MFRC522.h>
#include <sqlite3.h>

class FileHandler
{
protected:
  bool _isReady = false;
  File file;
  char fileMode[3] = {0};

public:
  bool isReady()
  {
    return this->_isReady;
  }

  void openFile(String path)
  {
    Serial.print(F("Trying open: "));
    Serial.println(path);

    if (this->_isReady)
    {
      Serial.println(F("File already openned!"));
    }
    else
    {
      File file;
      file = SPIFFS.open(path, this->fileMode);
      this->file = file;
      this->_isReady = true;
    }
  }

  virtual void run() = 0;
};

class FileReader : public FileHandler
{
public:
  FileReader()
  {
    this->fileMode[0] = 'r';
    this->fileMode[1] = '+';
  }

  void run()
  {
    char ch;
    if (this->_isReady)
    {
      while (file.available() && (ch = file.read()) > 0)
      {
        Serial.print(ch);
      }
      Serial.println("");
      this->file.close();
      this->_isReady = false;
    }
  }
};

class FileWriter : public FileHandler
{
private:
  bool running = true;
  char delimiter = 168;

public:
  FileWriter()
  {
    this->fileMode[0] = 'w';
    this->fileMode[1] = '+';
  }

  void run()
  {
    char ch;
    if (this->_isReady)
    {
      while (this->running && Serial.available())
      {
        ch = Serial.read();
        if (ch == this->delimiter)
        {
          Serial.println("File closed!");
          this->file.close();
          this->running = false;
          this->_isReady = false;
        }
        else
        {
          file.print(String(ch));
        }
      }
    }
  }
};

FileReader reader;
FileWriter writer;
FileHandler *fileHandler = NULL;

int error = 0;
int state = 0;
String readingInput;
bool isReadingInput = false;

void setup()
{
  if (!SPIFFS.begin(true))
  {
    error = 1;
    Serial.println("[ERROR]: Failed to mount file system");
  }

  // Wait a time to Arduino Serial Monitor opens
  Serial.begin(115200);
  delay(1000);
}

void loop()
{
  char ch;

  if (error)
    return;

  if (isReadingInput)
  {
    if (Serial.available())
    {
      ch = Serial.read();
      Serial.print(ch);

      if (ch == '\n')
        isReadingInput = false;
      else
        readingInput += ch;
    }
  }
  else if (state != 3)
  {
    if (state == 0)
    {
      Serial.println(F("    Choose:"));
      Serial.println(F("(1) Read"));
      Serial.println(F("(2) Write"));
      state = 1;
      isReadingInput = true;
    }
    else if (state == 1)
    {
      if (readingInput[0] == '1')
        fileHandler = &reader;
      else if (readingInput[0] == '2')
        fileHandler = &writer;

      if (fileHandler != NULL)
      {
        Serial.print(F("Filepath?\n >>> "));

        state = 2;
        isReadingInput = true;
      }
      else
      {
        Serial.println(F("Please, choose one of the options!"));
        state = 0;
      }

      readingInput.clear();
    }
    else if (state == 2)
    {
      if (fileHandler != NULL)
        fileHandler->openFile(readingInput);
      else
        Serial.println("[ERROR]: Something wrong happened!");

      state = 3;
      readingInput.clear();
    }
  }
  else
  {
    if (fileHandler->isReady())
    {
      fileHandler->run();
    }
    else
    {
      fileHandler = NULL;
      state = 0;
    }
  }
}
