#include "MicManager.h"
#include <cstring>
#include <cmath>

MicManager::MicManager()
{
}

MicManager::~MicManager()
{
	// 終了時に録音を終了
	Stop();

	// バッファを開放	
	for (auto buf : buffers_) {
		delete[] buf;
	}

	buffers_.clear();

	if (outBuffer_) delete[] outBuffer_;
}

bool MicManager::Init(int sampleRate, int bufferSize, int bufferCount)
{
	sampleRate_ = sampleRate;
	bufferSize_ = bufferSize;
	bufferCount_ = bufferCount;

	// WAVEフォーマット設定
	WAVEFORMATEX wf{};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 1;
	wf.nSamplesPerSec = sampleRate_;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	// 入力デバイスを開く(コールバック方式)
	if (waveInOpen(&hWaveIn_, WAVE_MAPPER, &wf, (DWORD_PTR)WaveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
	{
		// マイクが開けなかった場合、falseを返す
		return false;
	}

	// 出力バッファを開く
	if(waveOutOpen(&hWaveOut_, WAVE_MAPPER, &wf, 0,0,CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		// 開けなかった場合、開いた入力デバイスを解放してfalseを返す
		waveInClose(hWaveIn_);
		hWaveIn_ = nullptr;
		return false;
	}

	// バッファを用意
	buffers_.resize(bufferCount_);
	headers_.resize(bufferCount_);

	for (int i = 0; i < bufferCount_; i++) {
		
		// 録音用のバッファ準備	
		buffers_[i] = new short[bufferSize_];		// 録音データを格納するメモリを確保
		ZeroMemory(&headers_[i], sizeof(WAVEHDR));	// ZeroMemoryでWAVEHDR構造体をゼロ初期化
		headers_[i].lpData = (LPSTR)buffers_[i];
		headers_[i].dwBufferLength = bufferSize_ * sizeof(short);
		waveInPrepareHeader(hWaveIn_, &headers_[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn_, &headers_[i], sizeof(WAVEHDR));		// ここで指定したバッファに録音していいと通知する
	}
}

void MicManager::Update(void)
{
}

void MicManager::Start(void)
{
}

void MicManager::Stop(void)
{
}

int MicManager::GetLevel(void) const
{
	return 0;
}

int MicManager::GetPlayGameLevel(void) const
{
	return 0;
}

void MicManager::WaveInProc(HWAVEIN hwi, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2)
{
}

void MicManager::OnBufferDone(WAVEHDR* hdr)
{
}
