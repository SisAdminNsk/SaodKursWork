#include <cstdio>
#include <conio.h>
#include <Windows.h>
#include <algorithm>
#include "Queue.h"
#include "heapSort.h"
#include "recordStruct.h"
#include "Bsearch.h"
#include "tree.h"
#include "EncodingDecoding.h"

void showDb(Record* db,const int recordsCount,const int partSize = 20);
void showDb(Record** db, const int recordsCount, const int partSize = 20);
std::string CallInput();
void showQueue(const Queue<Record*>& queue,const std::pair<size_t,size_t>& interval);
std::pair<size_t, size_t> findRecords(Record** db,const int recordsCount,const std::string& key);
void fillRecordsQueue(Record** db, Queue<Record*>& queue,const std::pair<size_t, size_t>& interval);
enum MenuCommands {
    Noting = 0, ShowDataBase, ShowSortedDataBase, ShowQueue, ShowBinaryTree, DoEncoding, DoDataBaseCompression
};
MenuCommands menuController(bool isHavingQueue);
void sort(Record** array, int size);
void showMenu();

// coding/encoding
std::vector<std::string> convertRecordsArrToVector(Record* allRecords, size_t recordsCount = 4000);
std::vector<char> convertStringVecToCharVec(const std::vector<std::string>& stringVec);
std::vector<std::pair<char, double>> getDecProbabilityArr(const std::map<char, double>& mapSymbolToProbability);
std::pair<std::vector<double>, std::vector<char>> splitPair(const std::vector<std::pair<char, double>>& modifiedDecProbArr);
void printMapSymbolToProbability(const std::map<char, double>& mapCharToProbability);
void printMapSymbolToCode(const std::map<char, std::string>& mapCharToCode);
void printDecodedText(const std::vector<char>& decodedText);
void doEncoding(const std::vector<char,double> decProbArr);
void showEncodingInfo(const std::map<char,std::string>& mapSymbolToCode);
void compressDb(std::string filePath);
double calcCompressionCofficient(double nonCompressedDbSize, double compressedDbSize);

enum CriticalErrors {
    controlSumErr, openFileErr
};


int main()
{
    system("chcp 866");

    FILE* dbStream;
    auto err = fopen_s(&dbStream,"GreatPeoplesLife.dat", "rb");

    if (err != NULL) {
        std::cout << "Can't open the file!";
        return -1;
    }

    const auto recordsCount = 4000;

    Record* allRecords = new Record[recordsCount];
    auto stopMarker = fread((Record*)allRecords, sizeof(Record), recordsCount, dbStream);
    auto stringVec = convertRecordsArrToVector(allRecords,recordsCount);
    auto text = convertStringVecToCharVec(stringVec);

    Record* allRecordsPtr[recordsCount] = { nullptr };
    for (auto i = 0; i < recordsCount; i++) {
        allRecordsPtr[i] = &allRecords[i];
    }

    HeapSort(allRecordsPtr, recordsCount);

    bool menuCicle = true;
    bool isHavingQueue = false;
    std::string userCommand = "";
    Queue<Record*> recordsQueue;
    Record** treeData;
    size_t treeSize = 0;

    auto interval = std::make_pair(size_t(), size_t());
    while (menuCicle) {

        showMenu();

        auto answer = menuController(isHavingQueue);
        switch (answer)
        {
        case MenuCommands::ShowDataBase:
            showDb(allRecords, stopMarker);
            break;
        case MenuCommands::ShowSortedDataBase:
            showDb(allRecordsPtr, recordsCount);
            break;
        case MenuCommands::ShowQueue:
            userCommand = CallInput();
            interval = findRecords(allRecordsPtr, recordsCount, userCommand);
            isHavingQueue = true;

            if (interval.first == 0 && interval.second == 0)
              break;

            fillRecordsQueue(allRecordsPtr, recordsQueue, interval);
            showQueue(recordsQueue, interval);
            break;

        case MenuCommands::ShowBinaryTree:
            treeSize = interval.second - interval.first;
            std::cout << treeSize << "\n";
            treeData = new Record* [treeSize];
            for (auto i = 0; i < treeSize; i++)
                treeData[i] = recordsQueue.pop();
        
            sort(treeData, treeSize);
            tree(&allRecordsPtr[interval.first], treeSize);
            _getch();
            break;
        default:
            break;
        }
    }

    std::map<char,double> symbolsProbability = TextReader::getSymbolsProbability(text);
    printMapSymbolToProbability(symbolsProbability);
    auto decProbabilityArr = getDecProbabilityArr(symbolsProbability);
    std::pair<std::vector<double>, std::vector<char>> decArrAndSymbols = splitPair(decProbabilityArr);
    auto huffmanEncoder = HuffmanEncoding();
    auto mapSymbolToCodeHuffman = huffmanEncoder.encode(decArrAndSymbols.first, decArrAndSymbols.second);
    auto midHuffmanWordLen = huffmanEncoder.calcMidWordLen(symbolsProbability, mapSymbolToCodeHuffman);
    std::cout << "Huffman code data:\n";
    printMapSymbolToCode(mapSymbolToCodeHuffman);
    std::cout << "\nMid word len Huffman:\n" << midHuffmanWordLen << "\n";

    const std::string compressedDbPath = "res/compresed.dat";
    auto binaryText = TextReader::getTextBinary(mapSymbolToCodeHuffman, text);
    auto bytesSequence = TextWriter::getBytesToWrite(binaryText);
    auto errFlag = TextWriter::writeBytesToFile(compressedDbPath, bytesSequence);

    if (errFlag == FileErrors::openingError) {
        std::cout << "File err in writtining bytes.Stoping the process...\n";
        exit(CriticalErrors::openFileErr);
    }

    std::pair<std::vector<Byte>, FileErrors> bytesFromFileAndErr = TextReader::readCodeFromFile(compressedDbPath);
    if (bytesFromFileAndErr.second != FileErrors::ok) {
        std::cout << "Error in reading bytes.Stoping the process...\n";
        exit(CriticalErrors::openFileErr);
    }
    std::vector<Byte> bytesFromFile = bytesFromFileAndErr.first;
    auto booleanCode = ByteWrapper::convertBytesToBool(bytesFromFile);
    auto huffmanDecoder = HuffmanEncoding::getDecoderFromCoder(mapSymbolToCodeHuffman);
    auto decodedText = BaseDecoder::getDecodedText(huffmanDecoder, booleanCode);
    printDecodedText(decodedText);

    return 0;
}

void showDb(Record* db,const int recordsCount,const int partSize)
{
    system("cls");

    int inc = 0;
    while (true) {

        std::cout << "\t\t\t------------------------------------\n";
        std::cout << "\t\t\t|BACK - Q | GO - ANY KEY | QUIT - E|\n";
        std::cout << "\t\t\t------------------------------------\n\n";

        for (int i = inc; i < inc + partSize; i++) {
            std::cout << i << ":\t" << db[i].Author << "\t" << db[i].Tittle << "\t" << db[i].Publisher << "\t" <<
                db[i].Year << "\t" << db[i].PageNumber << "\n";
        }
        
        if (inc % partSize == 0) {
            auto key = _getch();
            
            switch (key)
            {
            case 'e':
                return;
                break;
            case 'q':
                if(inc >= partSize)
                    inc -= partSize;
                break;
            default:
                if(inc + partSize < recordsCount)
                    inc += partSize;
                break;
            }
            system("cls");
        }
    }
}

void showDb(Record** db, const int recordsCount, const int partSize)
{
    system("cls");

    int inc = 0;
    while (true) {

        std::cout << "\t\t\t------------------------------------\n";
        std::cout << "\t\t\t|BACK - Q | GO - ANY KEY | QUIT - E|\n";
        std::cout << "\t\t\t------------------------------------\n\n";

        for (int i = inc; i < inc + partSize; i++) {
            std::cout << i << ":\t" << db[i]->Author << "\t" << db[i]->Tittle << "\t" << db[i]->Publisher << "\t" <<
                db[i]->Year << "\t" << db[i]->PageNumber << "\n";
        }

        if (inc % partSize == 0) {
            auto key = _getch();

            switch (key)
            {
            case 'e':
                return;
                break;
            case 'E':
                return;
                break;
            case 'q':
                if (inc >= partSize)
                    inc -= partSize;
                break;
            case 'Q':
                if (inc >= partSize)
                    inc -= partSize;
                break;
            case 'Й':
                if (inc >= partSize)
                    inc -= partSize;
                break;
            case 'й':
                if (inc >= partSize)
                    inc -= partSize;
                break;
            default:
                if (inc + partSize < recordsCount)
                    inc += partSize;
                break;
            }
            system("cls");
        }
    }
}

std::string CallInput()
{
    std::cout << "Input first 3 last name letter's: ";
    std::string command = "";
    std::cin >> command;
    return command;
}

void sort(Record** array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j]->PageNumber > array[j + 1]->PageNumber) {
                auto temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void showQueue(const Queue<Record*>& queue, const std::pair<size_t, size_t>& interval)
{
    std::cout << "First id: "<< interval.first << "\n";

    queue.print();

    std::cout << "\nLast id:" << interval.second << "\n";
    std::cout << "\nPRESS ANY KEY...";
    _getch();
}

std::pair<size_t,size_t> findRecords(Record** db, const int recordsCount,const std::string& key)
{
    auto startIndex = BSearch(db, recordsCount, key);
    if (startIndex == -1)
        return std::make_pair(0, 0);
    auto endIndex = startIndex;

    for (auto i = startIndex + 1; i < recordsCount; i++) {
        auto guess = setSpecificKey(db, i, key);
        if (guess == key)
           endIndex++;
    }

    return std::make_pair(startIndex, endIndex);
}


void fillRecordsQueue(Record** db, Queue<Record*>& queue,const std::pair<size_t, size_t>& interval)
{
    queue.clear();
    for (auto i = interval.first; i != interval.second; i++)
        queue.add(db[i]);
}

MenuCommands menuController(bool isHavingQueue)
{
    auto userInput = _getch();

    switch (userInput)
    {
    case '1':
        return MenuCommands::ShowDataBase;
    case '2':
        return MenuCommands::ShowSortedDataBase;
    case '3':
        return MenuCommands::ShowQueue;
    case '4':
        if (isHavingQueue)
            return MenuCommands::ShowBinaryTree;
        else {
            std::cout << "Queue is empty.Fill queue then try again\n";
            break;
        }
        break;
    default:
        break;
    }

    return MenuCommands::Noting;
}

void showMenu()
{
    system("cls");

    std::cout << "\t\t\t------------------------------------\n";
    std::cout << "\t\t\t|               MENU               |\n";
    std::cout << "\t\t\t|   SHOW DB - 1,SHOW SORTED DB - 2 |\n";
    std::cout << "\t\t\t|      FIND ALL BY LAST NAME - 3   |\n";
    std::cout << "\t\t\t|   SHOW TREE OR FIND IN TREE - 4  |\n";
    std::cout << "\t\t\t------------------------------------\n";
}

std::vector<std::string> convertRecordsArrToVector(Record* allRecords, size_t recordsCount)
{
    std::vector<std::string> convertedText;
    for (size_t i = 0; i < recordsCount; i++) {
        convertedText.push_back(allRecords[i].Author);
        convertedText.push_back(allRecords[i].Publisher);
        convertedText.push_back(allRecords[i].Tittle);
        convertedText.push_back(std::to_string(allRecords[i].Year));
        convertedText.push_back(std::to_string(allRecords[i].PageNumber));
    }
    return convertedText;
}

std::vector<char> convertStringVecToCharVec(const std::vector<std::string>& stringVec)
{
    std::vector<char> convertedText;
    size_t spaceIter = 0;
    for (size_t i = 0; i < stringVec.size(); i++) {
        for (size_t j = 0; j < stringVec[i].length(); j++) {
            convertedText.push_back(stringVec[i][j]);
        }
        spaceIter++;
        if (spaceIter == 5) {
            convertedText.push_back('\n');
            spaceIter = 0;
        }
        convertedText.push_back(' ');
    }
    return convertedText;
}

std::vector<std::pair<char, double>> getDecProbabilityArr(const std::map<char, double>& mapSymbolToProbability) {
    std::vector<std::pair<char, double>> decProbabilityArr;
    for (const auto pair : mapSymbolToProbability) {
        decProbabilityArr.push_back(std::make_pair(pair.first, pair.second));
    }

    std::sort(decProbabilityArr.begin(), decProbabilityArr.end(), [](std::pair<char, double> first,
        std::pair<char, double> second) {return first.second > second.second; });

    return decProbabilityArr;
}

std::pair<std::vector<double>, std::vector<char>> splitPair(const std::vector<std::pair<char, double>>& modifiedDecProbArr)
{
    std::vector<double> decProbArr;
    std::vector<char> mappingCharToProbArr;
    for (const auto& pair : modifiedDecProbArr) {
        decProbArr.push_back(pair.second);
        mappingCharToProbArr.push_back(pair.first);
    }

    return std::make_pair(decProbArr, mappingCharToProbArr);
}

void printMapSymbolToProbability(const std::map<char, double>& mapCharToProbability)
{
    for (const auto& charAndProbability : mapCharToProbability) {
        std::cout << charAndProbability.first << "->" << charAndProbability.second << "\n";
    }
}

void printMapSymbolToCode(const std::map<char, std::string>& mapCharToCode)
{
    for (const auto& charAndCode : mapCharToCode) {
        std::cout << charAndCode.first << "->" << charAndCode.second << "\n";
    }
}

void printDecodedText(const std::vector<char>& decodedText)
{
    std::cout << "\n";
    for (const auto& symbol : decodedText) {
        std::cout << symbol;
    }
}