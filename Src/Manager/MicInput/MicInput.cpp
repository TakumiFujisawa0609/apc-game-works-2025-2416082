#include "MicInput.h"
#include <cstring>
#include <cmath>

#include "../../Application/Application.h"

static constexpr float M_PI = 3.14159265358979323846f;

static constexpr int MIN_MIC_LEVEL = 2000;

static constexpr float MAX_HZ = 3400.0f;
static constexpr float MIN_HZ = 300.0f;

MicInput::MicInput() {}

MicInput::~MicInput()
{
    // 終了時に録音を停止
    Stop();

    // バッファを解放
    for (auto buf : buffers_)
    {
        delete[] buf;
    }
    buffers_.clear();
    
    if (outBuffer_) delete[] outBuffer_;
}

bool MicInput::Init(int sampleRate, int bufSize, int bufferCount)
{
    sampleRate_ = sampleRate;
    bufferSize_ = bufSize;
    bufferCount_ = bufferCount;

    // WAVEフォーマット設定
    WAVEFORMATEX wf{};
    wf.wFormatTag = WAVE_FORMAT_PCM;            // PCM形式
    wf.nChannels = 1;                           // モノラル
    wf.nSamplesPerSec = sampleRate;             // サンプリング周波数
    wf.wBitsPerSample = 16;                     // 16bitサンプル
    wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
    wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

    // 入力デバイスを開く (コールバック方式)
    if (waveInOpen(&hWaveIn_, WAVE_MAPPER, &wf, (DWORD_PTR)WaveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        // マイクが開けない
        return false;
    }

    // 出力デバイスを開く
    if (waveOutOpen(&hWaveOut_, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
    {
        waveInClose(hWaveIn_);
        hWaveIn_ = nullptr;    
        return false;
    }

    // バッファを用意
    buffers_.resize(bufferCount_);
    headers_.resize(bufferCount_);

    for (int i = 0; i < bufferCount_; i++)
    {
        // 録音用のバッファ準備
        buffers_[i] = new short[bufferSize_];
        ZeroMemory(&headers_[i], sizeof(WAVEHDR));
        headers_[i].lpData = (LPSTR)buffers_[i];
        headers_[i].dwBufferLength = bufferSize_ * sizeof(short);         // 初期状態
        waveInPrepareHeader(hWaveIn_, &headers_[i], sizeof(WAVEHDR));     // ヘッダーを準備
        waveInAddBuffer(hWaveIn_, &headers_[i], sizeof(WAVEHDR));         // デバイスに登録
    }

    outBuffer_ = new short[bufferSize_];
    return true;
}

void MicInput::Start()
{
    // 録音開始
    if (hWaveIn_) waveInStart(hWaveIn_);
}

void MicInput::Stop()
{
    if (hWaveIn_)
    {
        // 録音停止
        waveInStop(hWaveIn_);

        for (auto& hdr : headers_)
        {
            // バッファの解放
            waveInUnprepareHeader(hWaveIn_, &hdr, sizeof(WAVEHDR));
        }

        // 入力デバイスを閉じる
        waveInClose(hWaveIn_);
        hWaveIn_ = nullptr;
    }

    if (hWaveOut_)
    {
        // バッファの解放
        waveOutUnprepareHeader(hWaveOut_, &WaveOutHdr_, sizeof(WAVEHDR));

        // 出力デバイスを閉じる
        waveOutClose(hWaveOut_);
        hWaveOut_ = nullptr;
    }
}

void MicInput::Update()
{
   
}

int MicInput::GetLevel() const
{
    return level_;
}

int MicInput::GetPlayGameLevel() const
{
    // 最低値は返さない
    return (level_ < MIN_MIC_LEVEL) ?  0 : level_;
}

void MicInput::VoiceLevelDraw(void)
{
    VECTOR startPos = { 0, Application::SCREEN_SIZE_Y - 50 };

    int level = level_;
    level /= 10;   

    // 一定の音量を超えたら赤く描画する
    int color = (level > 400) ? 0xbb0000 : 0x0000bb;    
    for (int i = 0; i < level; i++) {

        DrawBox(
            (startPos.x + (i * 3)) + (i * 10),
            startPos.y, ((startPos.x + 10) + (i * 3)) + (i * 10),
            startPos.y + 50,
            color,
            true
        );
    }
}

// ===== コールバック =====
void CALLBACK MicInput::WaveInProc(HWAVEIN hwi, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2)
{
    if (msg == WIM_DATA)
    {
        MicInput* self = reinterpret_cast<MicInput*>(instance);
        WAVEHDR* hdr = reinterpret_cast<WAVEHDR*>(param1);
        self->OnBufferDone(hdr);
    }
}

void MicInput::OnBufferDone(WAVEHDR* hdr)
{
    // 音量計算
    long sum = 0;
    short* data = (short*)hdr->lpData;
    for (int i = 0; i < bufferSize_; i++) sum += abs(data[i]);
    level_ = static_cast<int>(sum / bufferSize_);

    // ノイズ除去（小さい音は無視）
    const int threshold = MIN_MIC_LEVEL;
    if (level_ < threshold) 
    {
        // 無音/ノイズ扱い
        waveInAddBuffer(hWaveIn_, hdr, sizeof(WAVEHDR));
        return;
    }

    // ===== バンドパスフィルタ =====
    float RC_low = 1.0 / (2 * M_PI * MAX_HZ);
    float RC_high = 1.0 / (2 * M_PI * MIN_HZ);
    float dt = 1.0 / sampleRate_;

    float alpha_low = dt / (RC_low + dt);
    float alpha_high = RC_high / (RC_high + dt);

    float low_pass_prev = 0.0;
    float high_pass_prev = 0.0;

    for (int i = 0; i < bufferSize_; i++) {
        double x = data[i];

        // ローパス（3400Hz以下）
        low_pass_prev = low_pass_prev + alpha_low * (x - low_pass_prev);

        // ハイパス（300Hz以上）
        high_pass_prev = alpha_high * (high_pass_prev + low_pass_prev - x);

        data[i] = (short)high_pass_prev;
    }

    //ループバック再生
    memcpy(outBuffer_, data, bufferSize_ * sizeof(short));
    WaveOutHdr_.lpData = (LPSTR)outBuffer_;
    WaveOutHdr_.dwBufferLength = bufferSize_ * sizeof(short);
    WaveOutHdr_.dwFlags = 0;
    waveOutPrepareHeader(hWaveOut_, &WaveOutHdr_, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut_, &WaveOutHdr_, sizeof(WAVEHDR));

    // 再度録音バッファとして登録
    waveInAddBuffer(hWaveIn_, hdr, sizeof(WAVEHDR));
}
