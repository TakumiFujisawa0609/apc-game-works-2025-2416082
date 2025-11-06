#pragma once
#include <Windows.h>
#include <vector>


class MicManager
{
public:
	static constexpr float M_PI = 3.14159265358979323846f;

	static constexpr int MIN_MIC_LEVEL = 2000;

	static constexpr float MAX_HZ = 3400.0f;
	static constexpr float MIN_HZ = 300.0f;

	MicManager();
	~MicManager();

	// 初期化(サンプリング周波数とバッファサイズの指定)
	bool Init(int sampleRate = 44100, int bufferSize = 512, int bufferCount = 4);

	// 更新処理
	void Update(void);

	void Start(void);	// 録音開始
	void Stop(void);	// 録音停止

	// 音量の取得(0～32767 程度)
	int GetLevel(void) const;

	// 音量の取得(2000～32767 程度)
	int GetPlayGameLevel(void) const;

private:

	static void CALLBACK WaveInProc(HWAVEIN hwi, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2);

	void OnBufferDone(WAVEHDR* hdr);	//コールバックから呼ばれる

private:

	HWAVEIN hWaveIn_ = nullptr;		// マイク入力デバイス
	HWAVEOUT hWaveOut_ = nullptr;	// スピーカー出力デバイス

	int sampleRate_ = 44100;
	int bufferSize_ = 512;
	int bufferCount_ = 4;
	int level_ = 0;
	
	std::vector<short*> buffers_;	// 音声を録音するためのバッファを配列で用意
	std::vector<WAVEHDR> headers_;	// OSに登録するためのやつ

	// 出力用ヘッダ(再利用)
	WAVEHDR WaveOutHeader_{};
	short* outBuffer_ = nullptr;
};
