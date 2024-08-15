#include <memory>
#include <random>

#include "SceneCreator.h"
#include "VBObjectExt.h"
#include "vbm.h"

bool SceneCreator::CreateOpaqueObjects(std::vector<std::shared_ptr<VBObjectExt>>& out)
{
    out.clear();

    // 创建一个随机数引擎
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 以random_device()作为种子的Mersenne Twister生成器
    // 创建一个在[0.0, 1.0)区间内均匀分布的随机数分布
    std::uniform_real_distribution<> dis(0.0, 1.0);

    const float STEP = 50;
    const float LENGTH = 400;
    const float YOFFSET = .0;

    out.clear();
    float startX = -LENGTH * 0.5f;
    float startZ = -LENGTH * 0.5f;
    for (float z = .0; z < LENGTH; z += STEP) {
        for (float x = .0; x < LENGTH; x += STEP) {
            std::shared_ptr<VBObjectExt> obj(new VBObjectExt());
            obj->LoadFromVBM("media/torus.vbm", 0, 1, 2);
            obj->SetColor(dis(gen), dis(gen), dis(gen), 1.0);
            obj->Translate(startX + x, YOFFSET, startZ + z);
            out.push_back(obj);
        }
    }

    return true;
}

bool SceneCreator::CreateTranslucentObjects(std::vector<std::shared_ptr<VBObjectExt>>& out)
{
    out.clear();

    // 创建一个随机数引擎
    std::random_device rd; // 用于获取随机种子
    std::mt19937 gen(rd()); // 以random_device()作为种子的Mersenne Twister生成器
    // 创建一个在[0.0, 1.0)区间内均匀分布的随机数分布
    std::uniform_real_distribution<> dis(0.0, 1.0);

    const float STEP = 50;
    const float LENGTH = 400;
    const float YOFFSET = 100.0;

    out.clear();
    float startX = -LENGTH * 0.5f;
    float startZ = -LENGTH * 0.5f;
    for (float z = .0; z < LENGTH; z += STEP) {
        for (float x = .0; x < LENGTH; x += STEP) {
            std::shared_ptr<VBObjectExt> obj(new VBObjectExt());
            obj->LoadFromVBM("media/torus.vbm", 0, 1, 2);
            obj->SetColor(dis(gen), dis(gen), dis(gen), dis(gen));
            obj->Translate(startX + x, YOFFSET, startZ + z);
            out.push_back(obj);
        }
    }

    return true;
}