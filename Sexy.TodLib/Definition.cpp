#include "Trail.h"
#include <assert.h>
#include "TodDebug.h"
#include "Definition.h"
#include "../ImageLib/zlib/zlib.h"
#include "../PakLib/PakInterface.h"
#include "../SexyAppFramework/PerfTimer.h"
#include "../SexyAppFramework/XMLParser.h"
#include "../Sexy.TodLib/TodCommon.h"

DefSymbol gTrailFlagDefSymbols[] = {  //0x69E150
    { 0, "Loops" },                 { -1, nullptr }
};
DefField gTrailDefFields[] = {  //0x69E160
    { "Image",            offsetof(TrailDefinition, mImage),           DefFieldType::DT_IMAGE,         nullptr },
    { "MaxPoints",        offsetof(TrailDefinition, mMaxPoints),       DefFieldType::DT_INT,           nullptr },
    { "MinPointDistance", offsetof(TrailDefinition, mMinPointDistance),DefFieldType::DT_FLOAT,         nullptr },
    { "TrailFlags",       offsetof(TrailDefinition, mTrailFlags),      DefFieldType::DT_FLAGS,         gTrailFlagDefSymbols },
    { "WidthOverLength",  offsetof(TrailDefinition, mWidthOverLength), DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "WidthOverTime",    offsetof(TrailDefinition, mWidthOverTime),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AlphaOverLength",  offsetof(TrailDefinition, mAlphaOverLength), DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AlphaOverTime",    offsetof(TrailDefinition, mAlphaOverTime),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "TrailDuration",    offsetof(TrailDefinition, mTrailDuration),   DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                 0x0,                                         DefFieldType::DT_INVALID,       nullptr }
};
DefMap gTrailDefMap = { gTrailDefFields, sizeof(TrailDefinition), TrailDefinitionConstructor };  //0x69D98C

DefSymbol gParticleFlagSymbols[] = {  //0x69E290
    {  0, "RandomLaunchSpin" },     {  1, "AlignLaunchSpin" },  {  2, "AlignToPixel" },     {  4, "ParticleLoops" },    {  3, "SystemLoops" },
    {  5, "ParticlesDontFollow" },  {  6, "RandomStartTime" },  {  7, "DieIfOverloaded" },  {  8, "Additive" },         {  9, "FullScreen" },
    { 10, "SoftwareOnly" },         { 11, "HardwareOnly" },     { -1, nullptr }
};
DefSymbol gEmitterTypeSymbols[] = {  //0x69E260
    {  0, "Circle" },               {  1, "Box" },              {  2, "BoxPath" },          {  3, "CirclePath" },       {  4, "CircleEvenSpacing" },
    { -1, nullptr }
};
DefSymbol gParticleTypeSymbols[] = {  //0x69E200
    {  1, "Friction" },             {  2, "Acceleration" },     {  3, "Attractor" },        {  4, "MaxVelocity" },      {  5, "Velocity" },
    {  6, "Position" },             {  7, "SystemPosition" },   {  8, "GroundConstraint" }, {  9, "Shake" },            { 10, "Circle" },
    { 11, "Away" },                 { -1, nullptr }
};

DefField gParticleFieldDefFields[] = {  //0x69E2F8
    { "FieldType",          offsetof(ParticleField, mFieldType), DefFieldType::DT_ENUM,          gParticleTypeSymbols },
    { "x",                  offsetof(ParticleField, mX),         DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "y",                  offsetof(ParticleField, mY),         DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                   0x0,                                 DefFieldType::DT_INVALID,       nullptr },
};
DefMap gParticleFieldDefMap = { gParticleFieldDefFields, sizeof(ParticleField), ParticleFieldConstructor };  //0x69E338

DefField gEmitterDefFields[] = {  //0x69E350
    { "Image",              offsetof(TodEmitterDefinition,mImage)               ,  DefFieldType::DT_IMAGE,         nullptr },
    { "ImageRow",           offsetof(TodEmitterDefinition,mImageRow)            ,  DefFieldType::DT_INT,           nullptr },
    { "ImageCol",           offsetof(TodEmitterDefinition,mImageCol)            ,  DefFieldType::DT_INT,           nullptr },
    { "ImageFrames",        offsetof(TodEmitterDefinition,mImageFrames)         ,  DefFieldType::DT_INT,           nullptr },
    { "Animated",           offsetof(TodEmitterDefinition,mAnimated)            ,  DefFieldType::DT_INT,           nullptr },
    { "ParticleFlags",      offsetof(TodEmitterDefinition,mParticleFlags)       ,  DefFieldType::DT_FLAGS,         gParticleFlagSymbols },
    { "EmitterType",        offsetof(TodEmitterDefinition,mEmitterType)         ,  DefFieldType::DT_ENUM,          gEmitterTypeSymbols },
    { "Name",               offsetof(TodEmitterDefinition,mName)                ,  DefFieldType::DT_STRING,        nullptr },
    { "SystemDuration",     offsetof(TodEmitterDefinition,mSystemDuration)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "OnDuration",         offsetof(TodEmitterDefinition,mOnDuration)          ,  DefFieldType::DT_STRING,        nullptr },
    { "CrossFadeDuration",  offsetof(TodEmitterDefinition,mCrossFadeDuration)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnRate",          offsetof(TodEmitterDefinition,mSpawnRate)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMinActive",     offsetof(TodEmitterDefinition,mSpawnMinActive)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMaxActive",     offsetof(TodEmitterDefinition,mSpawnMaxActive)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SpawnMaxLaunched",   offsetof(TodEmitterDefinition,mSpawnMaxLaunched)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterRadius",      offsetof(TodEmitterDefinition,mEmitterRadius)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterOffsetX",     offsetof(TodEmitterDefinition,mEmitterOffsetX)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterOffsetY",     offsetof(TodEmitterDefinition,mEmitterOffsetY)     ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterBoxX",        offsetof(TodEmitterDefinition,mEmitterBoxX)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterBoxY",        offsetof(TodEmitterDefinition,mEmitterBoxY)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterPath",        offsetof(TodEmitterDefinition,mEmitterPath)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterSkewX",       offsetof(TodEmitterDefinition,mEmitterSkewX)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "EmitterSkewY",       offsetof(TodEmitterDefinition,mEmitterSkewY)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleDuration",   offsetof(TodEmitterDefinition,mParticleDuration)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemRed",          offsetof(TodEmitterDefinition,mSystemRed)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemGreen",        offsetof(TodEmitterDefinition,mSystemGreen)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemBlue",         offsetof(TodEmitterDefinition,mSystemBlue)         ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemAlpha",        offsetof(TodEmitterDefinition,mSystemAlpha)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "SystemBrightness",   offsetof(TodEmitterDefinition,mSystemBrightness)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "LaunchSpeed",        offsetof(TodEmitterDefinition,mLaunchSpeed)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "LaunchAngle",        offsetof(TodEmitterDefinition,mLaunchAngle)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "Field",              offsetof(TodEmitterDefinition,mParticleFields)     ,  DefFieldType::DT_ARRAY,         &gParticleFieldDefMap },
    { "SystemField",        offsetof(TodEmitterDefinition,mSystemFields)       ,  DefFieldType::DT_ARRAY,         &gParticleFieldDefMap },
    { "ParticleRed",        offsetof(TodEmitterDefinition,mParticleRed)        ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleGreen",      offsetof(TodEmitterDefinition,mParticleGreen)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleBlue",       offsetof(TodEmitterDefinition,mParticleBlue)       ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleAlpha",      offsetof(TodEmitterDefinition,mParticleAlpha)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleBrightness", offsetof(TodEmitterDefinition,mParticleBrightness) ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleSpinAngle",  offsetof(TodEmitterDefinition,mParticleSpinAngle)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleSpinSpeed",  offsetof(TodEmitterDefinition,mParticleSpinSpeed)  ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleScale",      offsetof(TodEmitterDefinition,mParticleScale)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ParticleStretch",    offsetof(TodEmitterDefinition,mParticleStretch)    ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "CollisionReflect",   offsetof(TodEmitterDefinition,mCollisionReflect)   ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "CollisionSpin",      offsetof(TodEmitterDefinition,mCollisionSpin)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipTop",            offsetof(TodEmitterDefinition,mClipTop)            ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipBottom",         offsetof(TodEmitterDefinition,mClipBottom)         ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipLeft",           offsetof(TodEmitterDefinition,mClipLeft)           ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "ClipRight",          offsetof(TodEmitterDefinition,mClipRight)          ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "AnimationRate",      offsetof(TodEmitterDefinition,mAnimationRate)      ,  DefFieldType::DT_TRACK_FLOAT,   nullptr },
    { "",                   0x0,                                                  DefFieldType::DT_INVALID,       nullptr },
};
DefMap gEmitterDefMap = { gEmitterDefFields, sizeof(TodEmitterDefinition), TodEmitterDefinitionConstructor };  //0x69E344

DefField gParticleDefFields[] = {  //0x69E670
    { "Emitter",            offsetof(TodParticleDefinition,mEmitterDefs),        DefFieldType::DT_ARRAY,         &gEmitterDefMap },
    { "",                   0x0,        DefFieldType::DT_INVALID,       nullptr }
};
DefMap gParticleDefMap = { gParticleDefFields, sizeof(TodParticleDefinition), TodParticleDefinitionConstructor };  //0x69E690

DefField gReanimatorTransformDefFields[] = {  //0x69F088
    { "x",    offsetof(ReanimatorTransform,mTransX), DefFieldType::DT_FLOAT,         nullptr },
    { "y",    offsetof(ReanimatorTransform,mTransY), DefFieldType::DT_FLOAT,         nullptr },
    { "kx",   offsetof(ReanimatorTransform,mSkewX), DefFieldType::DT_FLOAT,         nullptr },
    { "ky",   offsetof(ReanimatorTransform,mSkewY), DefFieldType::DT_FLOAT,         nullptr },
    { "sx",   offsetof(ReanimatorTransform,mScaleX), DefFieldType::DT_FLOAT,         nullptr },
    { "sy",   offsetof(ReanimatorTransform,mScaleY), DefFieldType::DT_FLOAT,         nullptr },
    { "f",    offsetof(ReanimatorTransform,mFrame), DefFieldType::DT_FLOAT,         nullptr },
    { "a",    offsetof(ReanimatorTransform,mAlpha), DefFieldType::DT_FLOAT,         nullptr },
    { "i",    offsetof(ReanimatorTransform,mImage), DefFieldType::DT_IMAGE,         nullptr },
    { "font", offsetof(ReanimatorTransform,mFont), DefFieldType::DT_FONT,          nullptr },
    { "text", offsetof(ReanimatorTransform,mText), DefFieldType::DT_STRING,        nullptr },
    { "",     0, DefFieldType::DT_INVALID,       nullptr }
};
DefMap gReanimatorTransformDefMap = { gReanimatorTransformDefFields, sizeof(ReanimatorTransform), ReanimatorTransformConstructor };  //0x69F07C

DefField gReanimatorTrackDefFields[] = {  //0x69F148
    { "name",               offsetof(ReanimatorTrack,mName),      DefFieldType::DT_STRING,        nullptr },
    { "t",                  offsetof(ReanimatorTrack,mTransforms),DefFieldType::DT_ARRAY,         &gReanimatorTransformDefMap },
    { "",                   0x0,                                  DefFieldType::DT_INVALID,       nullptr }
};
DefMap gReanimatorTrackDefMap = { gReanimatorTrackDefFields, sizeof(ReanimatorTrack), ReanimatorTrackConstructor };  //0x69F178

DefField gReanimatorDefFields[] = {
    { "track",              offsetof(ReanimatorDefinition,mTracks),DefFieldType::DT_ARRAY,         &gReanimatorTrackDefMap },
    { "fps",                offsetof(ReanimatorDefinition,mFPS),   DefFieldType::DT_FLOAT,         nullptr },
    { "",                   0x0,                                   DefFieldType::DT_INVALID,       nullptr }
};  //0x69F184
DefMap gReanimatorDefMap = { gReanimatorDefFields, sizeof(ReanimatorDefinition), ReanimatorDefinitionConstructor };  //0x69F1B4

static DefLoadResPath gDefLoadResPaths[4] = { {"IMAGE_", ""}, {"IMAGE_", "particles\\"}, {"IMAGE_REANIM_", "reanim\\"}, {"IMAGE_REANIM_", "images\\"} };  //0x6A1A48

//0x5155A0
void* ParticleFieldConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ParticleField*)thePointer)->mX.mNodes = nullptr;
        ((ParticleField*)thePointer)->mX.mCountNodes = 0;
        ((ParticleField*)thePointer)->mY.mNodes = nullptr;
        ((ParticleField*)thePointer)->mY.mCountNodes = 0;
        ((ParticleField*)thePointer)->mFieldType = ParticleFieldType::FIELD_INVALID;
    }
    return thePointer;
}

//0x5155C0
void* TodEmitterDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        memset(thePointer, NULL, sizeof(TodEmitterDefinition));
        ((TodEmitterDefinition*)thePointer)->mImageFrames = 1;
        ((TodEmitterDefinition*)thePointer)->mEmitterType = EmitterType::EMITTER_BOX;
        ((TodEmitterDefinition*)thePointer)->mName = "";
        ((TodEmitterDefinition*)thePointer)->mOnDuration = "";
        ((TodEmitterDefinition*)thePointer)->mImageRow = 0;
        ((TodEmitterDefinition*)thePointer)->mImageCol = 0;
        ((TodEmitterDefinition*)thePointer)->mAnimated = 0;
        ((TodEmitterDefinition*)thePointer)->mImage = nullptr;
        ((TodEmitterDefinition*)thePointer)->mParticleFieldCount = 0;
    }
    return thePointer;
}

//0x515620
void* TodParticleDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((TodParticleDefinition*)thePointer)->mEmitterDefs = nullptr;
        ((TodParticleDefinition*)thePointer)->mEmitterDefCount = 0;
    }
    return thePointer;
}

void* TrailDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        memset(thePointer, 0, sizeof(TrailDefinition));
        ((TrailDefinition*)thePointer)->mMinPointDistance = 1.0f;
        ((TrailDefinition*)thePointer)->mMaxPoints = 2;
        ((TrailDefinition*)thePointer)->mTrailFlags = 0U;
        ((TrailDefinition*)thePointer)->mImage = nullptr;
    }
    return thePointer;
}

//0x471570
void* ReanimatorTransformConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorTransform*)thePointer)->mTransX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mTransY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mSkewX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mSkewY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mScaleX = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mScaleY = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mFrame = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mAlpha = DEFAULT_FIELD_PLACEHOLDER;
        ((ReanimatorTransform*)thePointer)->mImage = nullptr;
        ((ReanimatorTransform*)thePointer)->mFont = nullptr;
        ((ReanimatorTransform*)thePointer)->mText = "";
    }
    return thePointer;
}

//0x4715B0
void* ReanimatorTrackConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorTrack*)thePointer)->mName = "";
        ((ReanimatorTrack*)thePointer)->mTransforms = { nullptr, 0 };
    }
    return thePointer;
}

//0x4715D0
void* ReanimatorDefinitionConstructor(void* thePointer)
{
    if (thePointer)
    {
        ((ReanimatorDefinition*)thePointer)->mTracks = { nullptr, 0 };
        ((ReanimatorDefinition*)thePointer)->mFPS = 12.0f;
        ((ReanimatorDefinition*)thePointer)->mReanimAtlas = nullptr;
    }
    return thePointer;
}

void* DefinitionAlloc(int theSize)
{
    void* aPtr = operator new[](theSize);
    TOD_ASSERT(aPtr);
    memset(aPtr, 0, theSize);
    return aPtr;
}

//0x443BE0
bool DefinitionLoadImage(Image** theImage, const SexyString& theName)
{
    // 当贴图文件路径不存在时，无须获取贴图
    if (theName.size() == 0)
    {
        *theImage = nullptr;
        return true;
    }

    // 尝试借助资源管理器，从 XML 中加载贴图
    Image* anImage = (Image*)gSexyAppBase->mResourceManager->LoadImage(SexyStringToString(theName));
    if (anImage)
    {
        *theImage = anImage;
        return true;
    }

    // 从可能的贴图路径中手动加载贴图
    for (const DefLoadResPath& aLoadResPath : gDefLoadResPaths)
    {
        int aNameLen = theName.size();
        int aPrefixLen = strlen(aLoadResPath.mPrefix);
        if (aPrefixLen < aNameLen)
        {
            SexyString aPathToTry = StringToSexyString(aLoadResPath.mDirectory) + theName.substr(aPrefixLen, aNameLen);
            SharedImageRef aImageRef = gSexyAppBase->GetSharedImage(SexyStringToString("resourcepack/" + aPathToTry));
            if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(SexyStringToString("extension/" + aPathToTry));
            if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(SexyStringToString("dependency/" + aPathToTry));
            if ((Image*)aImageRef == nullptr) aImageRef = gSexyAppBase->GetSharedImage(SexyStringToString(aPathToTry));
            if ((Image*)aImageRef != nullptr)
            {
                TodHesitationTrace("Load Image '%s'", theName.c_str());
                TodAddImageToMap(&aImageRef, theName);
                TodMarkImageForSanding((Image*)aImageRef);
                *theImage = (Image*)aImageRef;
                return true;
            }
        }
    }
    return false;
}

//0x443F60
bool DefinitionLoadFont(Font** theFont, const SexyString& theName)
{
    Font* aFont = gSexyAppBase->mResourceManager->LoadFont(theName);
    *theFont = aFont;
    return aFont != nullptr;
}

bool DefinitionLoadXML(const SexyString& theFileName, DefMap* theDefMap, void* theDefinition)
{
    return DefinitionCompileAndLoad(theFileName, theDefMap, theDefinition);
}

void SMemR(void*& _Src, void* _Dst, size_t _Size)
{
    memcpy(_Dst, _Src, _Size);
    _Src = (void*)((unsigned int)_Src + _Size);
}
//0x444020
bool DefReadFromCacheArray(void*& theReadPtr, DefinitionArrayDef* theArray, DefMap* theDefMap)
{
    int aDefSize;
    SMemR(theReadPtr, &aDefSize, sizeof(int));  // 先读取一个整数表示 theDefMap 描述的定义结构类的大小
    if (aDefSize != theDefMap->mDefSize)  // 比较其与当前给出的定义结构图声明的大小是否一致
    {
        TodTrace("cache has old def: array size");
        return false;
    }
    if (theArray->mArrayCount == 0)  // 如果类中没有实例，则无需读取
        return true;

    int aArraySize = aDefSize * theArray->mArrayCount;
    void* pData = DefinitionAlloc(aArraySize);  // 申请内存并初始化填充为 0
    theArray->mArrayData = pData;
    SMemR(theReadPtr, pData, aArraySize);  // 仍然是粗略读取全部数据，然后再根据 theDefMap 的结构字段数组修复指针
    for (int i = 0; i < theArray->mArrayCount; i++)
        if (!DefMapReadFromCache(theReadPtr, theDefMap, (void*)((int)pData + theDefMap->mDefSize * i)))  // 最后一个参数表示 pData[i]
            return false;
    return true;
}

//0x4440B0
bool DefReadFromCacheFloatTrack(void*& theReadPtr, FloatParameterTrack* theTrack)
{
    int& aCountNodes = theTrack->mCountNodes;
    SMemR(theReadPtr, &aCountNodes, sizeof(int));
    if (aCountNodes > 0)
    {
        int aSize = aCountNodes * sizeof(FloatParameterTrackNode);
        FloatParameterTrackNode* aPtr = (FloatParameterTrackNode*)DefinitionAlloc(aSize);
        theTrack->mNodes = aPtr;
        SMemR(theReadPtr, aPtr, aSize);
    }
    return true;
}

//0x444110
bool DefReadFromCacheString(void*& theReadPtr, char** theString)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));
    TOD_ASSERT(aLen >= 0 && aLen <= 100000);
    if (aLen == 0)
        *theString = "";
    else
    {
        char* aPtr = (char*)DefinitionAlloc(aLen + 1);
        *theString = aPtr;
        SMemR(theReadPtr, aPtr, aLen);
        aPtr[aLen] = '\0';
    }
    return true;
}

//0x444180
bool DefReadFromCacheImage(void*& theReadPtr, Image** theImage)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));  // 读取贴图标签字符数组的长度
    char* aImageName = (char*)_alloca(aLen + 1);  // 在栈上分配贴图标签字符数组的内存空间
    SMemR(theReadPtr, aImageName, aLen);  // 读取贴图标签字符数组
    aImageName[aLen] = '\0';

    *theImage = nullptr;
    return aImageName[0] == '\0' || DefinitionLoadImage(theImage, StringToSexyString(aImageName));
}

//0x444220
bool DefReadFromCacheFont(void*& theReadPtr, Font** theFont)
{
    int aLen;
    SMemR(theReadPtr, &aLen, sizeof(int));  // 读取字体标签字符数组的长度
    SexyChar* aFontName = (SexyChar*)_alloca(aLen + 1);  // 在栈上分配字体标签字符数组的内存空间
    SMemR(theReadPtr, aFontName, aLen);  // 读取字体标签字符数组
    aFontName[aLen] = _S('\0');

    *theFont = nullptr;
    return aFontName[0] == _S('\0') || DefinitionLoadFont(theFont, aFontName);
}

//0x4442C0
bool DefMapReadFromCache(void*& theReadPtr, DefMap* theDefMap, void* theDefinition)
{
    // 分别确认每一个成员变量，并修复其中的指针类型和标志类型的变量
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        bool aSucceed = true;
        void* aDest = (void*)((int)theDefinition + aField->mFieldOffset);  // 指向该成员变量的指针
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_STRING:
            aSucceed = DefReadFromCacheString(theReadPtr, (char**)aDest);
            break;
        case DefFieldType::DT_ARRAY:
            aSucceed = DefReadFromCacheArray(theReadPtr, (DefinitionArrayDef*)aDest, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_IMAGE:
            aSucceed = DefReadFromCacheImage(theReadPtr, (Image**)aDest);
            break;
        case DefFieldType::DT_FONT:
            aSucceed = DefReadFromCacheFont(theReadPtr, (Font**)aDest);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            aSucceed = DefReadFromCacheFloatTrack(theReadPtr, (FloatParameterTrack*)aDest);
            break;
        }

        if (!aSucceed)
            return false;
    }
    return true;
}

//0x444380
uint DefinitionCalcHashSymbolMap(int aSchemaHash, DefSymbol* theSymbolMap)
{
    while (theSymbolMap->mSymbolName != nullptr)
    {
        aSchemaHash = crc32(aSchemaHash, (const Bytef*)theSymbolMap->mSymbolName, strlen(theSymbolMap->mSymbolName));
        aSchemaHash = crc32(aSchemaHash, (const Bytef*)&theSymbolMap->mSymbolValue, sizeof(int));
        theSymbolMap++;
    }
    return aSchemaHash;
}

//0x4443D0
uint DefinitionCalcHashDefMap(int aSchemaHash, DefMap* theDefMap, TodList<DefMap*>& theProgressMaps)
{
    for (TodListNode<DefMap*>* aNode = theProgressMaps.mHead; aNode != nullptr; aNode = aNode->mNext)
        if (aNode->mValue == theDefMap)
            return aSchemaHash;
    theProgressMaps.AddTail(theDefMap);

    aSchemaHash = crc32(aSchemaHash, (Bytef*)&theDefMap->mDefSize, sizeof(int));
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        aSchemaHash = crc32(aSchemaHash, (Bytef*)&aField->mFieldType, sizeof(DefFieldType));
        aSchemaHash = crc32(aSchemaHash, (Bytef*)&aField->mFieldOffset, sizeof(int));
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_ENUM:
        case DefFieldType::DT_FLAGS:
            aSchemaHash = DefinitionCalcHashSymbolMap(aSchemaHash, (DefSymbol*)aField->mExtraData);
            break;
        case DefFieldType::DT_ARRAY:
            aSchemaHash = DefinitionCalcHashDefMap(aSchemaHash, (DefMap*)aField->mExtraData, theProgressMaps);
            break;
        }
    }
    return aSchemaHash;
}

//0x444490
uint DefinitionCalcHash(DefMap* theDefMap)
{
    TodList<DefMap*> aProgressMaps;
    uint aResult = DefinitionCalcHashDefMap(crc32(0L, (Bytef*)Z_NULL, NULL) + 1, theDefMap, aProgressMaps);
    aProgressMaps.RemoveAll();
    return aResult;
}

//0x444500 : UnCompress(&theUncompressedSize, theCompressedBufferSize, esi = *theCompressedBuffer)
void* DefinitionUncompressCompiledBuffer(void* theCompressedBuffer, size_t theCompressedBufferSize, size_t& theUncompressedSize, const SexyString& theCompiledFilePath)
{
    auto sz = theCompressedBufferSize;
    // theCompressedBuffer 的前两个四字节存有特殊数据，此处检测其长度是否足够 8 字节（即 2 个四字节）
    if (theCompressedBufferSize < 8)
    {
        TodTrace("Compile def too small", theCompiledFilePath.c_str());
        return nullptr;
    }
    // 将 theCompressedBuffer 的前两个四字节视为一个 CompressedDefinitionHeader
    CompressedDefinitionHeader* aHeader = (CompressedDefinitionHeader*)theCompressedBuffer;
    if (aHeader->mCookie != 0xDEADFED4L)
    {
        TodTrace("Compiled fire cookie wrong: %s\n", theCompiledFilePath.c_str());
        return nullptr;
    }
    Bytef* aUncompressedBuffer = (Bytef*)DefinitionAlloc(aHeader->mUncompressedSize);
    theCompressedBufferSize = aHeader->mUncompressedSize; //my addition
    Bytef* aSrc = (Bytef*)((int)theCompressedBuffer + sizeof(CompressedDefinitionHeader));  // 实际解压数据从第 3 个四字节开始
    int aResult = uncompress(aUncompressedBuffer, (uLongf*)&theCompressedBufferSize, aSrc, sz - sizeof(CompressedDefinitionHeader));
    TOD_ASSERT(aResult == Z_OK);
    TOD_ASSERT(theCompressedBufferSize == aHeader->mUncompressedSize);
    theUncompressedSize = aHeader->mUncompressedSize;
    return aUncompressedBuffer;
}

//0x444560 : (void* def, *defMap, eax = string& compiledFilePath)  //esp -= 8
bool DefinitionReadCompiledFile(const SexyString& theCompiledFilePath, DefMap* theDefMap, void* theDefinition)
{
    PerfTimer aTimer;
    aTimer.Start();
    PFILE* pFile = p_fopen(SexyStringToString(theCompiledFilePath).c_str(), "rb");
    if (pFile)
    {
        p_fseek(pFile, 0, 2);  // Move the pointer to the read location to the end of the file
        size_t aCompressedSize = p_ftell(pFile);  // The offset obtained at this time is the size of the entire file
        p_fseek(pFile, 0, 0);  // Then move the pointer to the read position back to the beginning of the file
        void* aCompressedBuffer = DefinitionAlloc(aCompressedSize);
        // Read the file, and determine whether the actual read size is the complete file size, if it is not equal, it is determined that the read failed
        bool aReadCompressedFailed = p_fread(aCompressedBuffer, sizeof(char), aCompressedSize, pFile) != aCompressedSize;
        p_fclose(pFile);  // Close the resource file stream and free up the memory occupied by pFile
        if (aReadCompressedFailed)  // Determine whether the reading is successful
        {
            TodTrace("Failed to read compiled file: %s\n", theCompiledFilePath.c_str());
            delete[] aCompressedBuffer;
        }
        else
        {
            size_t aUncompressedSize;
            void* aUncompressedBuffer = DefinitionUncompressCompiledBuffer(aCompressedBuffer, aCompressedSize, aUncompressedSize, theCompiledFilePath);
            delete[] aCompressedBuffer;
            if (aUncompressedBuffer)
            {
                uint aDefHash = DefinitionCalcHash(theDefMap);  // Calculate the CRC check value, which will be used to detect the integrity of the data
                if (aUncompressedSize < theDefMap->mDefSize + sizeof(uint))  // Detect whether the length of the decompressed data is sufficient for the length of "define data + a check value to record data"
                    TodTrace("Compiled file size too small: %s\n", theCompiledFilePath.c_str());
                else
                {
                    // A pointer to copy a copy of the decompressed data is used to move when reading, and the original pointer will be used to calculate the size of the read area and delete[] operations in the future.
                    void* aBufferPtr = aUncompressedBuffer;
                    uint aCashHash;
                    SMemR(aBufferPtr, &aCashHash, sizeof(uint));  //Read the CRC check value of the record
                    if (aCashHash != aDefHash)  // Determine whether the check value is consistent, if it is inconsistent, the data is wrong
                        TodTrace("Compiled file schema wrong: %s\n", theCompiledFilePath.c_str());
                    else
                    {
                        // ☆ Officially started reading definition data ☆
                        // Roughly read the definition data of the original type of theDefinition for the first time, and gulp all the recorded data into theDefinition.
                        // At this time, all the data of theDefinition's original non-pointer type will be read correctly, but the variables of its pointer type will be read and assigned as wild pointers.
                        // / The problem of these wild pointers will be fixed in DefMapReadFromCache() with the help of the corresponding DEFIELD's MEXTRDATA in the future
                        SMemR(aBufferPtr, theDefinition, theDefMap->mDefSize);
                        // Repair the wild pointer and flag data, and save the result of whether it is successful, and use it as the return value later
                        bool aResult = DefMapReadFromCache(aBufferPtr, theDefMap, theDefinition);
                        size_t aReadMemSize = (uint)aBufferPtr - (uint)aUncompressedBuffer;
                        delete[] aUncompressedBuffer;
                        if (aResult && aReadMemSize != aUncompressedSize)
                            TodTrace("Compiled file wrong size: %s\n", theCompiledFilePath.c_str());
                        return aResult;
                    }
                }
            }
            delete[] aUncompressedBuffer;
        }
    }
    return false;
}

//0x444770
SexyString DefinitionGetCompiledFilePathFromXMLFilePath(const SexyString& theXMLFilePath)
{
    return StringToSexyString("compiled\\" + SexyStringToString(theXMLFilePath.c_str())+ ".compiled");
}

SexyString DefinitionGetCompiledFilePathFromXMLFilePathInDependency(const SexyString& theXMLFilePath)
{
    return StringToSexyString("dependency\\compiled\\" + SexyStringToString(theXMLFilePath.c_str()) + ".compiled");
}

SexyString DefinitionGetCompiledFilePathFromXMLFilePathInExtension(const SexyString& theXMLFilePath)
{
    return StringToSexyString("extension\\compiled\\" + SexyStringToString(theXMLFilePath.c_str()) + ".compiled");
}

SexyString DefinitionGetCompiledFilePathFromXMLFilePathInResourcePack(const SexyString& theXMLFilePath)
{
    return StringToSexyString("resourcepack\\compiled\\" + SexyStringToString(theXMLFilePath.c_str()) + ".compiled");
}

bool IsFileInPakFile(const SexyString& theFilePath)
{
    PFILE* pFile = p_fopen(theFilePath.c_str(), _S("rb"));
    bool aIsInPak = pFile && !pFile->mFP;  // 通过 mPakRecordMap.find 找到并打开的文件，其 mFP 为空指针（因为不是从实际文件中打开的）
    if (pFile)
    {
        p_fclose(pFile);
    }
    return aIsInPak;
}

bool DefinitionIsCompiled(const SexyString& theXMLFilePath)
{
    SexyString aCompiledFilePathInResourcePack = DefinitionGetCompiledFilePathFromXMLFilePathInResourcePack(theXMLFilePath);
    if (IsFileInPakFile(aCompiledFilePathInResourcePack))
        return true;

    SexyString aCompiledFilePathInExtension = DefinitionGetCompiledFilePathFromXMLFilePathInExtension(theXMLFilePath);
    if (IsFileInPakFile(aCompiledFilePathInExtension))
        return true;

    SexyString aCompiledFilePathInDependency = DefinitionGetCompiledFilePathFromXMLFilePathInDependency(theXMLFilePath);
    if (IsFileInPakFile(aCompiledFilePathInDependency))
        return true;

    SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (IsFileInPakFile(aCompiledFilePath))
        return true;

    _WIN32_FILE_ATTRIBUTE_DATA lpFileData;
    _FILETIME aCompiledFileTime;

    SexyString aPath;

    if (GetFileAttributesEx(aCompiledFilePathInResourcePack.c_str(), _GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &lpFileData)) aPath = aCompiledFilePathInResourcePack;
    if (GetFileAttributesEx(aCompiledFilePathInExtension.c_str(), _GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &lpFileData)) aPath = aCompiledFilePathInExtension;
    if (GetFileAttributesEx(aCompiledFilePathInDependency.c_str(), _GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &lpFileData)) aPath = aCompiledFilePathInDependency;
    else aPath = aCompiledFilePath;

    bool aSucceed = GetFileAttributesEx(aPath.c_str(), _GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &lpFileData);
    if (aSucceed)
        aCompiledFileTime = lpFileData.ftLastWriteTime;

    if (!GetFileAttributesEx(aPath.c_str(), _GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &lpFileData))
    {
        TodTrace("Can't file source file to compile '%s'", SexyStringToString(aPath.c_str()).c_str());
        return true;
    }
    else
    {
        return aSucceed && CompareFileTime(&aCompiledFileTime, &lpFileData.ftLastWriteTime) == 1;
    }
}

void DefinitionFillWithDefaults(DefMap* theDefMap, void* theDefinition)
{
    memset(theDefinition, NULL, theDefMap->mDefSize);  // 将 theDefinition 初始化填充为 0
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)  // 遍历 theDefinition 的每一个成员变量
        if (aField->mFieldType == DefFieldType::DT_STRING)
            *(char**)((uint)theDefinition + aField->mFieldOffset) = "";  // 将所有 char* 类型的成员变量赋值为空字符数组的指针
}

void DefinitionXmlError(XMLParser* theXmlParser, const char* theFormat, ...)
{
    va_list argList;
    va_start(argList, theFormat);
    std::string aFormattedMessage = vformat(theFormat, argList);
    va_end(argList);

    int aLine = theXmlParser->GetCurrentLineNum();
    SexyString aFileName = theXmlParser->GetFileName();
    TodTraceAndLog("%s(%d): XML Definition Error: %s\n", aFileName.c_str(), aLine, aFormattedMessage.c_str());
}

bool DefinitionReadXMLString(XMLParser* theXmlParser, SexyString& theValue)
{
    XMLElement aXMLElement;
    if (!theXmlParser->NextElement(&aXMLElement))  // 读取下一个 XML 元素
    {
        DefinitionXmlError(theXmlParser, "Missing element value");
        return false;
    }
    if (aXMLElement.mType == XMLElement::TYPE_END)  // 读取到结束标签则结束处理
        return true;
    else if (aXMLElement.mType != XMLElement::TYPE_ELEMENT)  // 除结束标签外，正常情况下，此处读取到的应是定义的正片内容
    {
        DefinitionXmlError(theXmlParser, "unknown element type");
        return false;
    }

    theValue = aXMLElement.mValue;  // ☆ 赋值出参

    if (!theXmlParser->NextElement(&aXMLElement))  // 继续读取下一个 XML 元素
    {
        DefinitionXmlError(theXmlParser, "Can't read element end");
        return false;
    }
    if (aXMLElement.mType != XMLElement::TYPE_END)  // 正常情况下，此处读取到的应是结束标签
    {
        DefinitionXmlError(theXmlParser, "Missing element end");
        return false;
    }
    return true;
}

bool DefSymbolValueFromString(DefSymbol* theSymbolMap, const char* theName, int* theResultValue)
{
    while (theSymbolMap->mSymbolName != nullptr)
    {
        if (stricmp(theName, theSymbolMap->mSymbolName) == 0)
        {
            *theResultValue = theSymbolMap->mSymbolValue;
            return true;
        }
        theSymbolMap++;
    }
    return false;
}

bool DefinitionReadIntField(XMLParser* theXmlParser, int* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%d"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadFloatField(XMLParser* theXmlParser, float* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%f"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse float value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadStringField(XMLParser* theXmlParser, SexyChar** theValue)
{
   SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (aStringValue.size() == 0)
    {
        *theValue = _S("");
    }
    else
    {
        *theValue = (SexyChar*)DefinitionAlloc(aStringValue.size());
        sexystrcpy(*theValue, aStringValue.c_str());
    }
    return true;
}

bool DefinitionReadEnumField(XMLParser* theXmlParser, int* theValue, DefSymbol* theSymbolMap)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefSymbolValueFromString(theSymbolMap, SexyStringToString(aStringValue).c_str(), theValue))
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse enum value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadVector2Field(XMLParser* theXmlParser, SexyVector2* theValue)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (sexysscanf(aStringValue.c_str(), _S("%f %f"), theValue) == 1)
        return true;

    DefinitionXmlError(theXmlParser, "Can't parse vector2 value '%s'", aStringValue.c_str());
    return false;
}

bool DefinitionReadArrayField(XMLParser* theXmlParser, DefinitionArrayDef* theArray, DefField* theField)
{
    DefMap* aDefMap = (DefMap*)theField->mExtraData;

    if (theArray->mArrayCount == 0)
    {
        theArray->mArrayCount = 1;
        theArray->mArrayData = DefinitionAlloc(aDefMap->mDefSize);
    }
    else
    {
        // 当 theArray 中已存在元素，且元素的个数为 2 的整数次幂时
        if (theArray->mArrayCount >= 1 && (theArray->mArrayCount == 1 || (theArray->mArrayCount & (theArray->mArrayCount - 1) == 0)))
        {
            void* anOldData = theArray->mArrayData;
            theArray->mArrayData = DefinitionAlloc(2 * theArray->mArrayCount * aDefMap->mDefSize);
            memcpy(theArray->mArrayData, anOldData, theArray->mArrayCount * aDefMap->mDefSize);
            delete[] anOldData;
        }
        theArray->mArrayCount++;
    }

    if (DefinitionLoadMap(theXmlParser, aDefMap, (unsigned char*)theArray->mArrayData + aDefMap->mDefSize * (theArray->mArrayCount - 1)))
        return true;

    DefinitionXmlError(theXmlParser, "failed to read sub def");
    return false;
}

bool DefinitionReadFloatTrackField(XMLParser* theXmlParser, FloatParameterTrack* theTrack)
{
    /*
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    */
    return true;
}

bool DefinitionReadFlagField(XMLParser* theXmlParser, const SexyString& theElementName, uint* theResultValue, DefSymbol* theSymbolMap)
{
    int aValue;
    if (!DefSymbolValueFromString(theSymbolMap, SexyStringToString(theElementName).c_str(), &aValue))
        return false;

    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    int aFlag;
    if (sexysscanf(aStringValue.c_str(), _S("%f %f"), &aFlag) != 1)
    {
        DefinitionXmlError(theXmlParser, "Can't parse int value '%s'", aStringValue.c_str());
        return false;
    }

    if (aFlag)
    {
        *theResultValue |= 1 << aValue;
    }
    else
    {
        *theResultValue &= ~(1 << aValue);
    }
    return true;
}

bool DefinitionReadImageField(XMLParser* theXmlParser, Image** theImage)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefinitionLoadImage(theImage, aStringValue))
        return true;

    std::string aMessgae = StrFormat("Failed to find image '%s' in %s", SexyStringToStringFast(aStringValue).c_str(), theXmlParser->GetFileName());
    TodErrorMessageBox(aMessgae.c_str(), "Missing image");
}

bool DefinitionReadFontField(XMLParser* theXmlParser, Font** theFont)
{
    SexyString aStringValue;
    if (!DefinitionReadXMLString(theXmlParser, aStringValue))
        return false;

    if (DefinitionLoadFont(theFont, aStringValue))
        return true;

    std::string aMessgae = StrFormat("Failed to find font '%s' in %s", SexyStringToStringFast(aStringValue).c_str(), theXmlParser->GetFileName());
    TodErrorMessageBox(aMessgae.c_str(), "Missing font");
}

bool DefinitionReadField(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition, bool* theDone)
{
    if (theXmlParser->HasFailed())
        return false;

    XMLElement aXMLElement;
    if (!theXmlParser->NextElement(&aXMLElement) || aXMLElement.mType == XMLElement::TYPE_END)  // 读取下一个 XML 元素
    {
        *theDone = true;  // 没有下一个元素则表示读取完成
        return true;
    }
    if (aXMLElement.mType != XMLElement::TYPE_START)  // 正常情况下，此处读取到的应是开始标签，而其他内容在后续的相应函数中读取
    {
        DefinitionXmlError(theXmlParser, "Missing element start");
        return false;
    }

    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        void* pVar = (void*)((uint)theDefinition + aField->mFieldOffset);
        if (aField->mFieldType == DefFieldType::DT_FLAGS && DefinitionReadFlagField(theXmlParser, aXMLElement.mValue, nullptr, (DefSymbol*)aField->mExtraData))
            return true;

        if (sexystricmp(aXMLElement.mValue.c_str(), StringToSexyString(aField->mFieldName).c_str()) == 0)  // 判断 aXMLElement 定义的是否为该成员变量
        {
            bool aSuccess;
            switch (aField->mFieldType)
            {
            case DefFieldType::DT_INT:
                aSuccess = DefinitionReadIntField(theXmlParser, (int*)pVar);
                break;
            case DefFieldType::DT_FLOAT:
                aSuccess = DefinitionReadFloatField(theXmlParser, (float*)pVar);
                break;
            case DefFieldType::DT_STRING:
                aSuccess = DefinitionReadStringField(theXmlParser, (SexyChar**)pVar);
                break;
            case DefFieldType::DT_ENUM:
                aSuccess = DefinitionReadEnumField(theXmlParser, (int*)pVar, (DefSymbol*)aField->mExtraData);
                break;
            case DefFieldType::DT_VECTOR2:
                aSuccess = DefinitionReadVector2Field(theXmlParser, (SexyVector2*)pVar);
                break;
            case DefFieldType::DT_ARRAY:
                aSuccess = DefinitionReadArrayField(theXmlParser, (DefinitionArrayDef*)pVar, aField);
                break;
            case DefFieldType::DT_TRACK_FLOAT:
                aSuccess = DefinitionReadFloatTrackField(theXmlParser, (FloatParameterTrack*)pVar);
                break;
            case DefFieldType::DT_IMAGE:
                aSuccess = DefinitionReadImageField(theXmlParser, (Image**)pVar);
                break;
            case DefFieldType::DT_FONT:
                aSuccess = DefinitionReadFontField(theXmlParser, (Font**)pVar);
                break;
            default:
                TOD_ASSERT(false);
                break;
            }
            if (aSuccess)
                return true;

            DefinitionXmlError(theXmlParser, "Failed to read '%s' field", aXMLElement.mValue.c_str());
            return false;
        }
    }
    DefinitionXmlError(theXmlParser, "Ignoring unknown element '%s'", aXMLElement.mValue.c_str());  // aXMLElement 未定义任何成员变量时
    return false;
}

bool DefinitionLoadMap(XMLParser* theXmlParser, DefMap* theDefMap, void* theDefinition)
{
    if (theDefMap->mConstructorFunc)
        theDefMap->mConstructorFunc(theDefinition);  // 利用构造函数构造 theDefinition
    else
        DefinitionFillWithDefaults(theDefMap, theDefinition);  // 以默认值填充 theDefinition

    bool aDone = false;
    while (!aDone)
        if (!DefinitionReadField(theXmlParser, theDefMap, theDefinition, &aDone))
            return false;
    return true;
}

bool DefinitionWriteCompiledFile(const std::string& theCompiledFilePath, DefMap* theDefMap, void* theDefinition)
{
    /*
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    ####################################################################################################
    */
    return true;
}

bool DefinitionCompileFile(const std::string theXMLFilePath, const std::string& theCompiledFilePath, DefMap* theDefMap, void* theDefinition)
{
    XMLParser aXMLParser;
    if (!aXMLParser.OpenFile(theXMLFilePath))
    {
        TodTrace("XML file not found: %s\n", theXMLFilePath.c_str());
        return false;
    }
    else if (!DefinitionLoadMap(&aXMLParser, theDefMap, theDefinition))
        return false;

    DefinitionWriteCompiledFile(theCompiledFilePath, theDefMap, theDefinition);
    return true;
}

//0x4447F0 : (void* def, *defMap, string& xmlFilePath)  //esp -= 0xC
bool DefinitionCompileAndLoad(const SexyString& theXMLFilePath, DefMap* theDefMap, void* theDefinition)
{
    //Changed to compile from in debug to this preprocessor
#ifdef _COMPILEXML  // 内测版执行的内容

    TodHesitationTrace(_S("predef"));
    SexyString aCompiledFilePathInResourcePack = DefinitionGetCompiledFilePathFromXMLFilePathInResourcePack(theXMLFilePath);
    SexyString aCompiledFilePathInExtension = DefinitionGetCompiledFilePathFromXMLFilePathInExtension(theXMLFilePath);
    SexyString aCompiledFilePathInDependency = DefinitionGetCompiledFilePathFromXMLFilePathInDependency(theXMLFilePath);
    SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);

    if (DefinitionIsCompiled(theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInResourcePack, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInResourcePack.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(theXMLFilePath, aCompiledFilePathInResourcePack, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInResourcePack.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInResourcePack.c_str());
            return true;
        }
    }

    if (DefinitionIsCompiled(theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInExtension, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInExtension.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(theXMLFilePath, aCompiledFilePathInExtension, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInExtension.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInExtension.c_str());
            return true;
        }
    }

    if (DefinitionIsCompiled(theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePathInDependency, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePathInDependency.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(theXMLFilePath, aCompiledFilePathInDependency, theDefMap, theDefinition);
        if (aResult)
        {
            TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePathInDependency.c_str());
            TodHesitationTrace(_S("compiled %s"), aCompiledFilePathInDependency.c_str());
            return true;
        }
    }

    if (DefinitionIsCompiled(theXMLFilePath) && DefinitionReadCompiledFile(aCompiledFilePath, theDefMap, theDefinition))
    {
        TodHesitationTrace(_S("loaded %s"), aCompiledFilePath.c_str());
        return true;
    }
    else
    {
        PerfTimer aTimer;
        aTimer.Start();
        bool aResult = DefinitionCompileFile(theXMLFilePath, aCompiledFilePath, theDefMap, theDefinition);
        TodTrace(_S("compile %d ms:'%s'"), (int)aTimer.GetDuration(), aCompiledFilePath.c_str());
        TodHesitationTrace(_S("compiled %s"), aCompiledFilePath.c_str());
        return aResult;
    }

#else  // 原版执行的内容
    SexyString aCompiledFilePathInResourcePack = DefinitionGetCompiledFilePathFromXMLFilePathInResourcePack(theXMLFilePath);
    if (DefinitionReadCompiledFile(aCompiledFilePathInResourcePack, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePathInExtension = DefinitionGetCompiledFilePathFromXMLFilePathInExtension(theXMLFilePath);
    if (DefinitionReadCompiledFile(aCompiledFilePathInExtension, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePathInDependency = DefinitionGetCompiledFilePathFromXMLFilePathInDependency(theXMLFilePath);
    if (DefinitionReadCompiledFile(aCompiledFilePathInDependency, theDefMap, theDefinition))
        return true;

    SexyString aCompiledFilePath = DefinitionGetCompiledFilePathFromXMLFilePath(theXMLFilePath);
    if (DefinitionReadCompiledFile(aCompiledFilePath, theDefMap, theDefinition))
        return true;

    TodErrorMessageBox(StrFormat("missing resource %s", aCompiledFilePath.c_str()).c_str(), "Error");
    exit(0);

#endif
}

//0x4448E0
float FloatTrackEvaluate(FloatParameterTrack& theTrack, float theTimeValue, float theInterp)
{
    if (theTrack.mCountNodes == 0)
        return 0.0f;

    if (theTimeValue < theTrack.mNodes[0].mTime)  // 如果当前时间小于第一个节点的开始时间
        return TodCurveEvaluate(theInterp, theTrack.mNodes[0].mLowValue, theTrack.mNodes[0].mHighValue, theTrack.mNodes[0].mDistribution);

    for (int i = 1; i < theTrack.mCountNodes; i++)
    {
        FloatParameterTrackNode* aNodeNxt = &theTrack.mNodes[i];
        if (theTimeValue <= aNodeNxt->mTime)  // 寻找首个开始时间大于当前时间的节点
        {
            FloatParameterTrackNode* aNodeCur = &theTrack.mNodes[i - 1];
            // 计算当前时间在〔当前节点至下一节点〕的过程中的进度
            float aTimeFraction = (theTimeValue - aNodeCur->mTime) / (aNodeNxt->mTime - aNodeCur->mTime);
            float aLeftValue = TodCurveEvaluate(theInterp, aNodeCur->mLowValue, aNodeCur->mHighValue, aNodeCur->mDistribution);
            float aRightValue = TodCurveEvaluate(theInterp, aNodeNxt->mLowValue, aNodeNxt->mHighValue, aNodeNxt->mDistribution);
            return TodCurveEvaluate(aTimeFraction, aLeftValue, aRightValue, aNodeCur->mCurveType);
        }
    }

    FloatParameterTrackNode* aLastNode = &theTrack.mNodes[theTrack.mCountNodes - 1];  // 如果当前时间大于最后一个节点的开始时间
    return TodCurveEvaluate(theInterp, aLastNode->mLowValue, aLastNode->mHighValue, aLastNode->mDistribution);
}

//0x4449F0
void FloatTrackSetDefault(FloatParameterTrack& theTrack, float theValue)
{
    if (theTrack.mNodes == nullptr && theValue != 0.0f)  // 确保该参数轨道无节点（未被赋值过）且给定的默认值不为 0
    {
        theTrack.mCountNodes = 1;  // 默认参数轨道有且仅有 1 个节点
        FloatParameterTrackNode* aNode = (FloatParameterTrackNode*)DefinitionAlloc(sizeof(FloatParameterTrackNode));
        theTrack.mNodes = aNode;
        aNode->mTime = 0.0f;
        aNode->mLowValue = theValue;
        aNode->mHighValue = theValue;
        aNode->mCurveType = TodCurves::CURVE_CONSTANT;
        aNode->mDistribution = TodCurves::CURVE_LINEAR;
    }
}

bool FloatTrackIsSet(const FloatParameterTrack& theTrack)
{
    return theTrack.mCountNodes != 0 && theTrack.mNodes[0].mCurveType != TodCurves::CURVE_CONSTANT;
}

bool FloatTrackIsConstantZero(FloatParameterTrack& theTrack)
{
    // 当轨道无节点，或仅存在一个节点且该节点的最大、最小值均为 0 时，认为该轨道上的值恒为零
    return theTrack.mCountNodes == 0 || (theTrack.mCountNodes == 1 && theTrack.mNodes[0].mLowValue == 0.0f && theTrack.mNodes[0].mHighValue == 0.0f);
}

//0x5167F0
float FloatTrackEvaluateFromLastTime(FloatParameterTrack& theTrack, float theTimeValue, float theInterp)
{
    return theTimeValue < 0.0f ? 0.0f : FloatTrackEvaluate(theTrack, theTimeValue, theInterp);
}

//0x444A50
void DefinitionFreeArrayField(DefinitionArrayDef* theArray, DefMap* theDefMap)
{
    for (int i = 0; i < theArray->mArrayCount; i++)
        DefinitionFreeMap(theDefMap, (void*)((int)theArray->mArrayData + theDefMap->mDefSize * i));  // 最后一个参数表示 pData[i]
    delete[] theArray->mArrayData;
    theArray->mArrayData = nullptr;
}

//0x444A90
void DefinitionFreeMap(DefMap* theDefMap, void* theDefinition)
{
    // 根据 theDefMap 遍历 theDefinition 的每个成员变量
    for (DefField* aField = theDefMap->mMapFields; *aField->mFieldName != '\0'; aField++)
    {
        void* aVar = (void*)((int)theDefinition + aField->mFieldOffset);  // 指向该成员变量的指针
        switch (aField->mFieldType)
        {
        case DefFieldType::DT_STRING:
            if (**(char**)aVar != '\0')
                delete[] * (char**)aVar;  // 释放字符数组
            *(char**)aVar = nullptr;
            break;
        case DefFieldType::DT_ARRAY:
            DefinitionFreeArrayField((DefinitionArrayDef*)aVar, (DefMap*)aField->mExtraData);
            break;
        case DefFieldType::DT_TRACK_FLOAT:
            if (((FloatParameterTrack*)aVar)->mCountNodes != 0)
                delete[]((FloatParameterTrack*)aVar)->mNodes;  // 释放浮点参数轨道的节点
            ((FloatParameterTrack*)aVar)->mNodes = nullptr;
            break;
        }
    }
}