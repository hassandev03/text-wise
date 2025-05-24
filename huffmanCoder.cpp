#include <unordered_map>
#include <fstream>
#include "min_heap.cpp"
using namespace ::std;

class HuffmanCoder
{
private:
    unordered_map<char, string> encoder;
    unordered_map<string, char> decoder;

    int compareNodes(Node *a, Node *b)
    {
        return a->freq - b->freq;
    }

    void initEncoderDecoder(Node *node, string osf)
    {
        if (node == nullptr)
            return;

        // the leaf nodes contain the characters and code
        if (node->left == nullptr && node->right == nullptr)
        {
            encoder[node->data] = osf;
            decoder[osf] = node->data;
        }

        // whenever going left, add '0' to outputSoFar(osf)
        initEncoderDecoder(node->left, osf + '0');

        // whenever going right, add '1' to outputSoFar(osf)
        initEncoderDecoder(node->right, osf + '1');
    }

    void saveHuffmanCode(string huffmanCode, string filename)
    {
        ofstream file(filename, ios::out | ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return;
        }

        size_t numBits = huffmanCode.length();
        size_t numBytes = (numBits + 7) / 8;

        vector<bool> bitVector;
        bitVector.reserve(numBits); // Reserve space for the bits

        // Convert Huffman code to bit vector
        for (char bit : huffmanCode)
            bitVector.push_back(bit == '1');

        // Create a buffer to hold the binary data
        vector<unsigned char> buffer(numBytes, 0);

        // Convert the bit vector to bytes
        for (size_t i = 0; i < numBits; ++i)
        {
            if (bitVector[i])
                buffer[i / 8] |= (1 << (7 - (i % 8)));
        }

        // Write the buffer to the file
        file.write(reinterpret_cast<char *>(buffer.data()), numBytes);

        // Close the file
        file.close();
    }

    void saveHashTable(unordered_map<string, char> &hashTable, string filename)
    {
        ofstream hashTableFile(filename, ios::out | ios::binary);

        if (hashTableFile.is_open())
        {
            size_t size = hashTable.size();
            // write the size of the hashTable
            hashTableFile.write((char *)&size, sizeof(size));
            size_t keySize;
            for (const auto &p : hashTable)
            {
                // write the key
                keySize = p.first.size();
                hashTableFile.write(reinterpret_cast<const char *>(&keySize), sizeof(keySize));
                hashTableFile.write(p.first.c_str(), keySize);

                // write the value
                hashTableFile.write(reinterpret_cast<const char *>(&p.second), sizeof(p.second));
            }
        }
        else
            cout << "Error opening hashTable file" << endl;

        hashTableFile.close();
    }

    string readFile(const string &filename)
    {
        ifstream file(filename);
        string encodedText = "";
        if (file.is_open())
        {
            while (!file.eof())
            {
                string line;
                getline(file, line);
                encodedText += line + "\n";
            }
        }
        else
        {
            cerr << "Unable to open file " << filename << ".\n";
            return "";
        }
        return encodedText;
    }

    void loadHashTable(string codeFileName)
    {
        unordered_map<string, char> hashTable;
        ifstream hashTableFile(codeFileName, ios::in | ios::binary);
        if (!hashTableFile.is_open())
        {
            cerr << "Error opening hashTable file" << endl;
            return;
        }

        size_t size;
        // read th size of the hashTable
        hashTableFile.read((char *)&size, sizeof(size));

        size_t keySize;
        char value;
        for (size_t i = 0; i < size; i++)
        {
            // read the key size
            hashTableFile.read((char *)&keySize, sizeof(keySize));
            string key(keySize, ' ');

            // read the key
            hashTableFile.read(&key[0], keySize);
            // read the value
            hashTableFile.read((char *)&value, sizeof(value));
            // insert the key value pair into the hashTable
            hashTable[key] = value;
        }
        decoder = hashTable;
    }

    string readEncodedFile(string filename)
    {
        ifstream file(filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file for reading: " << filename << endl;
            return "";
        }

        // Determine the file size
        file.seekg(0, ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, ios::beg);

        // Read binary data into a buffer
        char *buffer = new char[fileSize];
        file.read(buffer, fileSize);

        // Convert binary data back to Huffman codes string
        string huffmanCodes;
        for (size_t i = 0; i < fileSize; ++i)
        {
            for (int j = 7; j >= 0; --j)
                huffmanCodes += ((buffer[i] & (1 << j)) ? '1' : '0');
        }

        // Clean up
        delete[] buffer;
        file.close();

        return huffmanCodes;
    }

public:
    HuffmanCoder()
    {
    }

    unordered_map<string, char> getDecoder()
    {
        return decoder;
    }

    string encode(string origFileName)
    {
        string source = readFile(origFileName);
        // creating a frequency map
        unordered_map<char, int> freqMap;

        for (char c : source)
            freqMap[c]++;

        // create min heap and insert nodes
        MinHeap minHeap;
        for (const pair<char, int> &p : freqMap)
        {
            Node *temp = new Node(p.first, p.second);
            minHeap.insert(temp);
        }

        while (minHeap.size() != 1)
        {
            Node *first = minHeap.remove();
            Node *second = minHeap.remove();

            // join two nodes and assign left right of newNode appropriately
            Node *newNode = new Node('\0', first->freq + second->freq);
            newNode->left = first, newNode->right = second;

            // insert newNode into the heap
            minHeap.insert(newNode);
        }

        Node *fullTreeNode = minHeap.remove();
        initEncoderDecoder(fullTreeNode, "");

        string result = "";
        for (int i = 0; i < source.size(); i++)
        {
            result += encoder[source[i]];
        }
        return result;
    }

    string decode(string codeFileName, string hashTableFileName)
    {
        loadHashTable(hashTableFileName);
        // string encodedText = readFile(codeFileName);
        string encodedText = readEncodedFile(codeFileName);
        string result = "", key = "";
        for (int i = 0; i < encodedText.size(); i++)
        {
            key += encodedText[i];
            if (decoder.count(key) != 0)
            {
                result += decoder[key];
                key = "";
            }
        }
        return result;
    }

    void saveEncoded(string code, string codeFileName, string hashTableFileName)
    {
        saveHuffmanCode(code, codeFileName);
        saveHashTable(decoder, hashTableFileName);
    }
};
