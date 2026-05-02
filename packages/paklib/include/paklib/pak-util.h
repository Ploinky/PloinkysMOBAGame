#pragma once

#include <string>
#include <fstream>
#include "pak-header.h"
#include "pak-file-table-entry.h"
#include <vector>
#include <filesystem>
#include "pak-file.h"
#include <regex>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

namespace paklib {
	struct file_data_t {
		unsigned char* data;
		size_t size;
	};

	class PakUtil {
	public:
		static file_data_t ReadBytesFromFile(std::string strFileName) {
			unsigned char* f = 0;

			// open the file
			std::ifstream file(strFileName, std::ios::in | std::ios::binary | std::ios::ate);

			// if open was successful
			if (file.is_open())
			{
				// find the length of the file
				size_t length = file.tellg();
				
				// collect the file data
				f = new unsigned char[length];
				file.seekg(0, std::ios::beg);
				file.read(reinterpret_cast<char*>(f), length);
				file.close();
				
				return {f, length};
			}

			throw new std::runtime_error("Could not load content of file <" + strFileName + ">");
		}

		static void PackToPak(std::string fileName, std::string pakFileName) {
			// header
			PakHeader header{};
			// collect entries for header block here
			std::vector<PakFileTableEntry> entries;
			// collect file data block here
			std::vector<uint8_t> data;

			PackFile(fileName, fileName, &data, &entries);

			header.EntryCount = entries.size();

			// header
			std::ofstream pakFile(pakFileName, std::ios::binary);
			pakFile.write((const char*) &header.Magic, 3);
			pakFile.write((const char*) &header.EntryCount, sizeof(header.EntryCount));
			pakFile.write((const char*)&header.EntryCount, sizeof(int));
			
			for (PakFileTableEntry& entry : entries) {
				// header table entry
				pakFile.write((const char*)&entry.FilePathLength, sizeof(entry.FilePathLength));
				pakFile.write((const char*)entry.FilePath.data(), entry.FilePathLength);
				pakFile.write((const char*)&entry.Offset, sizeof(entry.Offset));
				pakFile.write((const char*)&entry.Size, sizeof(entry.Size));
			}

			std::streampos len = pakFile.tellp();
			pakFile.seekp(3 + sizeof(header.EntryCount), std::ios::beg);
			pakFile.write((const char*) &len, sizeof(int));
			pakFile.seekp(0, std::ios::end);
			pakFile.write((char*)data.data(), data.size());


			pakFile.close();
		}

		static void PackFile(std::string fileParentDir, std::string fileName, std::vector<uint8_t>* data, std::vector<PakFileTableEntry>* entries) {
			if (IsDirectory(fileName)) {
				printf("That is a directory!\n");

				for (const auto& entry : std::filesystem::directory_iterator(fileName)) {
					std::cout << entry.path().string().append("\n") << std::endl;
					PackFile(fileParentDir, entry.path().string(), data, entries);
				}

				return;
			}

			file_data_t file = ReadBytesFromFile(fileName);

			uint64_t offset = data->size();

			PakFileTableEntry entry{};
			entry.FilePathLength = fileName.length();
			entry.FilePath = std::regex_replace(fileName, std::regex("\\\\"), "/");
			entry.Offset = offset;
			entry.Size = file.size;

			entries->push_back(entry);

			data->resize(offset + file.size);
			memcpy(data->data() + offset, file.data, file.size);
		}

		static PakFile* UnpackFromPak(std::string pakFileName) {
			file_data_t file = ReadBytesFromFile(pakFileName);

			PakHeader header{};
			uint64_t offset = 0;
			memcpy(&header.Magic, file.data, 3);
			offset += 3;

			memcpy(&header.EntryCount, file.data + offset, sizeof(header.EntryCount));
			offset += sizeof(header.EntryCount);

			memcpy(&header.HeaderSize, file.data + offset, sizeof(header.HeaderSize));
			offset += sizeof(header.HeaderSize);
			
			std::cout << "Magic: " << header.Magic << std::endl;
			std::cout << "Entries: " << header.EntryCount << std::endl;
			std::cout << "Size: " << header.HeaderSize << std::endl;

			std::vector<PakFileTableEntry> entries;

			for (int i = 0; i < header.EntryCount; i++) {
				PakFileTableEntry entry{};
				
				memcpy(&entry.FilePathLength, file.data + offset, sizeof(entry.FilePathLength));
				offset += sizeof(entry.FilePathLength);

				entry.FilePath = std::string();
				entry.FilePath.resize(entry.FilePathLength);
				std::cout << entry.FilePathLength << std::endl;
				memcpy(entry.FilePath.data(), file.data + offset, entry.FilePathLength);
				offset += entry.FilePathLength;

				memcpy(&entry.Offset, file.data + offset, sizeof(entry.Offset));
				offset += sizeof(entry.Offset);

				memcpy(&entry.Size, file.data + offset, sizeof(entry.Size));
				offset += sizeof(entry.Size);

				entries.push_back(entry);
			}

			for (PakFileTableEntry entry : entries) {
				std::cout << "FilePath: " << entry.FilePath << ", Offset: " << entry.Offset << ", Size: " << entry.Size << std::endl;
			}

			PakFile* pakFile = new PakFile();
			pakFile->entries = entries;
			pakFile->data.resize(file.size);
			pakFile->header = header;
			memcpy(pakFile->data.data(), file.data, file.size);

			return pakFile;
		}

	private:
		static bool IsDirectory(std::string fileName) {
			struct stat s;
			if (stat(fileName.c_str(), &s) == 0)
			{
				return s.st_mode & S_IFDIR;
			}
			else
			{
				return false;
			}
		}
	};
}