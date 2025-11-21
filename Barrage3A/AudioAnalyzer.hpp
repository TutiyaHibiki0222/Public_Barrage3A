/*
	サウンド 解析.
*/

#pragma once

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

namespace System {
    
    // .mp3 形式の 解読.
    class MP3Info {
    public:
        static bool Parse(const std::string& filepath, double& duration, int& channels, int& sampleRate) {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) return false;

            // ID3v2
            char id3[10];
            file.read(id3, 10);
            int skip = 0;
            if (std::memcmp(id3, "ID3", 3) == 0) {
                skip =
                    ((id3[6] & 0x7F) << 21) |
                    ((id3[7] & 0x7F) << 14) |
                    ((id3[8] & 0x7F) << 7) |
                    (id3[9] & 0x7F);
                file.seekg(10 + skip, std::ios::beg);
            }
            else {
                file.seekg(0, std::ios::beg);
            }

            std::streampos frameStartPos;
            uint8_t buffer[4];
            while (file.read(reinterpret_cast<char*>(buffer), 4)) {
                if (!IsFrameHeader(buffer)) {
                    file.seekg(-3, std::ios::cur);
                    continue;
                }

                int bitrate = GetBitrate(buffer);
                sampleRate = GetSampleRate(buffer);
                channels = GetChannelMode(buffer);
                if (bitrate == 0 || sampleRate == 0) return false;

                int versionID = (buffer[1] >> 3) & 0x03;
                int layer = (buffer[1] >> 1) & 0x03;
                int samplesPerFrame = GetSamplesPerFrame(versionID, layer);

                // フレーム先頭保存
                frameStartPos = file.tellg();
                file.seekg(-4, std::ios::cur);

                // Xing or Info
                int xingOffset = (channels == 1) ? 21 : 36;
                file.seekg(xingOffset, std::ios::cur);
                char xing[4];
                file.read(xing, 4);
                if (std::memcmp(xing, "Xing", 4) == 0 || std::memcmp(xing, "Info", 4) == 0) {
                    uint32_t flags = ReadBigEndianUInt32(file);
                    if (flags & 0x0001) {
                        uint32_t frames = ReadBigEndianUInt32(file);
                        duration = static_cast<double>(frames * samplesPerFrame) / sampleRate;
                        return true;
                    }
                }

                // VBRI (Xingの後方互換)
                file.seekg(frameStartPos + std::streamoff(32), std::ios::beg);
                char vbri[4];
                file.read(vbri, 4);
                if (std::memcmp(vbri, "VBRI", 4) == 0) {
                    file.seekg(6, std::ios::cur); // skip version/delay
                    uint32_t frames = ReadBigEndianUInt32(file);
                    duration = static_cast<double>(frames * samplesPerFrame) / sampleRate;
                    return true;
                }

                // --- CBR fallback ---
                file.seekg(0, std::ios::end);
                std::streampos fileSize = file.tellg();
                std::streampos audioSize = fileSize - frameStartPos;
                duration = static_cast<double>(audioSize * 8) / (bitrate * 1000);
                return true;
            }

            return false;
        }

    private:
        static bool IsFrameHeader(uint8_t* h) {
            return (h[0] == 0xFF) && ((h[1] & 0xE0) == 0xE0);
        }

        static uint32_t ReadBigEndianUInt32(std::ifstream& f) {
            uint8_t buf[4];
            f.read(reinterpret_cast<char*>(buf), 4);
            return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        }

        static int GetBitrate(uint8_t* h) {
            static const int bitrates[2][16] = {
                {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}, // MPEG1
                {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}      // MPEG2/2.5
            };
            int version = (h[1] >> 3) & 0x03;
            int bitrateIndex = (h[2] >> 4) & 0x0F;
            int row = (version == 3) ? 0 : 1; // 3=MPEG1
            return bitrates[row][bitrateIndex];
        }

        static int GetSampleRate(uint8_t* h) {
            static const int sampleRates[4] = { 44100, 48000, 32000, 0 };
            int version = (h[1] >> 3) & 0x03;
            int index = (h[2] >> 2) & 0x03;
            int base = sampleRates[index];
            if (version == 2) return base / 2;       // MPEG2
            if (version == 0) return base / 4;       // MPEG2.5
            return base;                             // MPEG1
        }

        static int GetChannelMode(uint8_t* h) {
            int mode = (h[3] >> 6) & 0x03;
            return (mode == 3) ? 1 : 2;
        }

        static int GetSamplesPerFrame(int versionID, int layer) {
            if (layer == 3) return 384; // Layer I
            if (layer == 2) return 1152; // Layer II
            if (layer == 1) {
                if (versionID == 3) return 1152; // MPEG1 Layer III
                else return 576;                // MPEG2,2.5 Layer III
            }
            return 1152;
        }
    };
    // .wav 形式の 解読.
    class WAVInfo {
    public:
        static bool Parse(const std::string& filepath, double& duration, int& channels, int& sampleRate) {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) return false;

            char chunkId[4];
            uint32_t chunkSize;
            char format[4];

            file.read(chunkId, 4);
            file.read(reinterpret_cast<char*>(&chunkSize), 4);
            file.read(format, 4);

            if (std::string(chunkId, 4) != "RIFF" || std::string(format, 4) != "WAVE") return false;

            uint16_t bitsPerSample = 0;
            bool gotFmt = false;
            bool gotData = false;
            uint32_t dataSize = 0;
            channels = 0;
            sampleRate = 0;

            while (file.read(chunkId, 4)) {
                uint32_t subchunkSize;
                file.read(reinterpret_cast<char*>(&subchunkSize), 4);
                std::streampos chunkStart = file.tellg();

                std::string id(chunkId, 4);
                if (id == "fmt ") {
                    uint16_t audioFormat = 0;
                    file.read(reinterpret_cast<char*>(&audioFormat), 2);
                    file.read(reinterpret_cast<char*>(&channels), 2);
                    file.read(reinterpret_cast<char*>(&sampleRate), 4);
                    file.seekg(6, std::ios::cur);
                    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);

                    if (audioFormat != 1 && audioFormat != 3 && audioFormat != 0xFFFE) return false;
                    gotFmt = true;
                }
                else if (id == "data") {
                    dataSize = subchunkSize;
                    gotData = true;
                }

                file.seekg(chunkStart + static_cast<std::streamoff>(subchunkSize));
                if (subchunkSize % 2 == 1)
                    file.seekg(1, std::ios::cur);
            }

            if (!gotFmt || !gotData) return false;
            if (channels <= 0 || sampleRate <= 0 || bitsPerSample <= 0) return false;

            double bytesPerSample = static_cast<double>(bitsPerSample) / 8.0;
            double frameSize = static_cast<double>(channels) * bytesPerSample;

            if (frameSize <= 0.0) return false;

            duration = static_cast<double>(dataSize) / frameSize / static_cast<double>(sampleRate);
            return true;
        }
    };
    // .ogg 形式の 解読.
    class OGGInfo {
    public:
        static bool Parse(const std::string& filepath, double& duration, int& channels, int& sampleRate) {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) return false;

            // --- ヘッダー読み込み ---
            const size_t initialScanSize = 4096;
            std::vector<uint8_t> buffer(initialScanSize);
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());


            bool foundInfo = false;
            for (size_t i = 0; i + 15 < buffer.size(); ++i) {
                if (buffer[i] == 1 && std::memcmp(&buffer[i + 1], "vorbis", 6) == 0) {
                    channels = buffer[i + 11];
                    sampleRate = buffer[i + 12] |
                        (buffer[i + 13] << 8) |
                        (buffer[i + 14] << 16) |
                        (buffer[i + 15] << 24);
                    foundInfo = true;
                    break;
                }
            }
            if (!foundInfo) return false;

            // --- ファイルサイズ取得 ---
            file.clear();
            file.seekg(0, std::ios::end);
            std::streampos fileSizePos = file.tellg();
            if (fileSizePos == -1) return false;

            std::streamoff fileSize = static_cast<std::streamoff>(fileSizePos);
            const std::streamoff chunkSize = 4096;
            std::streamoff readSize = std::min(chunkSize, fileSize);
            if (readSize <= 0) return false;

            std::vector<char> tailBuffer(static_cast<size_t>(readSize));
            file.seekg(fileSize - readSize, std::ios::beg);
            file.read(tailBuffer.data(), static_cast<std::streamsize>(readSize));

            // --- granule position 検出 ---
            uint64_t lastGranulePos = 0;
            for (size_t i = 0; i + 27 < tailBuffer.size(); ++i) {
                if (std::memcmp(&tailBuffer[i], "OggS", 4) == 0) {
                    uint64_t granulePos = 0;
                    for (int j = 0; j < 8; ++j) {
                        granulePos |= static_cast<uint64_t>(
                            static_cast<uint8_t>(tailBuffer[i + 6 + j])) << (j * 8);
                    }
                    if (granulePos > lastGranulePos) {
                        lastGranulePos = granulePos;
                    }
                }
            }

            // --- 再生時間の計算 ---
            if (sampleRate > 0 && lastGranulePos > 0)
                duration = static_cast<double>(lastGranulePos) / sampleRate;
            else
                duration = 0.0;

            return true;
        }
    };
    // .opus 形式の 解読.
    class OPUSInfo {
    public:
        static bool Parse(const std::string& filepath, double& duration, int& channels, int& sampleRate) {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) return false;

            std::vector<uint8_t> buffer(256);
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

            bool foundOpusHead = false;
            for (size_t i = 0; i + 19 < buffer.size(); ++i) {
                if (std::memcmp(&buffer[i], "OpusHead", 8) == 0) {
                    channels = buffer[i + 9];
                    sampleRate = 48000;
                    foundOpusHead = true;
                    break;
                }
            }
            if (!foundOpusHead) return false;

            file.clear();
            file.seekg(0, std::ios::end);
            std::streamoff fileSize = file.tellg();
            if (fileSize <= 0) return false;

            const std::streamoff chunkSize = 65536;
            std::streamoff readSize = std::min(chunkSize, fileSize);

            std::vector<char> tailBuffer(static_cast<size_t>(readSize));
            file.seekg(fileSize - readSize, std::ios::beg);
            file.read(tailBuffer.data(), static_cast<std::streamsize>(readSize));

            uint64_t lastGranulePos = 0;
            for (size_t i = 0; i + 27 < tailBuffer.size(); ++i) {
                if (std::memcmp(&tailBuffer[i], "OggS", 4) == 0) {
                    uint64_t granulePos = 0;
                    for (int j = 0; j < 8; ++j) {
                        granulePos |= static_cast<uint64_t>(
                            static_cast<uint8_t>(tailBuffer[i + 6 + j])) << (j * 8);
                    }
                    if (granulePos > lastGranulePos) {
                        lastGranulePos = granulePos;
                    }
                }
            }

            duration = (sampleRate > 0 && lastGranulePos > 0)
                ? static_cast<double>(lastGranulePos) / sampleRate
                : 0.0;

            return true;
        }
    };

}