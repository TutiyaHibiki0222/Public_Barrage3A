#include <filesystem>
#include "Path.hpp"
namespace fs = std::filesystem;

// リソースディレクトリのルートパス
inline const fs::path ResourcePath = "Resources";

// 各種リソースのサブディレクトリパス

inline const fs::path ImagesFilePath = ResourcePath / "Images";      // 画像リソースのディレクトリ
inline const fs::path LoadFilePath	 = ResourcePath / "LoadFile";    // 各種CSVなどの読み込み用ファイルのディレクトリ
inline const fs::path SoundsFilePath = ResourcePath / "Sounds";      // 音声リソースのディレクトリ

// サウンドの種類ごとのパス
inline const fs::path BGM_FilePath	= SoundsFilePath / "BGM";       // BGM（背景音楽）用ディレクトリ
inline const fs::path SE_FilePath	= SoundsFilePath / "SE";        // SE（効果音）用ディレクトリ

// パッチ情報（CSV）のディレクトリ
inline const fs::path FilePatchCSV = LoadFilePath / "FilePatchCSV";

// 画像アセットのパス定義（CSVファイルで管理）
inline const fs::path LoadBulletImageCSV	= FilePatchCSV / "BulletImagesPatch.csv";   // 敵や自機の弾画像のパス情報.
inline const fs::path LoadEffectImageCSV	= FilePatchCSV / "EffectImagesPatch.csv";   // エフェクト画像のパス情報.
inline const fs::path LoadPlayerImageCSV	= FilePatchCSV / "PlayerImagesPatch.csv";   // 自機関連の画像パス情報.
inline const fs::path LoadEnemyImageCSV		= FilePatchCSV / "EnemyImagesPatch.csv";    // 敵画像のパス情報.
inline const fs::path LoadStageImageCSV		= FilePatchCSV / "StageImagesPatch.csv";    // ステージ背景などの画像パス情報.
inline const fs::path LoadUIImageCSV		= FilePatchCSV / "UI_ImagesPatch.csv";      // UI用画像のパス情報.
inline const fs::path LoadItemImageCSV		= FilePatchCSV / "ItemImagesPatch.csv";     // アイテム用画像のパス情報.

inline const fs::path LoadAudioSE_CSV	= FilePatchCSV / "AudioSE_Patch.csv";	
inline const fs::path LoadAudioBGM_CSV	= FilePatchCSV / "AudioBGM_Patch.csv";

// 弾の画像or色or当たり判定をまとめたデータ.
inline const fs::path LoadBulletTypeDataJson = LoadFilePath / "BulletTypeData.json";

// ステージ1の敵出現データ（CSV形式）
inline const fs::path LoadStage1CSV = LoadFilePath / "EnemySpawn.csv";
inline const fs::path LoadStage2CSV = LoadFilePath / "EnemySpawn2.csv";
