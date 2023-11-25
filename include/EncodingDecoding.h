#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

typedef unsigned char Byte;

enum FileErrors {
	ok, openingError
};

class ByteWrapper {
public:
	static Byte pack_byte(bool bits[8]);
	static void unpack_byte(Byte b, bool bits[8]);
	static std::vector<std::vector<bool>> convertBytesToBool(const std::vector<Byte>& bytes);
};

class TextReader {
public:
	static std::map<char, double> getSymbolsProbability(const std::vector<char>& text);
	static bool isProbabilitysSumTrue(const std::map<char, double>& symbolToProbability);
	static double calcTextEntrophy(const std::map<char, double>& symbolToProbability);
	static std::pair<std::vector<char>, FileErrors> readText(const std::string& filePath);
	static std::vector<int> getTextBinary(std::map<char, std::string>& encoder, const std::vector<char>& text);
	static std::pair<size_t, FileErrors> getFileSizeInBytes(std::string filePath);
	static std::pair<std::vector<Byte>, FileErrors> readCodeFromFile(std::string filePath);
};

class TextWriter {
public:
	static std::vector<Byte> getBytesToWrite(const std::vector<int>& code);
	static FileErrors writeBytesToFile(std::string filePath, const std::vector<Byte>& bytes);
};

class BaseDecoder {
public:
	static std::vector<char> getDecodedText(std::map<std::string, char>& decoder,
		const std::vector<std::vector<bool>>& codesMatrix);
};

class HuffmanEncoding {
private:
	struct huffmanElement {
		size_t firstAncestor = 0;
		size_t secondAncestor = 0;
		std::string binaryCode = "";

		huffmanElement(size_t firstAnc, size_t secondAnc) : firstAncestor(firstAnc), secondAncestor(secondAnc) {};
		huffmanElement() {};
	};
	void insertSum(std::vector<std::pair<huffmanElement, double>>& whereInsert, double whatInsert,
		size_t preLastWhatInsertId, size_t lastWhatInsertId);
	void copyingFromPrevVec(const std::vector<std::pair<huffmanElement, double>>& from,
		std::vector<std::pair<huffmanElement, double>>& to);
	double calcSum(const std::vector<std::pair<huffmanElement, double>>& vector);
	void restoreCodes(std::vector<std::vector<std::pair<huffmanElement, double>>>& huffmanBody);
public:
	std::map<char, std::string> encode(std::vector<double>& decProbabilityArr,
		const std::vector<char>& charMappingToProb);

	double calcMidWordLen(std::map<char, double>& symbolToProbability, std::map<char, std::string>& huffmanDecoder);
	static std::map<std::string, char> getDecoderFromCoder(const std::map<char, std::string>& coder);
	void printCode(const std::map<char, std::string>& mapCharToCode);
};