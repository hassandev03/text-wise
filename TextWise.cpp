#include <iostream>
#include <fstream>
#include <string>
#include "stack.cpp"
#include "huffmanCoder.cpp"
using namespace std;

void mainMenu();
void editorWindow();

class TextWiseFileManager
{

private:
    Stack undoStack, redoStack;
    unordered_map<string, int> wordCount;
    unordered_map<char, int> charCount;

    string fileContent = "", fileName;

    string toLowerCase(string text)
    {
        string newText = "";
        for (int i = 0; i < text.length(); i++)
        {
            if (text[i] >= 'A' && text[i] <= 'Z')
                newText += (char)(text[i] + 32);
            else
                newText += text[i];
        }
        return newText;
    }
    

public:
    // file utilities
    void resetContents();
    void setFileContent(string content);
    bool isValidFileName(string fileName, string ext);
    string getFileName(string prompt, string ext);
    string getDirectoryName(string prompt);
    void undo();
    void redo();
    void readFile();
    void replaceText(string oldText, string newText);
    void replaceWord(string oldWord, string newWord);
    void searchText(string text);
    void searchWord(string word);
    long countCharacterOccurance(char character);
    long countCharacters();
    long countWords();
    void appendText(string text);
    void openFromDisk(string fileName);
    void saveToDisk(string fileName);
    void saveFile();
    void openFile();
    string getfileContent();

    // file compression/encoding
    void openCompressedEncryptedFile();
    void compressEncryptFile(string check);
    void decompressDecryptFile(string check);
};

string TextWiseFileManager::getfileContent()
{
    return fileContent;
}
void TextWiseFileManager::setFileContent(string content)
{
    fileContent = content;
}

void TextWiseFileManager::resetContents()
{
    fileContent = "";
    wordCount.clear();
    charCount.clear();
}

bool TextWiseFileManager::isValidFileName(string fileName, string ext)
{
    int pos = fileName.find_last_of(".");
    if (pos != string::npos)
    {
        if (fileName.substr(pos + 1) == ext)
            return true;
        else
            return false;
    }
    else
        return false;
}

string TextWiseFileManager::getFileName(string prompt, string ext)
{
    string fileName;
    while (true)
    {
        cout << prompt << ": ";
        getline(cin, fileName);
        if (isValidFileName(fileName, ext))
            break;
        else
            cout << "Invalid file name. Please try again.\n";
    }
    return fileName;
}

string TextWiseFileManager::getDirectoryName(string prompt)
{
    string directName;
    cout << prompt << ": ";
    getline(cin, directName);

    if (directName[directName.length() - 1] != '/')
        directName += "\\";

    return directName;
}

void TextWiseFileManager::openFromDisk(string filename)
{
    this->fileName = filename;
    ifstream file(filename);
    if (file.is_open())
    {
        string line, word = "";
        while (getline(file, line))
        {
            fileContent += line + "\n";

            // Update character count
            for (size_t i = 0; i < line.size(); i++)
                charCount[line[i]]++;

            charCount['\n']++;

            // Process the line to count words
            line += ' '; // Add a space at the end to handle the last word
            for (char c : line)
            {
                if (isalpha(c))
                {
                    word += tolower(c);
                }
                else
                {
                    if (!word.empty())
                    {
                        wordCount[word]++;
                        word = "";
                    }
                }
            }
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file " << filename << ".\n";
    }
}

void TextWiseFileManager::saveToDisk(string filename)
{

    ofstream file(filename);
    if (file.is_open())
    {
        file << fileContent;
        file.close();
        cout << "File " << filename << " saved successfully.\n";
    }
    else
    {
        cout << "Unable to save file " << filename << ".\n";
    }
}

void TextWiseFileManager::readFile()
{
    cout << fileContent;
}

void TextWiseFileManager::searchWord(string word)
{
    word = toLowerCase(word);
    if (wordCount.find(word) != 0)
        cout << "Word " << word << " found in text " << wordCount[word] << " times.\n";
    else
        cout << "Word " << word << " not found.\n";
}

void TextWiseFileManager::searchText(string text)
{
    size_t pos = fileContent.find(text);
    if (pos != string::npos)
        cout << "Text exists in the file.\n";
    else
        cout << "Text not found in the file.\n";
}

void TextWiseFileManager::replaceText(string oldText, string newText)
{
    size_t pos = fileContent.find(oldText);
    if (pos != string::npos)
    {
        undoStack.push(fileContent);
        // starting postion, length of text to be replaced, new text
        fileContent.replace(pos, oldText.length(), newText);
    }

    else
        cout << "Text not found in the file.\n";
}

void TextWiseFileManager::replaceWord(string oldWord, string newWord)
{
    if (oldWord.empty() || newWord.empty())
    {
        // Handle empty oldWord or newWord cases (optional)
        cerr << "Error: Old or new word cannot be empty.\n";
        return;
    }

    // Push current content to undo stack for potential undo
    undoStack.push(fileContent);
    redoStack.clear(); // Clear redo stack when replacing a word

    // Perform the replacement
    size_t start = 0;
    while ((start = fileContent.find(oldWord, start)) != string::npos)
    {
        fileContent.replace(start, oldWord.length(), newWord);
        start += newWord.length(); // Advance start position after replacement
        wordCount[newWord]++;
    }

    // delete old word
    wordCount.erase(oldWord);
}

long TextWiseFileManager::countCharacterOccurance(char character)
{
    long count = 0;
    if (charCount.count(character) > 0)
        count = charCount[character];
    else
        cout << "Character not found.\n";

    return count;
}

long TextWiseFileManager::countCharacters()
{
    int count = 0;
    for (auto p : charCount)
        count += p.second;
    return count;
}

long TextWiseFileManager::countWords()
{
    int count = 0;
    for (auto p : wordCount)
        count += p.second;

    return count;
}

void TextWiseFileManager::undo()
{
    if (undoStack.isEmpty())
    {
        cout << "Nothing to undo.\n";
        return;
    }
    redoStack.push(fileContent);
    fileContent = undoStack.pop();
}

void TextWiseFileManager::redo()
{
    if (redoStack.isEmpty())
    {
        cout << "Nothing to redo.\n";
        return;
    }
    undoStack.push(fileContent);
    fileContent = redoStack.pop();
}

void TextWiseFileManager::appendText(string text)
{
    undoStack.push(fileContent);
    string word = "";
    for (int i = 0; i < text.length(); i++)
    {
        if ((text[i] >= 'A' && text[i] <= 'Z') || (text[i] >= 'a' && text[i] <= 'z'))
            word = word + text[i];
        else
        {
            wordCount[word]++;
            word = "";
        }
    }
    fileContent += text;
}

void TextWiseFileManager::openFile()
{
    string fileName = getFileName("Enter a file name with location to open", "txt");
    if (isValidFileName(fileName, "txt"))
        openFromDisk(fileName);

    else
        cout << "Invalid file name. Please try again.\n";
}

void TextWiseFileManager::saveFile()
{
    string fileName = getDirectoryName("Enter a file location to save");
    fileName += getFileName("Enter file name", "txt");
    saveToDisk(fileName);
}

void TextWiseFileManager::openCompressedEncryptedFile()
{
    HuffmanCoder hc;
    string coded, hashTable;
    coded = getFileName("Enter encoded file name with location to open", "bin");
    hashTable = getFileName("Enter hash file name with location to open", "hash");
    setFileContent(hc.decode(coded, hashTable));
}

void TextWiseFileManager::compressEncryptFile(string check)
{
    HuffmanCoder hc;
    string filename, compCodeFileName, hashFileName, code;
    if (check == "open and encode")
        filename = getFileName("Enter a file name with location to compress/encrypt", "txt");
    else
        filename = this->fileName;

    code = hc.encode(filename);

    compCodeFileName = getDirectoryName("Enter a file location to save compressed/encrypted file");
    compCodeFileName += getFileName("Enter file name", "bin");
    hashFileName = compCodeFileName.substr(0, compCodeFileName.find_last_of(".")) + ".hash";

    hc.saveEncoded(code, compCodeFileName, hashFileName);
}

void TextWiseFileManager::decompressDecryptFile(string check)
{
    HuffmanCoder hc;
    string decompFileName;

    if (check == "open and decode")
        openCompressedEncryptedFile();

    decompFileName = getDirectoryName("Enter a file location to save decompressed/decrypted file");
    decompFileName += getFileName("Enter file name", "txt");
    saveToDisk(decompFileName);

    if (check == "open and decode")
        resetContents();
}

TextWiseFileManager tfm;

void editorWindow()
{
    char choice;
    do
    {
        if (!(tfm.getfileContent().empty()))
        {
            cout << "\n------------------------------------\n";
            tfm.readFile();
            cout << "\n------------------------------------\n";
            cout << "Word Count: " << tfm.countWords() << endl;
            cout << "Character Count: " << tfm.countCharacters();
        }
        else
            mainMenu();

        cout << "\n------------------------------------\n";
        cout << "| 1. Undo                          | \n";
        cout << "| 2. Redo                          | \n";
        cout << "| 3. Append Text                   | \n";
        cout << "| 4. Replace Text                  | \n";
        cout << "| 5. Replace Word                  | \n";
        cout << "| 6. Search Text                   | \n";
        cout << "| 7. Search Word                   | \n";
        cout << "| 8. Count Character Occurrences   | \n";
        cout << "| 9. Save File                     | \n";
        cout << "| 0. Save Compressed/Encrypted     | \n";
        cout << "| (r) Return to main menu          | \n";
        cout << "| (q) Quit                         | \n";
        cout << "------------------------------------\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // consume any extra characters in the input buffer
        char ch;
        while ((ch = cin.get()) != '\n')
            ;

        switch (choice)
        {
        case '1':
        {
            tfm.undo();
            break;
        }
        case '2':
        {
            tfm.redo();
            break;
        }
        case '3':
        {
            string text;
            tfm.readFile();
            getline(cin, text);
            tfm.appendText(text);
            break;
        }
        case '4':
        {
            string oldText, newText;
            cout << "Enter the old text to replace: ";
            getline(cin, oldText);
            cout << "Enter the new text: ";
            getline(cin, newText);
            tfm.replaceText(oldText, newText);
            break;
        }
        case '5':
        {
            string oldWord, newWord;
            cout << "Enter the word to replace: ";
            getline(cin, oldWord);
            cout << "Enter the new word: ";
            getline(cin, newWord);
            tfm.replaceWord(oldWord, newWord);
            break;
        }
        case '6':
        {
            string text;
            cout << "Enter the text to search: ";
            getline(cin, text);
            tfm.searchText(text);
            break;
        }
        case '7':
        {
            string word;
            cout << "Enter the word to search: ";
            getline(cin, word);
            tfm.searchWord(word);
            break;
        }
        case '8':
        {
            char character;
            cout << "Enter the character to count: ";
            cin >> character;
            cout << "Number of occurrences: " << tfm.countCharacterOccurance(character) << endl;
            break;
        }
        case '9':
        {
            tfm.saveFile();
            break;
        }
        case '0':
        {
            tfm.compressEncryptFile("encode");
            break;
        }
        case 'r':
            tfm.resetContents();
            mainMenu();
            break;
        case 'q':
            cout << "Exiting TextWise..." << endl;
            exit(0);
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 'q');
}

void mainMenu()
{
    char choice;
    do
    {
        cout << "\n\n=======================================\n";
        cout << "|              TextWise               |\n";
        cout << "=======================================\n";
        cout << "| 1. Open File                        |\n";
        cout << "| 2. Open Compressed/Encrypted File   |\n";
        cout << "| 3. Compress/Encrypt File            |\n";
        cout << "| 4. Decompress/Decrypt File          |\n";
        cout << "| (q) Quit                            |\n";
        cout << "=======================================\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // consume any extra characters in the input buffer
        char ch;
        while ((ch = cin.get()) != '\n')
            ;

        switch (choice)
        {
        case '1':
            tfm.openFile();
            editorWindow();
            break;
        case '2':
            tfm.openCompressedEncryptedFile();
            break;
        case '3':
            tfm.compressEncryptFile("open and encode");
            break;
        case '4':
            tfm.decompressDecryptFile("open and decode");
            break;
        case 'q':
            cout << "Exiting TextWise..." << endl;
            exit(0);
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 'q');
}

int main()
{
    mainMenu();
    return 0;
}
