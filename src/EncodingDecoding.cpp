#include "EncodingDecoding.h"

Byte ByteWrapper::pack_byte(bool bits[8])
{
    Byte result(0);
    for (unsigned i(8); i--;)
    {
        result <<= 1;
        result |= Byte(bits[i]);
    }
    return result;
}

void ByteWrapper::unpack_byte(Byte b, bool bits[8])
{
    for (unsigned i(0); i < 8; ++i)
    {
        bits[i] = (b & 0x01) != 0;
        b >>= 1;
    }
}

std::vector<std::vector<bool>> ByteWrapper::convertBytesToBool(const std::vector<Byte>& bytes)
{
    std::vector<std::vector<bool>> convertedBytes;
    bool bits[8] = { 0 };
    for (const auto& byte : bytes) {
        ByteWrapper::unpack_byte(byte, bits);
        convertedBytes.push_back(std::vector<bool>{bits[0], bits[1], bits[2],
            bits[3], bits[4], bits[5], bits[6], bits[7]});
    }

    return convertedBytes;
}

std::map<char, double> TextReader::getSymbolsProbability(const std::vector<char>& text)
{
    std::map<char, size_t> symbolToCount;
    for (const auto symbol : text) {
        if (symbolToCount.count(symbol) == 0) {
            symbolToCount[symbol] = 1;
        }
        else {
            symbolToCount[symbol]++;
        }
    }

    size_t sum = 0;
    for (const auto pair : symbolToCount) {
        sum += pair.second;
    }

    std::map<char, double> symbolToProbability;
    for (const auto pair : symbolToCount) {
        symbolToProbability[pair.first] = pair.second / double(sum);
    }

    return symbolToProbability;
}

bool TextReader::isProbabilitysSumTrue(const std::map<char, double>& symbolToProbability)
{
    double sum = 0.0;
    for (const auto& pair : symbolToProbability) {
        sum += pair.second;
    }
    if (sum == 1.0) {
        return true;
    }
    return false;
}

double TextReader::calcTextEntrophy(const std::map<char, double>& symbolToProbability)
{
    double entrophy = 0.0f;
    for (const auto pair : symbolToProbability) {
        entrophy += -pair.second * log2f(pair.second);
    }
    return entrophy;
}

std::pair<std::vector<char>, FileErrors> TextReader::readText(const std::string& filePath)
{
    std::vector<char> text;
    std::ifstream freader;

    try
    {
        freader.open(filePath);
        if (freader.is_open()) {
            while (!freader.eof()) {
                char ch = freader.get();
                text.push_back(ch);
            }
        }
        freader.close();
    }
    catch (const std::exception&)
    {
        std::cout << "Can't open file with name " << filePath << "\n";
        if (freader.is_open()) {
            freader.close();
        }
        return std::make_pair(std::vector<char>(), FileErrors::openingError);
    }

    return std::make_pair(text, FileErrors::ok);
}

std::vector<int> TextReader::getTextBinary(std::map<char, std::string>& encoder, const std::vector<char>& text)
{
    std::vector<int> codedText;
    for (const auto symbol : text) {
        std::string wordCode = encoder[symbol];
        for (const auto bit : wordCode) {
            if (bit == '1') {
                codedText.push_back(1);
            }
            else {
                codedText.push_back(0);
            }
        }
    }

    return codedText;
}

std::pair<size_t, FileErrors> TextReader::getFileSizeInBytes(std::string filePath)
{
    size_t fileSizeByte;
    try
    {
        std::fstream freader(filePath);
        freader.seekg(0, std::ios::end);
        fileSizeByte = freader.tellg();
        freader.close();
    }
    catch (const std::exception&)
    {
        std::cout << "Error in getFileSizeBytes().\n";
        std::cout << "fstream file errror.\n";
        return std::make_pair(0, FileErrors::openingError);
    }

    return std::make_pair(fileSizeByte, FileErrors::ok);
}

std::pair<std::vector<Byte>, FileErrors> TextReader::readCodeFromFile(std::string filePath)
{
    Byte* code = nullptr;
    std::pair<size_t, FileErrors> fileSizeBytesAndErr = TextReader::getFileSizeInBytes(filePath);

    if (fileSizeBytesAndErr.second != FileErrors::ok) {
        std::cout << "Error in getting file size.\n";
        return std::make_pair(std::vector<Byte>(), FileErrors::openingError);
    }
    size_t fileSize = fileSizeBytesAndErr.first;

    if (fileSize == 0) {
        std::cout << "File is empty.\n";
        return std::make_pair(std::vector<Byte>(), FileErrors::ok);
    }

    try
    {
        FILE* freaderBytes = nullptr;
        code = new Byte[fileSize];
        fopen_s(&freaderBytes, filePath.c_str(), "rb");
        fread((Byte*)code, sizeof(Byte), fileSize, freaderBytes);
        fclose(freaderBytes);
    }
    catch (const std::exception&)
    {
        std::cout << "Error in readCodeFromFile().\n";
        std::cout << "FILE critical error in reading binary.\n";
        return std::make_pair(std::vector<Byte>(), FileErrors::openingError);
    }

    std::vector<Byte> fromFileBytes;
    for (size_t i = 0; i < fileSize; i++) {
        fromFileBytes.push_back(code[i]);
    }

    delete[] code;
    return std::make_pair(fromFileBytes, FileErrors::ok);
}

std::vector<Byte> TextWriter::getBytesToWrite(const std::vector<int>& code)
{
    bool bits[8];
    std::vector<bool> oneByte;
    std::vector<Byte> bytesToWrite;
    Byte byte;

    for (const auto bit : code) {
        oneByte.push_back(bit);
        if (oneByte.size() == 8) {
            for (size_t i = 0; i < 8; i++) {
                bits[i] = oneByte[i];
                byte = ByteWrapper::pack_byte(bits);
            }
            bytesToWrite.push_back(byte);
            oneByte.resize(0);
        }
    }

    size_t missingBitsCount = 8 - oneByte.size();
    for (size_t i = 0; i < missingBitsCount; i++) {
        oneByte.push_back(0);
    }
    for (size_t i = 0; i < 8; i++) {
        bits[i] = oneByte[i];
    }
    Byte lastByte = ByteWrapper::pack_byte(bits);
    bytesToWrite.push_back(lastByte);

    return bytesToWrite;
}

FileErrors TextWriter::writeBytesToFile(std::string filePath, const std::vector<Byte>& bytes)
{
    try
    {
        FILE* fwriter;
        fopen_s(&fwriter, filePath.c_str(), "w");
        size_t i = 0;
        for (const auto& byte : bytes) {
            fwrite(&byte, sizeof(Byte), 1, fwriter);
        }
        fclose(fwriter);
    }
    catch (const std::exception&)
    {
        std::cout << "Error in writesBytesToFile().\n";
        std::cout << "Can't open file with name " << filePath << " stoping the process...\n";
        return FileErrors::openingError;
    }
    return FileErrors::ok;
}

std::vector<char> BaseDecoder::getDecodedText(std::map<std::string, char>& decoder, const std::vector<std::vector<bool>>& codesMatrix)
{
    std::string inputBitSequence = "";
    for (const auto& byte : codesMatrix) {
        for (const auto bit : byte) {
            inputBitSequence += std::to_string(bit);
        }
    }

    std::vector<char> decodedText;
    std::string wordInSequence = "";
    for (const auto bit : inputBitSequence) {
        wordInSequence += bit;
        if (decoder.count(wordInSequence) != 0) {
            decodedText.push_back(decoder[wordInSequence]);
            wordInSequence = "";
        }
    }

    return decodedText;
}

void HuffmanEncoding::insertSum(std::vector<std::pair<huffmanElement, double>>& whereInsert,
    double whatInsert, size_t preLastWhatInsertId, size_t lastWhatInsertId)
{
    size_t j = 0;
    for (size_t i = 0; i < whereInsert.size(); i++) {
        if (whatInsert <= whereInsert[i].second) {
            j++;
        }
        else {
            break;
        }
    }
    whereInsert.insert(whereInsert.begin() + j, std::make_pair(huffmanElement(preLastWhatInsertId, lastWhatInsertId), whatInsert));
}

void HuffmanEncoding::copyingFromPrevVec(const std::vector<std::pair<huffmanElement, double>>& from,
    std::vector<std::pair<huffmanElement, double>>& to)
{
    for (size_t i = 0; i < from.size() - 2; i++) {
        to.push_back(std::make_pair(huffmanElement(i, i), from[i].second));
    }
}

double HuffmanEncoding::calcSum(const std::vector<std::pair<huffmanElement, double>>& vector)
{
    return (vector[vector.size() - 1].second + vector[vector.size() - 2].second);
}

void HuffmanEncoding::restoreCodes(std::vector<std::vector<std::pair<huffmanElement, double>>>& huffmanBody)
{
    std::string code = "";
    std::reverse(huffmanBody.begin(), huffmanBody.end());

    huffmanBody[0][0].first.binaryCode = "0";
    huffmanBody[0][1].first.binaryCode = "1";

    for (size_t i = 0; i < huffmanBody.size() - 1; i++) {
        for (size_t j = 0; j < huffmanBody[i].size(); j++) {
            std::pair<size_t, size_t> inPrevVecIndexes = std::make_pair(huffmanBody[i][j].first.firstAncestor,
                huffmanBody[i][j].first.secondAncestor);
            if (inPrevVecIndexes.first == inPrevVecIndexes.second) {
                huffmanBody[i + 1][inPrevVecIndexes.first].first.binaryCode = huffmanBody[i][j].first.binaryCode;
            }
            else {
                huffmanBody[i + 1][inPrevVecIndexes.first].first.binaryCode += huffmanBody[i][j].first.binaryCode + "1";
                huffmanBody[i + 1][inPrevVecIndexes.second].first.binaryCode += huffmanBody[i][j].first.binaryCode + "0";
            }
        }
    }
}

std::map<char, std::string> HuffmanEncoding::encode(std::vector<double>& decProbabilityArr,
    const std::vector<char>& charMappingToProb)
{
    std::map<char, std::string> huffmanDecoder;
    std::vector<std::vector<std::pair<huffmanElement, double>>> mainBody;
    mainBody.push_back(std::vector<std::pair<huffmanElement, double>>());

    for (const auto& val : decProbabilityArr) {
        mainBody[0].push_back(std::make_pair(huffmanElement(), val));
    }

    size_t currentVec = 0;
    while (currentVec < decProbabilityArr.size() - 2) {
        double sum = calcSum(mainBody[currentVec]);
        mainBody.push_back(std::vector<std::pair<huffmanElement, double>>());
        currentVec++;
        copyingFromPrevVec(mainBody[currentVec - 1], mainBody[currentVec]);
        insertSum(mainBody[currentVec], sum, mainBody[currentVec - 1].size() - 1, mainBody[currentVec - 1].size() - 2);
    }

    restoreCodes(mainBody);
    size_t symbolIter = 0;
    for (const auto& probAndCode : mainBody[mainBody.size() - 1]) {
        huffmanDecoder[charMappingToProb[symbolIter]] = probAndCode.first.binaryCode;
        symbolIter++;
    }

    return huffmanDecoder;
}

double HuffmanEncoding::calcMidWordLen(std::map<char, double>& symbolToProbability, std::map<char, std::string>& huffmanDecoder)
{
    double midLen = 0.0;
    size_t i = 0;
    for (const auto& symbolAndCode : huffmanDecoder) {
        midLen += symbolToProbability[symbolAndCode.first] * symbolAndCode.second.length();
    }

    return midLen;
}

std::map<std::string, char> HuffmanEncoding::getDecoderFromCoder(const std::map<char, std::string>& coder)
{
    std::map<std::string, char> decoder;
    for (const auto& keyVal : coder) {
        decoder[keyVal.second] = keyVal.first;
    }
    return decoder;
}

void HuffmanEncoding::printCode(const std::map<char, std::string>& mapCharToCode)
{
    for (const auto& charAndCode : mapCharToCode) {
        std::cout << charAndCode.first << "->" << charAndCode.second << "\n";
    }
}