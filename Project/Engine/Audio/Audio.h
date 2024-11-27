#pragma once

#include <fstream>
#include <array>
#include <cstdint>
#include <vector>
#include <map>
#include <wrl.h>

#include <xaudio2.h>

#pragma comment (lib,"xaudio2.lib")



class Audio
{
private:

    struct ChunkHeader
    {
        char id[4];             // チャンク毎のID
        int32_t size;           // チャンクサイズ
    };

    struct RiffHeader
    {
        ChunkHeader chunk;      // "RIFF"
        char type[4];             // "WAVE"
    };

    struct FormatChunk
    {
        ChunkHeader chunk;      // ”fmt"
        WAVEFORMATEX fmt;       // 波形フォーマット
    };

    struct SoundData
    {
        WAVEFORMATEX wfex;
        BYTE* pBuffer;
        unsigned int bufferSize;
    };
public:
    // 音
    struct Sound {
        uint32_t soundDataHandle;
        uint32_t voiceHandle;
        float volume;
    };


    ~Audio();

    void Initialize();

    uint32_t SoundLoadWave(const std::string& _filename);
    uint32_t SoundPlay(uint32_t _soundHandle, float _volume, bool _loop = false, bool _enableOverlap = true);
    bool IsPlaying(uint32_t _voiceHandle) const;
    void SetVolume(uint32_t _voiceHandle, float _volume);
    void SoundStop(uint32_t _voiceHandle);

    void SoundUnLoad(SoundData* _soundData);

private:

    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    IXAudio2MasteringVoice* masterVoice_;

    std::map <uint32_t, uint32_t> map_;

    std::vector<SoundData> sounds_;
    std::vector<IXAudio2SourceVoice*> sourceVoice_;
};