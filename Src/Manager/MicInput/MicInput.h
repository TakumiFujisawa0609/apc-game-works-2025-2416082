#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <vector>

#pragma comment(lib, "winmm.lib")

class MicInput
{
public:
    MicInput();
    ~MicInput();

    // 初期化 (サンプリング周波数とバッファサイズの指定)
    bool Init(int sampleRate = 44100, int bufferSize = 512, int bufferCount = 4);

    // 毎フレーム更新 (録音完了データを処理)
    void Update();

    // 録音開始・停止
    void Start();
    void Stop();

    // 音量取得 (0～32767 程度)
    int  GetLevel() const;

    // 音量取得 (2000～32767 程度)
    int  GetPlayGameLevel() const;

    void VoiceLevelDraw(void);

private:

    static void CALLBACK WaveInProc(HWAVEIN hwi, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

    void OnBufferDone(WAVEHDR* hdr);    // コールバックから呼ばれる

private:

    HWAVEIN hWaveIn_ = nullptr;         // マイク入力デバイス
    HWAVEOUT hWaveOut_ = nullptr;       // スピーカー出力デバイス

    int sampleRate_ = 44100;
    int bufferSize_ = 512;
    int bufferCount_ = 4;
    int level_ = 0;

    std::vector<short*>   buffers_;    // 録音バッファ
    std::vector<WAVEHDR>  headers_;    // バッファヘッダ

    // 出力用ヘッダ（再利用）
    WAVEHDR WaveOutHdr_{};
    short* outBuffer_ = nullptr;
};
